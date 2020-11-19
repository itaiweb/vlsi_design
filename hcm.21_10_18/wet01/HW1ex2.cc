#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include "hcm.h"
#include "flat.h"
#include <queue>
#include <algorithm>

using namespace std;

vector<hcmPort*> getInputPorts(hcmCell *topCell);
void topologicalOrdering(hcmCell* flatCell);
void dfs(hcmInstance* inst, vector<hcmInstance*> & topoSorted);
vector<hcmInstPort*> findOutput(map<string, hcmInstPort*> &instPorts);
void setRank(hcmInstance* inst);
void RankPropInit(hcmCell* flatCell);
vector<hcmInstance*> findAdjInst(hcmInstance* driver);
bool cmp(const pair<int, string> &a, const pair<int, string> &b);

// globals:
bool verbose = false;
///////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
	int argIdx = 1;
	int anyErr = 0;
	unsigned int i;
	vector<string> vlgFiles;
	
	if (argc < 3) {
		anyErr++;
	} else {
		if (!strcmp(argv[argIdx], "-v")) {
			argIdx++;
			verbose = true;
		}
		for (;argIdx < argc; argIdx++) {
			vlgFiles.push_back(argv[argIdx]);
		}
		
		if (vlgFiles.size() < 2) {
			cerr << "-E- At least top-level and single verilog file required for spec model" << endl;
			anyErr++;
		}
	}

	if (anyErr) {
		cerr << "Usage: " << argv[0] << "  [-v] top-cell file1.v [file2.v] ... \n";
		exit(1);
	}
 
	set< string> globalNodes;
	globalNodes.insert("VDD");
	globalNodes.insert("VSS");
	
	hcmDesign* design = new hcmDesign("design");
	string cellName = vlgFiles[0];
	for (i = 1; i < vlgFiles.size(); i++) {
		printf("-I- Parsing verilog %s ...\n", vlgFiles[i].c_str());
		if (!design->parseStructuralVerilog(vlgFiles[i].c_str())) {
			cerr << "-E- Could not parse: " << vlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}
	

	/*direct output to file*/
	string fileName = cellName + string(".rank");
	ofstream fv(fileName.c_str());
	if (!fv.good()) {
		cerr << "-E- Could not open file:" << fileName << endl;
		exit(1);
	}

	hcmCell *topCell = design->getCell(cellName);
	if (!topCell) {
		printf("-E- could not find cell %s\n", cellName.c_str());
		exit(1);
	}
	
	fv << "file name: " << fileName << endl;
	
	hcmCell *flatCell = hcmFlatten(cellName + string("_flat"), topCell, globalNodes);
	RankPropInit(flatCell);
	vector<hcmPort*> inPorts = getInputPorts(flatCell);
	vector <hcmInstance*> bfsSourceInsts;
	for (auto port = inPorts.begin(); port != inPorts.end(); port ++){
		for( auto instPort = (*port)->owner()->getInstPorts().begin(); instPort != (*port)->owner()->getInstPorts().end(); instPort++){
			bfsSourceInsts.push_back(instPort->second->getInst());
			instPort->second->getInst()->setProp("rank", 0);
		}	
	}

	//run BFS from each source that is connected directly to input ports
	for(auto inst = bfsSourceInsts.begin(); inst != bfsSourceInsts.end(); inst++){
		setRank(*inst);
	}

	vector<pair<int, string>> sortedVec;

	for(auto itr = flatCell->getInstances().begin(); itr != flatCell->getInstances().end(); itr++){
		int rank;
		itr->second->getProp("rank", rank);
		sortedVec.push_back(make_pair(rank, itr->first));
	}
	sort(sortedVec.begin(), sortedVec.end(), cmp);
	for(auto itr = sortedVec.begin(); itr != sortedVec.end(); itr++){
		if(itr->first == -1){ continue; }
		fv << itr->first << " " << itr->second << endl;
	}

	return(0);
}

//////////////////////////////////////////////////////////////////////////
// function name: RankPropInit
// description:   Initialaize "rank" property to all instances to -1.
// inputs:	  The flat model
// outputs:	  None. sets rank.
//////////////////////////////////////////////////////////////////////////

void RankPropInit(hcmCell* flatCell){
	for(auto itr = flatCell->getInstances().begin(); itr != flatCell->getInstances().end(); itr++){
		itr->second->setProp("rank", -1);
	}
}

//////////////////////////////////////////////////////////////////////////
// function name: setRank
// description:   Use BFS to traverse flat model and set the ranks.
// inputs:        Instances that are connected directly to outputs.
// outputs:	  None. sets rank.
//////////////////////////////////////////////////////////////////////////

void setRank(hcmInstance* inst){
	queue <hcmInstance* > instQ;
	instQ.push(inst);
	int rank;
	while(!instQ.empty()){
		hcmInstance* driver = instQ.front();
		instQ.pop();
		driver->getProp("rank", rank);
		vector<hcmInstance*> adjInst = findAdjInst(driver);
		for(auto instItr = adjInst.begin(); instItr != adjInst.end(); instItr++){
			int adjRank;
			(*instItr)->getProp("rank", adjRank);
			if(adjRank < rank + 1){
				(*instItr)->setProp("rank", rank + 1);
				instQ.push(*instItr);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// function name: findAdjInst
// description:   Find all the receivers of a driver cell.
// inputs:	  Driver cell instance.
// outputs:	  Vector of driven instances.
//////////////////////////////////////////////////////////////////////////

vector<hcmInstance*> findAdjInst(hcmInstance* driver){
	vector<hcmInstance*> res;
	vector<hcmInstPort*> instOut = findOutput(driver->getInstPorts());
	for(auto instPort = instOut.begin(); instPort != instOut.end(); instPort++){
		hcmNode* node = (*instPort)->getNode();
		for(auto innerInstPort = node->getInstPorts().begin(); innerInstPort != node->getInstPorts().end(); innerInstPort++){
			if(innerInstPort->second->getPort()->getDirection() == IN){
				res.push_back(innerInstPort->second->getInst());
			}
		}
	}
	return res;
}

//////////////////////////////////////////////////////////////////////////
// function name: cmp
// description:   Function sent to sort, to compare a pair (int, string).
// inputs:	  Two pairs to be compared.
// outputs:	  Bool value indicating which input is bigger.
//////////////////////////////////////////////////////////////////////////

bool cmp(const pair<int, string> &a, const pair<int, string> &b){
	if( a.first < b.first){
		return true;
	}
	else if(a.first == b.first){
		if(a.second < b.second){
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// function name: getInputPorts  
// description:   Find all the input ports of a given cell.
// inputs:	  The top cell.
// outputs:	  A vector of input ports.
//////////////////////////////////////////////////////////////////////////

vector<hcmPort*> getInputPorts(hcmCell *topCell){
	vector<hcmPort*> ports = topCell->getPorts();
	vector<hcmPort*> inPorts;
	for(auto iter = ports.begin(); iter != ports.end(); iter++){
		if ((*iter)->getDirection() == IN)
		{
			inPorts.push_back(*iter);
		}
	}
	return inPorts;
}

//////////////////////////////////////////////////////////////////////////
// function name: findOutput
// description:   Find all the output ports of a given instance.
// inputs:  	  A map of instports.
// outputs:	  A vector of output ports only.
//////////////////////////////////////////////////////////////////////////

vector<hcmInstPort*> findOutput(map<string, hcmInstPort*> &instPorts){
	vector<hcmInstPort*> outVec;
	for (auto i = instPorts.begin(); i != instPorts.end(); i++){
		if(i->second->getPort()->getDirection() == OUT){
			outVec.push_back(i->second);
		}
	}
	return outVec;
}

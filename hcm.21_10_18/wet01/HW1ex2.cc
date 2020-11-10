#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include "hcm.h"
#include "flat.h"
#include <queue>
#include <algorithm>
using namespace std;

bool verbose = false;
vector<hcmPort*> getInputPorts(hcmCell *topCell);
void topologicalOrdering(hcmCell* flatCell);
void dfs(hcmInstance* inst, vector<hcmInstance*> & topoSorted);
vector<hcmInstPort*> findOutput(map<string, hcmInstPort*> &instPorts);
void setRank(hcmInstance* inst);
void RankPropInit(hcmCell* flatCell);
vector<hcmInstance*> findAdjInst(hcmInstance* driver);
bool cmp(const pair<int, string> &a, const pair<int, string> &b);
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
	
	/* enter your code here */

	hcmCell *flatCell = hcmFlatten(cellName + string("_flat"), topCell, globalNodes);
	RankPropInit(flatCell);

	vector<hcmPort*> inPorts = getInputPorts(flatCell);
	for (auto port = inPorts.begin(); port != inPorts.end(); port ++){
		for( auto instPort = (*port)->owner()->getInstPorts().begin(); instPort != (*port)->owner()->getInstPorts().end(); instPort++){
			setRank(instPort->second->getInst());
		}	
	}

	vector<pair<int, string>> sortedVec;

	for(auto itr = flatCell->getInstances().begin(); itr != flatCell->getInstances().end(); itr++){
		int rank;
		itr->second->getProp("rank", rank);
		// cout << rank << "\t" << itr->first << endl;
		sortedVec.push_back(make_pair(rank, itr->first));
	}
	sort(sortedVec.begin(), sortedVec.end(), cmp);
	for(auto itr = sortedVec.begin(); itr != sortedVec.end(); itr++){
		fv << itr->first << " " << itr->second << endl;
	}

	return(0);
}


void RankPropInit(hcmCell* flatCell){
	for(auto itr = flatCell->getInstances().begin(); itr != flatCell->getInstances().end(); itr++){
		itr->second->setProp("rank", 0);
	}
}


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





//functions:
/*void topologicalOrdering(hcmCell* flatCell){	
	vector<hcmPort*> inPorts = getInputPorts(flatCell);
	vector<hcmInstance*> topoSorted;
	for( auto iter = flatCell->getInstances().begin(); iter != flatCell->getInstances().end(); iter++){
		iter->second->setProp("visited", 0);
	}
	map<string, hcmInstance*> instMap = flatCell->getInstances();
	for (auto j = instMap.begin(); j != instMap.end(); j++){
		//cout << "run dfs: " << j->first << endl;
		dfs(j->second, topoSorted);
	}
	for (auto i = topoSorted.begin(); i != topoSorted.end(); i++)
	{
		cout << (*i)->getName() << endl;
	}
	
}

void dfs(hcmInstance* inst, vector<hcmInstance*> & topoSorted){
	int visit;
	inst->getProp("visited", visit);
	if(visit){
		return;
	}
	inst->setProp("visited", 1);
	map<string, hcmInstPort*> instPorts = inst->getInstPorts();
	vector<hcmInstPort*> output = findOutput(instPorts);
	for(auto i = output.begin(); i != output.end(); i++){
		for (auto j = (*i)->getNode()->getInstPorts().begin(); j != (*i)->getNode()->getInstPorts().end(); j++){ //page 10 in tutorial. go throug all connections forward.
			dfs(j->second->getInst(), topoSorted);
		}
	}
	topoSorted.push_back(inst);
	return;
}
*/
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

vector<hcmInstPort*> findOutput(map<string, hcmInstPort*> &instPorts){
	vector<hcmInstPort*> outVec;
	for (auto i = instPorts.begin(); i != instPorts.end(); i++){
		if(i->second->getPort()->getDirection() == OUT){
			outVec.push_back(i->second);
		}
	}
	return outVec;
}

#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "hcm.h"
#include "flat.h"
void setVisitProp(hcmCell* topCell);
void dfs(hcmCell* topCell, string name, int dep);
void foldedDfs(hcmCell* topCell, int & and4Folded);
void deepestReachDfs(hcmInstPort* instPort, int dep, int &globDep); 
using namespace std;

bool verbose = false;
vector <string> globalDeepestVec;
int globalMaxDeep = 0;
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
	

	/*direct to file*/
	string fileName = cellName + string(".stat");
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
	
	hcmCell *flatCell = hcmFlatten(cellName + string("_flat"), topCell, globalNodes);
	cout << "-I- Top cell flattened" << endl;
	
	fv << "file name: " << fileName << endl;
	

	// section a
	int aCnt = 0;
	for(auto aItr = topCell->getNodes().begin(); aItr != topCell->getNodes().end(); aItr++){
		if(aItr->first == "VDD" || aItr->first == "VSS"){
			continue;
		}
		aCnt++;
	}
	fv << "a: " << aCnt << endl;

	// section b
	fv << "b: " << (topCell->getInstances()).size() << endl;

	//section c 
	int and4Folded = 0;
	setVisitProp(topCell); //setting all master cells prop visited = 0
	foldedDfs(topCell, and4Folded);
	fv << "c: " << and4Folded << endl;

	//section d
	int and4FlatCnt = 0;
	map< std::string, hcmInstance* >::iterator it = flatCell->getInstances().begin();
	while (it != flatCell->getInstances().end()){
		string name = it->second->masterCell()->getName();
		if (name == "and4")
		{
			and4FlatCnt++;
		}
		it++;
	}
	fv << "d: " << and4FlatCnt << endl;
	
	//section e
	int deepestReach = 1;
	map<string, hcmNode*> toplevelNodes = topCell->getNodes();
	for(auto nodeItr = toplevelNodes.begin(); nodeItr != toplevelNodes.end(); nodeItr++){
		map<string, hcmInstPort*> instPorts = nodeItr->second->getInstPorts();
		for(auto instPortItr = instPorts.begin(); instPortItr != instPorts.end(); instPortItr++){
			deepestReachDfs(instPortItr->second, 2, deepestReach); 
		}
	}
	fv << "e: " << deepestReach << endl;

	//section f
	dfs(topCell, "", 0);
	sort(globalDeepestVec.begin(), globalDeepestVec.end());
	vector<string>::iterator nodeIt = globalDeepestVec.begin();
	while (nodeIt != globalDeepestVec.end())
	{
		nodeIt->erase(0,1);
		fv << nodeIt->c_str() << endl;
		nodeIt++;
	}

	return(0);
}

void setVisitProp(hcmCell*topCell){
	topCell->setProp("visited", 0);
	for(auto itr = topCell->getInstances().begin(); itr != topCell->getInstances().end(); itr++){
		setVisitProp(itr->second->masterCell());
	}
}


void foldedDfs(hcmCell* topCell, int & and4Folded){
	int isVisit;
	topCell->getProp("visited", isVisit);
	if(isVisit){
		return;
	}
	topCell->setProp("visited", 1);
	for (auto itr = topCell->getInstances().begin(); itr != topCell->getInstances().end(); itr++){
		if(itr->second->masterCell()->getName() == "and4"){
			and4Folded++;
		}
		foldedDfs(itr->second->masterCell(), and4Folded);
	}
}


void dfs(hcmCell* topCell, string name, int dep){
	map< std::string, hcmInstance* >::iterator instIt = topCell->getInstances().begin();
	while (instIt != topCell->getInstances().end()){
		dfs(instIt->second->masterCell(), name + "/" + instIt->first, dep + 1);
		instIt++;
	}
	if (dep >= globalMaxDeep){
		if(dep > globalMaxDeep){
			globalMaxDeep = dep;
			globalDeepestVec.clear();
		}
		map< std::string, hcmNode* >::iterator nodeIt = topCell->getNodes().begin();
		while(nodeIt != topCell->getNodes().end()){
			if(nodeIt->first == "VDD" || nodeIt->first == "VSS"){
				nodeIt++;
				continue;
			}
			globalDeepestVec.push_back(name + "/" + nodeIt->first);
			nodeIt++;
		}
	}
}

void deepestReachDfs(hcmInstPort* instPort, int dep, int & globDep){
	hcmNode * node = instPort->getPort()->owner();
	map<string, hcmInstPort*> instPorts = node->getInstPorts();
	for(auto instPortItr = instPorts.begin(); instPortItr != instPorts.end(); instPortItr++) {
		deepestReachDfs(instPortItr->second, dep + 1, globDep);
	}
	if (dep > globDep ){
		globDep = dep;
	}
}

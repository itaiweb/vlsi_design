#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "hcm.h"
#include "flat.h"
void dfs(hcmCell* topCell, string name, int dep);
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
	fv << "The number of nodes in the top level cell is: " << aCnt << endl; // TODO: check definition of nodes 

	// section b
	fv << "The number of instances in the top level cell is: " << (topCell->getInstances()).size() << endl;

	//section c 
	//TODO: check forum for answers 

	//section d
	int and4Cnt = 0;
	map< std::string, hcmInstance* >::iterator it = flatCell->getInstances().begin();
	while (it != flatCell->getInstances().end()){
		string name = it->second->masterCell()->getName();
		if (name == "and4")
		{
			and4Cnt++;
		}
		it++;
	}
	fv << "The number of instances of cell and4 in the entire hierarchy is: " << and4Cnt << endl;
	
	//section e
	dfs(topCell, topCell->getName(), 0);
	fv << "There are " << globalMaxDeep << " of heirarchy traverses" << endl;

	//section f
	//cout << "before sort" << endl;
	sort(globalDeepestVec.begin(), globalDeepestVec.end());
	//cout << "after sort" << endl;
	vector<string>::iterator nodeIt = globalDeepestVec.begin();
	while (nodeIt != globalDeepestVec.end())
	{
		fv << nodeIt->c_str() << endl;
		nodeIt++;
	}
	


	


	return(0);
}


void dfs(hcmCell* topCell, string name, int dep){
	//cout << topCell->getName() << endl;
	map< std::string, hcmInstance* >::iterator instIt = topCell->getInstances().begin();
	while (instIt != topCell->getInstances().end()){
		dfs(instIt->second->masterCell(), name + "/" + instIt->first, dep + 1);
		instIt++;
	}
	//cout << topCell->getName() << ": ended loop" << endl;
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
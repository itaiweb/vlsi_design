#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include "hcm.h"
#include "flat.h"

using namespace std;

bool verbose = false;
vector<hcmPort*> getInputPorts(hcmCell *topCell);
void topologicalOrdering(hcmCell* flatCell);
void dfs(hcmInstance* inst, vector<hcmInstance*> & topoSorted);
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
	vector<hcmPort*> inPorts = flatCell->getPorts();
	topologicalOrdering(flatCell);
	
	return(0);
}

void topologicalOrdering(hcmCell* flatCell){	
	vector<hcmPort*> inPorts = getInputPorts(flatCell);
	vector<hcmInstance*> topoSorted;
	for( auto iter = flatCell->getInstances().begin(); iter != flatCell->getInstances().end(); iter++){
		iter->second->setProp("visited", 0);
	}
	map<string, hcmInstance*> instMap = flatCell->getInstances();
	for (auto j = instMap.begin(); j != instMap.end(); j++){
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
	for (auto j = instPorts.begin(); j != instPorts.end(); j++){
		dfs(j->second->getInst(), topoSorted);
	}
	topoSorted.push_back(inst);
	return;
}

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
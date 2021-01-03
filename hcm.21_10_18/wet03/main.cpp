#include "helper.h"

int debugCounter = 0;

using namespace std;
using namespace Minisat;

//globals:
bool verbose = false;

///////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	int argIdx = 1;
	int anyErr = 0;
	unsigned int i;
	vector<string> specVlgFiles;
	vector<string> implementationVlgFiles;
	string specCellName;
	string implementationCellName;
	
	if (argc < 3) {
		anyErr++;
	} else {
		if (!strcmp(argv[argIdx], "-v")) {
			argIdx++;
			verbose = true;
		}
		if (!strcmp(argv[argIdx], "-s")) {
			argIdx++;
			specCellName = argv[argIdx];
			argIdx++;
			while(strcmp(argv[argIdx], "-i")){
				specVlgFiles.push_back(argv[argIdx]);
				argIdx++;
			}
		}
		argIdx++;
		implementationCellName = argv[argIdx];
		argIdx++;
		for (;argIdx < argc; argIdx++) {
			implementationVlgFiles.push_back(argv[argIdx]);
		}
		
		if (implementationVlgFiles.size() < 2 || specVlgFiles.size() < 2) {
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
	
	// spec hcm
	hcmDesign* specDesign = new hcmDesign("specDesign");
	for (i = 0; i < specVlgFiles.size(); i++) {
		printf("-I- Parsing verilog %s ...\n", specVlgFiles[i].c_str());
		if (!specDesign->parseStructuralVerilog(specVlgFiles[i].c_str())) {
			cerr << "-E- Could not parse: " << specVlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}

	hcmCell *topSpecCell = specDesign->getCell(specCellName);
	if (!topSpecCell) {
		printf("-E- could not find cell %s\n", specCellName.c_str());
		exit(1);
	}
	
	hcmCell *flatSpecCell = hcmFlatten(specCellName + string("_flat"), topSpecCell, globalNodes);

	// implementation hcm
	hcmDesign* impDesign = new hcmDesign("impDesign");
	for (i = 0; i < implementationVlgFiles.size(); i++) {
		printf("-I- Parsing verilog %s ...\n", implementationVlgFiles[i].c_str());
		if (!impDesign->parseStructuralVerilog(implementationVlgFiles[i].c_str())) {
			cerr << "-E- Could not parse: " << implementationVlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}
	
	hcmCell *topImpCell = impDesign->getCell(implementationCellName);
	if (!topImpCell) {
		printf("-E- could not find cell %s\n", implementationCellName.c_str());
		exit(1);
	}
	
	hcmCell *flatImpCell = hcmFlatten(implementationCellName + string("_flat"), topImpCell, globalNodes);

	int nodeNum = 1;
	Solver s;
	map<string,int> inputs;

	setSpecCellNodes(s, nodeNum, flatSpecCell, inputs);
	setImpCellNodes(s, nodeNum, flatImpCell, inputs);

	setGlobalNodes(s, nodeNum, flatSpecCell, flatImpCell);


	//TODO: make cnf.
	//TODO: make pdf.
	//TODO: code cosmetics.

	for(map<string, hcmInstance*>::iterator instItr = flatSpecCell->getInstances().begin(); instItr != flatSpecCell->getInstances().end(); instItr++){
		addGateClause(instItr->second, s);
	}
	
	for(map<string, hcmInstance*>::iterator instItr = flatImpCell->getInstances().begin(); instItr != flatImpCell->getInstances().end(); instItr++){
		addGateClause(instItr->second, s);
	}
	
	connectCircuitOutputs(s, nodeNum, flatSpecCell, flatImpCell);
	makeFFXor(s, nodeNum, flatSpecCell, flatImpCell);
	s.addClause(mkLit(nodeNum - 1)); // add final output.

	s.toDimacs("DIMACS.cnf");
	cout << (nodeNum - 1) << endl;	
	cout << debugCounter << endl;

	s.simplify();
	int sat = s.solve();
	printf("is sat? %d\n", sat);


	for(int i=0; i<s.nVars(); i++){
		printf("%d = %s\n", i, (s.model.size() == 0) ? "Undef" : (s.model[i] == l_True ? "+" : "-"));
	}
	return 0;
	
}	
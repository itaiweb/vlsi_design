#include "helper.h"

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
	for(auto nodeItr = flatSpecCell->getNodes().begin(); nodeItr != flatSpecCell->getNodes().end(); nodeItr++){
		nodeItr->second->setProp("num", nodeNum);
		nodeNum++;
	}

	for(auto nodeItr = flatImpCell->getNodes().begin(); nodeItr != flatImpCell->getNodes().end(); nodeItr++){
		nodeItr->second->setProp("num", nodeNum);
		nodeNum++;
	}

	vec<Lit> clauseVec;
	Solver s;

	for(auto instItr = flatSpecCell->getInstances().begin(); instItr != flatSpecCell->getInstances().end(); instItr++){
		makeClause(instItr->second, clauseVec);
		s.addClause(clauseVec);
		clauseVec.clear();
	}

	for(auto instItr = flatSpecCell->getInstances().begin(); instItr != flatSpecCell->getInstances().end(); instItr++){
		makeClause(instItr->second, clauseVec);
		s.addClause(clauseVec);
		clauseVec.clear();
	}

	for(auto outputItr = flatSpecCell->getPorts().begin(); outputItr != flatSpecCell->getPorts().end(); outputItr++){
		hcmPort* impCellPort;
		int node1Num;
		int node2Num;
		if((*outputItr)->getDirection() == OUT){
			impCellPort = flatImpCell->getPort((*outputItr)->getName());
			hcmNode* node1 = impCellPort->owner();
			hcmNode* node2 = (*outputItr)->owner();
			node1->getProp("num", node1Num);
			node2->getProp("num", node2Num);
			//makeXORclause(node1Num,node2Num,clauseVec, counter);
		}
	}


	// Solver S;
	// S.newVar();
	// S.newVar();

	// vec<Lit> cv;
	// cv.push( mkLit(1) );
	// cv.push( ~mkLit(1) );
	// S.addClause(cv); cv.clear();

	// S.simplify();
	// int sat = S.solve();
	// printf("%d\n", sat);
	// for(int i=0; i<S.nVars(); i++){
	// 	printf("%d = %s\n", i+1, (S.model[i] == l_Undef) ? "Undef" : (S.model[i] == l_True ? "+" : "-"));
	// }
	
}	
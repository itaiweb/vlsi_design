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

	int nodeNum = 0;
	Solver s;
	map<string,int> inputs;

	for(map<string, hcmNode*>::iterator nodeItr = flatSpecCell->getNodes().begin(); nodeItr != flatSpecCell->getNodes().end(); nodeItr++){
		if(nodeItr->second->getName() == "VSS" || nodeItr->second->getName() == "VDD" || nodeItr->second->getName() == "CLK") {continue;}
		nodeItr->second->setProp("num", nodeNum);
		if(nodeItr->second->getPort() != NULL) {
			if(nodeItr->second->getPort()->getDirection() == IN){
				inputs[nodeItr->second->getPort()->getName()] = nodeNum;
			}
		}
		for( map<string, hcmInstPort*>::iterator portItr = nodeItr->second->getInstPorts().begin(); 
							portItr != nodeItr->second->getInstPorts().end(); portItr++){
			if(portItr->second->getInst()->masterCell()->getName() == "dff"){
				if(portItr->second->getPort()->getDirection() == OUT){
					inputs[portItr->second->getInst()->getName()] = nodeNum;
					cout << "spec cell inst name: " << portItr->second->getInst()->getName() << endl;
					cout << "spec cell inputs[instname]: " << inputs[portItr->second->getInst()->getName()] << endl;
					cout << nodeItr->second->getName() << endl;
				}
			}
		}
		s.newVar();
		cout << "node " << nodeNum << " is " << nodeItr->first << endl;
		nodeNum++;
	}

	for(map<string, hcmNode*>::iterator nodeItr = flatImpCell->getNodes().begin(); nodeItr != flatImpCell->getNodes().end(); nodeItr++){
		if(nodeItr->second->getName() == "VSS" || nodeItr->second->getName() == "VDD" || nodeItr->second->getName() == "CLK") {continue;}
		if(nodeItr->second->getPort() != NULL){
			if(nodeItr->second->getPort()->getDirection() == IN){
				string inputName = nodeItr->second->getPort()->getName();
				if(inputs.find(inputName) != inputs.end()){
					nodeItr->second->setProp("num", inputs[inputName]);
					continue;
				}
			}
		}
		bool foundFF = false;
		for( map<string, hcmInstPort*>::iterator portItr = nodeItr->second->getInstPorts().begin(); 
							portItr != nodeItr->second->getInstPorts().end(); portItr++){
			if(portItr->second->getInst()->masterCell()->getName() == "dff"){
				if(portItr->second->getPort()->getDirection() == OUT){
					string instName = portItr->second->getInst()->getName();
					cout << "imp cell inst name: " << instName << endl;
					cout << "imp cell inputs[instname]: " << inputs[instName] << endl;
					cout << nodeItr->second->getName() << endl;
					nodeItr->second->setProp("num", inputs[instName]);
					foundFF = true;
					break;
				}
			}
		}
		if(foundFF) {continue;}
		nodeItr->second->setProp("num", nodeNum);
		s.newVar();
		cout << "node " << nodeNum << " is " << nodeItr->first << endl;
		nodeNum++;
	}

	vec<Lit> clauseVec;
	
	//add VSS and VSS clauses.
	// int ConstNum;
	// flatSpecCell->getNode("VDD")->getProp("num", ConstNum);
	// clauseVec.push(mkLit(ConstNum));
	// s.addClause(clauseVec);
	// clauseVec.clear();
	// flatSpecCell->getNode("VSS")->getProp("num", ConstNum);
	// clauseVec.push(~mkLit(ConstNum));
	// s.addClause(clauseVec);
	// clauseVec.clear();

	//TODO: recognize VDD VSS.
	//TODO: make cnf.
	//TODO: make pdf.
	//TODO: code cosmetics.

	for(map<string, hcmInstance*>::iterator instItr = flatSpecCell->getInstances().begin(); instItr != flatSpecCell->getInstances().end(); instItr++){
		addGateClause(instItr->second, s);
	}
	
	for(map<string, hcmInstance*>::iterator instItr = flatImpCell->getInstances().begin(); instItr != flatImpCell->getInstances().end(); instItr++){
		addGateClause(instItr->second, s);
	}
	
	makeOutputXor(s, nodeNum, flatSpecCell, flatImpCell);
	makeFFXor(s, nodeNum, flatSpecCell, flatImpCell);
	s.addClause(mkLit(nodeNum - 1)); // add final output.
	
	s.simplify();
	int sat = s.solve();
	printf("is sat? %d\n", sat);


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
	for(int i=0; i<s.nVars(); i++){
		printf("%d = %s\n", i, (s.model.size() == 0) ? "Undef" : (s.model[i] == l_True ? "+" : "-"));
	}
	return 0;
	
}	
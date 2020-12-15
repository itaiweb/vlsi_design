#include "helper.h"

using namespace std;

//globals:
bool verbose = false;

bool debugFlag = 0;

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
	string signalTextFile = vlgFiles[1];
	string vectorTextFile = vlgFiles[2];
	for (i = 3; i < vlgFiles.size(); i++) {
		printf("-I- Parsing verilog %s ...\n", vlgFiles[i].c_str());
		if (!design->parseStructuralVerilog(vlgFiles[i].c_str())) {
			cerr << "-E- Could not parse: " << vlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}

	hcmCell *topCell = design->getCell(cellName);
	if (!topCell) {
		printf("-E- could not find cell %s\n", cellName.c_str());
		exit(1);
	}
	
	hcmCell *flatCell = hcmFlatten(cellName + string("_flat"), topCell, globalNodes);
	cout << "-I- Top cell flattened" << endl;
	for(auto nodeItr = flatCell->getNodes().begin(); nodeItr != flatCell->getNodes().end(); nodeItr++){
		nodeItr->second->setProp("value", false);
		nodeItr->second->setProp("prev_value", false);
	}
	queue<hcmInstance*> gateQ;
	for(auto instItr = flatCell->getInstances().begin(); instItr != flatCell->getInstances().end(); instItr++){
		//rest ff nor values.
		if(instItr->second->getName().find("_iwnh_ff") != instItr->second->getName().npos){
			for(auto portItr = instItr->second->getInstPorts().begin(); portItr != instItr->second->getInstPorts().end(); portItr++){
				if(portItr->second->getPort()->getDirection() == OUT){
					portItr->second->getNode()->setProp("value", true);
				}
			}
		}
		if(instItr->second->getName() == "dff"){
			instItr->second->setProp("ff_value", false);
			if(debugFlag){cout << "this should be printed only when using stdcell.v" << endl;}
		}
		instItr->second->setProp("inQueue", true);
		gateQ.push(instItr->second);
	}

	vcdFormatter vcd(cellName + ".vcd", flatCell, globalNodes);
	if(!vcd.good()){
		printf("-E- vcd no good balagan\n");
		exit(1);
	}
	map<const hcmNodeCtx, bool, cmpNodeCtx> valByNodeCtx;
	list<const hcmInstance*> noInsts;
	for(auto nodeItr = flatCell->getNodes().begin(); nodeItr != flatCell->getNodes().end(); nodeItr++){
		hcmNodeCtx nodeCtx(noInsts, (*nodeItr).second);
		valByNodeCtx[nodeCtx] = false;
	}
	int t = 1;
	hcmSigVec parser(signalTextFile, vectorTextFile, verbose);

	set<string> signals;
	parser.getSignals(signals);

	queue<pair<hcmNode*, bool>> eventQ;

	while (parser.readVector() == 0) {
		for(auto sigItr = signals.begin(); sigItr != signals.end(); sigItr++){
			string name = (*sigItr);
			bool val = false;
			parser.getSigValue(name, val);
			flatCell->getNode(name)->setProp("value", val);
			eventQ.push(make_pair(flatCell->getNode(name), val));
		}
		if(debugFlag){cout << "before simulate vector" << endl;}
		simulateVector(eventQ, gateQ);
		if(debugFlag){cout << "after simulate vector" << endl;}
		for(auto nodeItr = flatCell->getNodes().begin(); nodeItr != flatCell->getNodes().end(); nodeItr++){
			string name = nodeItr->second->getName();
			if(name == "VSS" || name == "VDD"){
				continue;
			}
			hcmNodeCtx temp(noInsts, (*nodeItr).second);
			bool currentVal;
			nodeItr->second->getProp("value", currentVal);
			valByNodeCtx[temp] = currentVal;
			vcd.changeValue(&temp, currentVal);
		}
		for(auto nodeItr = flatCell->getNodes().begin(); nodeItr != flatCell->getNodes().end(); nodeItr++){
			bool makePrevValue;
			nodeItr->second->getProp("value", makePrevValue);
			nodeItr->second->setProp("prev_value", makePrevValue);
		}
		vcd.changeTime(t++);
		if(debugFlag){ cout << t << endl;}
	}
}	
#include "helper.h"


//////////////////////////////////////////////////////////////////////////
// function name: addGateClause
// description:   create a clause for each gate, according to the 
//                gate Tseitin's transformation. add the clause to the solver.
// inputs: 	  gate, solver.
// outputs: 	  none.
//////////////////////////////////////////////////////////////////////////
void addGateClause(hcmInstance* gate, Solver& s){
        int outNodeNum;
        vector<int> inputNodesNum;
        vec<Lit> inputClauseVec;
        vec<Lit> totalClauseVec;
        string cellName = gate->masterCell()->getName();
        findInOut(gate, outNodeNum, inputNodesNum);

        if(cellName.find("nand") != cellName.npos){
                
                totalClauseVec.push(~mkLit(outNodeNum));
                for(unsigned i=0; i < inputNodesNum.size(); i++){
                        totalClauseVec.push(~mkLit(inputNodesNum[i]));
                        inputClauseVec.push(mkLit(inputNodesNum[i]));
                        inputClauseVec.push(mkLit(outNodeNum));
                        s.addClause(inputClauseVec);
                        inputClauseVec.clear();
                }
                s.addClause(totalClauseVec);
                totalClauseVec.clear();

        } else if (cellName.find("nor") != cellName.npos){
                totalClauseVec.push(mkLit(outNodeNum));
                for(unsigned i=0; i < inputNodesNum.size(); i++){
                        totalClauseVec.push(mkLit(inputNodesNum[i]));
                        inputClauseVec.push(~mkLit(inputNodesNum[i]));
                        inputClauseVec.push(~mkLit(outNodeNum));
                        s.addClause(inputClauseVec);
                        inputClauseVec.clear();
                }
                s.addClause(totalClauseVec);
                totalClauseVec.clear();

        } else if (cellName.find("xor") != cellName.npos){
                addXorClause(s, inputNodesNum, outNodeNum);

        } else if (cellName.find("buffer") != cellName.npos){
                addBuffClause(s, inputNodesNum, outNodeNum);

        } else if ((cellName.find("inv") != cellName.npos) || (cellName.find("not") != cellName.npos)){
                addInvClause(s, inputNodesNum, outNodeNum);

        } else if (cellName.find("or") != cellName.npos){
                totalClauseVec.push(~mkLit(outNodeNum));
                for(unsigned i=0; i < inputNodesNum.size(); i++){
                        totalClauseVec.push(mkLit(inputNodesNum[i]));
                        inputClauseVec.push(~mkLit(inputNodesNum[i]));
                        inputClauseVec.push(mkLit(outNodeNum));
                        s.addClause(inputClauseVec);
                        inputClauseVec.clear();
                }
                s.addClause(totalClauseVec);
                totalClauseVec.clear();

        } else if (cellName.find("and") != cellName.npos){
                totalClauseVec.push(mkLit(outNodeNum));
                for(unsigned i=0; i < inputNodesNum.size(); i++){
                        totalClauseVec.push(~mkLit(inputNodesNum[i]));
                        inputClauseVec.push(mkLit(inputNodesNum[i]));
                        inputClauseVec.push(~mkLit(outNodeNum));
                        s.addClause(inputClauseVec);
                        inputClauseVec.clear();
                }
                s.addClause(totalClauseVec);
                totalClauseVec.clear();
        }

        return;
}


//////////////////////////////////////////////////////////////////////////
// function name: findInOut
// description:   Identify the unique identifier of the gate inputs and output.
// inputs: 	  gate.
//                inputsNodesNum - a vector of input nodes ID's.
//                outNodeNum - an int with the output ID. assuming one output.
// outputs: 	  none.
//////////////////////////////////////////////////////////////////////////
void findInOut(hcmInstance* gate, int& outNodeNum, vector<int>& inputNodesNum) {
        int inputNode;
        map<string, hcmInstPort*>::iterator inputItr = gate->getInstPorts().begin();
        for(; inputItr != gate->getInstPorts().end(); inputItr++){
                if(inputItr->second->getPort()->getDirection() == IN){
                        inputItr->second->getNode()->getProp("num", inputNode);
                        inputNodesNum.push_back(inputNode);
                }
                if(inputItr->second->getPort()->getDirection() == OUT){
                        inputItr->second->getNode()->getProp("num", outNodeNum);
                }
        }
}


//////////////////////////////////////////////////////////////////////////
// function name: addXorClause
// description:   make a Xor clause and push to the solver.
// inputs: 	  s - solver instance.
//                inputsNodesNum - a vector of input nodes ID's.
//                outNodeNum - an int with the output ID. assuming one output.
// outputs: 	  none.
//////////////////////////////////////////////////////////////////////////
void addXorClause(Solver& s, vector<int>& inputNodesNum, int& outputNodeNum){
        vec<Lit> clauseVec;
        clauseVec.push(~mkLit(inputNodesNum[0]));
        clauseVec.push(~mkLit(inputNodesNum[1]));
        clauseVec.push(~mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();

        clauseVec.push(mkLit(inputNodesNum[0]));
        clauseVec.push(mkLit(inputNodesNum[1]));
        clauseVec.push(~mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();

        clauseVec.push(~mkLit(inputNodesNum[0]));
        clauseVec.push(mkLit(inputNodesNum[1]));
        clauseVec.push(mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();

        clauseVec.push(mkLit(inputNodesNum[0]));
        clauseVec.push(~mkLit(inputNodesNum[1]));
        clauseVec.push(mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();
}

//////////////////////////////////////////////////////////////////////////
// function name: addInvClause
// description:   adding an inverter clause to the Solver
// inputs: 	  s - solver instance.
//                inputsNodesNum - a vector of input nodes ID's.
//                outNodeNum - an int with the output ID. assuming one output.
// outputs: 	  none
//////////////////////////////////////////////////////////////////////////
void addInvClause(Solver& s, vector<int>& inputNodesNum, int& outputNodeNum){
        
        vec<Lit> clauseVec;

        clauseVec.push(~mkLit(inputNodesNum[0]));
        clauseVec.push(~mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();

        clauseVec.push(mkLit(inputNodesNum[0]));
        clauseVec.push(mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();
}

//////////////////////////////////////////////////////////////////////////
// function name: addBuffClause
// description:   adding a buffer clause to the Solver
// inputs: 	  s - solver instance.
//                inputsNodesNum - a vector of input nodes ID's.
//                outNodeNum - an int with the output ID. assuming one output.
// outputs: 	  none	  
//////////////////////////////////////////////////////////////////////////
void addBuffClause(Solver& s, vector<int>& inputNodesNum, int& outputNodeNum){
        
        vec<Lit> clauseVec;

        clauseVec.push(mkLit(inputNodesNum[0]));
        clauseVec.push(~mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();

        clauseVec.push(~mkLit(inputNodesNum[0]));
        clauseVec.push(mkLit(outputNodeNum));
        s.addClause(clauseVec);
        clauseVec.clear();
}

//////////////////////////////////////////////////////////////////////////
// function name: connectCircuitOutputs
// description:   connect all outputs of the 2 circuits through XOR gates and add all the clauses to Solver.
//                the first 2 outputs goes to a XOR gate, and then every output is XORed with the previus connection.	  
//////////////////////////////////////////////////////////////////////////
void connectCircuitOutputs(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell, vector<int>& finalOrInputs){
        int prevXorOutput;
	bool isFirst = true;
	hcmPort* impCellPort;
	hcmNode* node1;
	hcmNode* node2; 

        for(unsigned i=0; i<flatSpecCell->getPorts().size(); i++){
                
                hcmPort* outputPort = (flatSpecCell->getPorts())[i];
                if(outputPort->getDirection() == OUT){
                        impCellPort = flatImpCell->getPort(outputPort->getName()); // circuits with same output names.
                        node1 = impCellPort->owner();
                        node2 = outputPort->owner();
                        makeOutputXor(s, node1, node2, nodeNum);
                        // now nodeNum is the xor output ID.
                        finalOrInputs.push_back(nodeNum);
                        nodeNum++;
                }
        }
}

//////////////////////////////////////////////////////////////////////////
// function name: makeOutputXor
// description:   making a new XOR from 2 outputs of the compared cuircuits  
//////////////////////////////////////////////////////////////////////////
void makeOutputXor(Solver& s, hcmNode* node1, hcmNode* node2, int nodeNum){

        vector<int> outCompNodes;
        vector<int> inputToXor;

        int pushToVector;
        node1->getProp("num", pushToVector);
        outCompNodes.push_back(pushToVector);
        node2->getProp("num", pushToVector);
        outCompNodes.push_back(pushToVector);
        s.newVar();
        addXorClause(s, outCompNodes, nodeNum);

}

//////////////////////////////////////////////////////////////////////////
// function name: makeFFXor
// description:   connect every pair of FF from the 2 circuits to a XOR gate.
//////////////////////////////////////////////////////////////////////////
void makeFFXor(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell, vector<int>& finalOrInputs){

	hcmInstance* impCellffInst;
	hcmNode* node1;
	hcmNode* node2; 

        for(map<string, hcmInstance*>::iterator ffItr = flatSpecCell->getInstances().begin();
                                        ffItr != flatSpecCell->getInstances().end(); ffItr++){
                if(ffItr->second->masterCell()->getName() != "dff"){continue;}
                vector<int> outCompNodes;
                vector<int> inputToXor;
                map<string, hcmInstPort*>::iterator portItr = ffItr->second->getInstPorts().begin();
                for(; portItr != ffItr->second->getInstPorts().end(); portItr++){
                        if(portItr->second->getPort()->getName() == "D"){
                                node1 = portItr->second->getNode();
                        }
                }
                impCellffInst = flatImpCell->getInst(ffItr->first);
                for(portItr = impCellffInst->getInstPorts().begin(); portItr != impCellffInst->getInstPorts().end(); portItr++){
                        if(portItr->second->getPort()->getName() == "D"){
                                node2 = portItr->second->getNode();
                        }
                }

                makeOutputXor(s, node1, node2, nodeNum);
                finalOrInputs.push_back(nodeNum);
                nodeNum++;
        }
}

//////////////////////////////////////////////////////////////////////////
// function name: setGlobalNodes
// description:   set a unique number to the global nodes from the 2 circuits.	  
//////////////////////////////////////////////////////////////////////////
void setGlobalNodes(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell){
        
        vec<Lit> clauseVec;
        flatSpecCell->getNode("VDD")->setProp("num", nodeNum);
	flatImpCell->getNode("VDD")->setProp("num", nodeNum);
        s.newVar();
        clauseVec.push(mkLit(nodeNum));
	s.addClause(clauseVec);
	clauseVec.clear();
	nodeNum++;
	flatSpecCell->getNode("VSS")->setProp("num", nodeNum);
	flatImpCell->getNode("VSS")->setProp("num", nodeNum);
	clauseVec.push(~mkLit(nodeNum));
        s.newVar();
	s.addClause(clauseVec);
	clauseVec.clear();
	nodeNum++;
}

//////////////////////////////////////////////////////////////////////////
// function name: setSpecCellNodes
// description:   set a unique number to each node in the spec circuit 	  
//////////////////////////////////////////////////////////////////////////
void setSpecCellNodes(Solver& s, int& nodeNum, hcmCell* flatSpecCell, map<string,int>& inputs){
        map<string, hcmNode*>::iterator nodeItr = flatSpecCell->getNodes().begin();
	for(; nodeItr != flatSpecCell->getNodes().end(); nodeItr++){
		string nodeName = nodeItr->second->getName();
		if(nodeName == "VSS" || nodeName == "VDD" || nodeName == "CLK") {continue;}
		nodeItr->second->setProp("num", nodeNum);
                addCommonNodes(nodeItr->second, inputs, nodeNum);
		s.newVar();
		nodeNum++;
	}
}

//////////////////////////////////////////////////////////////////////////
// function name: addCommonNodes
// description:   add all inputs and FF output to the common nodes map	  
//////////////////////////////////////////////////////////////////////////
void addCommonNodes(hcmNode* node, map<string,int>& inputs, int nodeNum){
        if(node->getPort() != NULL) {
                if(node->getPort()->getDirection() == IN){
                        inputs[node->getPort()->getName()] = nodeNum;
                }
        }
        map<string, hcmInstPort*>::iterator portItr = node->getInstPorts().begin();
        for(; portItr != node->getInstPorts().end(); portItr++){
                if(portItr->second->getInst()->masterCell()->getName() == "dff"){
                        if(portItr->second->getPort()->getDirection() == OUT){
                                inputs[portItr->second->getInst()->getName()] = nodeNum;
                        }
                }
        }
}

//////////////////////////////////////////////////////////////////////////
// function name: setImpCellNodes
// description:   setting a unique number to all nodes in Imp circuit. giving common nodes the same number 	  
//////////////////////////////////////////////////////////////////////////
void setImpCellNodes(Solver& s, int& nodeNum, hcmCell* flatImpCell, map<string,int>& inputs){
        map<string, hcmNode*>::iterator nodeItr = flatImpCell->getNodes().begin();
        for(; nodeItr != flatImpCell->getNodes().end(); nodeItr++){
                string nodeName = nodeItr->second->getName();
		if(nodeName == "VSS" || nodeName == "VDD" || nodeName == "CLK") {continue;}

		if(findCommonNodes(nodeItr->second, inputs)) {continue;}
		nodeItr->second->setProp("num", nodeNum);
		s.newVar();
		nodeNum++;
	}
}

//////////////////////////////////////////////////////////////////////////
// function name: findCommonNodes
// description:   find a node in the common nodes Map. return true if found.	  
//////////////////////////////////////////////////////////////////////////
bool findCommonNodes(hcmNode* node, map<string,int>& inputs){
        if(node->getPort() != NULL){
                if(node->getPort()->getDirection() == IN){
                        string inputName = node->getPort()->getName();
                        if(inputs.find(inputName) != inputs.end()){
                                node->setProp("num", inputs[inputName]);
                                return true;
                        }
                }
        }
        map<string, hcmInstPort*>::iterator portItr = node->getInstPorts().begin();
        for(; portItr != node->getInstPorts().end(); portItr++){
                if(portItr->second->getInst()->masterCell()->getName() == "dff"){
                        if(portItr->second->getPort()->getDirection() == OUT){
                                string instName = portItr->second->getInst()->getName();
                                node->setProp("num", inputs[instName]);
                                return true;
                        }
                }
        }

        return false;
}

//////////////////////////////////////////////////////////////////////////
// function name: isOutputDiff
// description:   check if there are different outputs between the circuits. including FF	  
//////////////////////////////////////////////////////////////////////////
bool isOutputDiff(hcmCell* flatSpecCell, hcmCell* flatImpCell){
        
        set<string> specOutputs;
        set<string> impOutputs;
        getOutputMap(flatSpecCell, specOutputs);
        getOutputMap(flatImpCell, impOutputs);
        
        if(specOutputs.size() != impOutputs.size()){
                return true;
        }
        set<string>::iterator outItr = specOutputs.begin();
        for(; outItr != specOutputs.end(); outItr++){
                if(impOutputs.find(*outItr) == impOutputs.end()){
                        return true;
                }
        }

        return false;
}

//////////////////////////////////////////////////////////////////////////
// function name: getOutputMap
// description:   generate a Map of outputs for a given FlatCell. including FF	  
//////////////////////////////////////////////////////////////////////////
void getOutputMap(hcmCell* flatCell, set<string>& outputs){
        map<string, hcmInstance*>::iterator instItr = flatCell->getInstances().begin();
        for(; instItr != flatCell->getInstances().end(); instItr++){
                if(instItr->second->masterCell()->getName() == "dff"){
                        outputs.insert(instItr->second->getName());
                }
        }
        
        for(int i = 0; i < flatCell->getPorts().size(); i++){
                if((flatCell->getPorts())[i]->getDirection() == OUT){
                        outputs.insert((flatCell->getPorts())[i]->getName());
                }
        }
}

void makeFinalOrClause(Solver& s, int nodeNum, vector<int>& finalOrInputs){

        vec<Lit> totalClauseVec;
        vec<Lit> inputClauseVec; 
        s.newVar();

        totalClauseVec.push(~mkLit(nodeNum));
        for(unsigned i=0; i < finalOrInputs.size(); i++){
                totalClauseVec.push(mkLit(finalOrInputs[i]));
                inputClauseVec.push(~mkLit(finalOrInputs[i]));
                inputClauseVec.push(mkLit(nodeNum));
                s.addClause(inputClauseVec);
                inputClauseVec.clear();
        }
        s.addClause(totalClauseVec);
        totalClauseVec.clear();
}
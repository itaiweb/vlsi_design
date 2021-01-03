#include "helper.h"

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

void connectCircuitOutputs(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell){
        int prevXorOutput;
	bool isFirst = true;
	hcmPort* impCellPort;
	hcmNode* node1;
	hcmNode* node2; 

        for(unsigned i=0; i<flatSpecCell->getPorts().size(); i++){
                
                hcmPort* outputPort = (flatSpecCell->getPorts())[i];
                if(outputPort->getDirection() == OUT){
                        impCellPort = flatImpCell->getPort(outputPort->getName()); // TODO: make sure we only get circuits with same output names.
                        node1 = impCellPort->owner();
                        node2 = outputPort->owner();
                        makeOutputXor(s, node1, node2, nodeNum, isFirst, prevXorOutput);
                }
        }
}

void makeOutputXor(Solver& s, hcmNode* node1, hcmNode* node2, int& nodeNum, bool& isFirst, int& prevXorOutput){

        vector<int> outCompNodes;
        vector<int> inputToXor;

        int pushToVector;
        node1->getProp("num", pushToVector);
        outCompNodes.push_back(pushToVector);
        node2->getProp("num", pushToVector);
        outCompNodes.push_back(pushToVector);
        s.newVar();
        debugCounter++;
        addXorClause(s, outCompNodes, nodeNum);
        if(isFirst){
                prevXorOutput = nodeNum;
                nodeNum++;
                isFirst = false;
                return;
        }
        inputToXor.push_back(prevXorOutput);
        inputToXor.push_back(nodeNum);
        nodeNum++;
        s.newVar();
        debugCounter++;
        addXorClause(s, inputToXor, nodeNum);
        prevXorOutput = nodeNum;
        nodeNum++;
}

void makeFFXor(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell){
        int prevXorOutput = nodeNum - 1;
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

                bool isFirst = false;
                makeOutputXor(s, node1, node2, nodeNum, isFirst, prevXorOutput);
        }
}

void setGlobalNodes(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell){
        
        vec<Lit> clauseVec;
        flatSpecCell->getNode("VDD")->setProp("num", nodeNum);
	flatImpCell->getNode("VDD")->setProp("num", nodeNum);
        s.newVar();
        clauseVec.push(mkLit(nodeNum));
        debugCounter++;
	s.addClause(clauseVec);
	clauseVec.clear();
	nodeNum++;
	flatSpecCell->getNode("VSS")->setProp("num", nodeNum);
	flatImpCell->getNode("VSS")->setProp("num", nodeNum);
	clauseVec.push(~mkLit(nodeNum));
        s.newVar();
        debugCounter++;
	s.addClause(clauseVec);
	clauseVec.clear();
	nodeNum++;
}

void setSpecCellNodes(Solver& s, int& nodeNum, hcmCell* flatSpecCell, map<string,int>& inputs){
        map<string, hcmNode*>::iterator nodeItr = flatSpecCell->getNodes().begin();
	for(; nodeItr != flatSpecCell->getNodes().end(); nodeItr++){
		string nodeName = nodeItr->second->getName();
		if(nodeName == "VSS" || nodeName == "VDD" || nodeName == "CLK") {continue;}
		nodeItr->second->setProp("num", nodeNum);
                addCommonNodes(nodeItr->second, inputs, nodeNum);
		s.newVar();
                debugCounter++;
		cout << "node " << nodeNum << " is " << nodeItr->first << endl;
		nodeNum++;
	}
}

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

void setImpCellNodes(Solver& s, int& nodeNum, hcmCell* flatImpCell, map<string,int>& inputs){
        map<string, hcmNode*>::iterator nodeItr = flatImpCell->getNodes().begin();
        for(; nodeItr != flatImpCell->getNodes().end(); nodeItr++){
                string nodeName = nodeItr->second->getName();
		if(nodeName == "VSS" || nodeName == "VDD" || nodeName == "CLK") {continue;}

		if(findCommonNodes(nodeItr->second, inputs)) {continue;}
		nodeItr->second->setProp("num", nodeNum);
		s.newVar();
                debugCounter++;
		cout << "node " << nodeNum << " is " << nodeItr->first << endl;
		nodeNum++;
	}
}

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
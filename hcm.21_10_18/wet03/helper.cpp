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
                cout << "outnode num: " << outNodeNum << endl;
                cout << "innode num: " << inputNodesNum[0] << " " << inputNodesNum[1] << endl;
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
        for(map<string, hcmInstPort*>::iterator inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
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

void makeOutputXor(Solver& s, int& nodeNum, hcmCell* flatSpecCell, hcmCell* flatImpCell){
        int prevXorOutput;
	bool isFirst = true;
	hcmPort* impCellPort;
	hcmNode* node1;
	hcmNode* node2; 

        for(unsigned i=0; i<flatSpecCell->getPorts().size(); i++){
                vector<int> outCompNodes;
                vector<int> inputToXor;
                hcmPort* outputPort = (flatSpecCell->getPorts())[i];

                if(outputPort->getDirection() == OUT){
                        impCellPort = flatImpCell->getPort(outputPort->getName());
                        node1 = impCellPort->owner();
                        node2 = outputPort->owner();
                        int pushToVector;
                        node1->getProp("num", pushToVector);
                        outCompNodes.push_back(pushToVector);
                        node2->getProp("num", pushToVector);
                        outCompNodes.push_back(pushToVector);
                        s.newVar();
                        addXorClause(s, outCompNodes, nodeNum);
                        if(isFirst){
                                prevXorOutput = nodeNum;
                                nodeNum++;
                                isFirst = false;
                                continue;
                        }
                        inputToXor.push_back(prevXorOutput);
                        inputToXor.push_back(nodeNum);
                        nodeNum++;
                        s.newVar();
                        addXorClause(s, inputToXor, nodeNum);
                        prevXorOutput = nodeNum;
                        nodeNum++;
                }
        }
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
                for(map<string, hcmInstPort*>::iterator portItr = ffItr->second->getInstPorts().begin(); 
                                                portItr != ffItr->second->getInstPorts().end(); portItr++){
                        if(portItr->second->getPort()->getName() == "D"){
                                node1 = portItr->second->getNode();
                        }
                }
                cout << "Spec cell ff is: " << ffItr->first << endl;
                impCellffInst = flatImpCell->getInst(ffItr->first);
                cout << "Imp cell ff is:  " << impCellffInst->getName() << endl;
                for(map<string, hcmInstPort*>::iterator portItr = impCellffInst->getInstPorts().begin(); 
                                                portItr != impCellffInst->getInstPorts().end(); portItr++){
                        if(portItr->second->getPort()->getName() == "D"){
                                node2 = portItr->second->getNode();
                        }
                }

                int pushToVector;
                node1->getProp("num", pushToVector);
                outCompNodes.push_back(pushToVector);
                node2->getProp("num", pushToVector);
                outCompNodes.push_back(pushToVector);
                s.newVar();
                addXorClause(s, outCompNodes, nodeNum);
                
                inputToXor.push_back(prevXorOutput);
                inputToXor.push_back(nodeNum);
                nodeNum++;
                s.newVar();
                addXorClause(s, inputToXor, nodeNum);
                prevXorOutput = nodeNum;
                nodeNum++;
        }
}
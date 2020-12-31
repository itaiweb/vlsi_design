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

        } else if (cellName.find("dff") != cellName.npos){
                hcmNode* CLK; hcmNode* D;
                bool clkRes, dRes, ffVal;
                gate->getProp("ff_value", ffVal);
                for(map<string, hcmInstPort*>::iterator portItr = gate->getInstPorts().begin(); portItr != gate->getInstPorts().end(); portItr++){
                        if(portItr->second->getPort()->getName() == "CLK"){
                                CLK = portItr->second->getNode();
                        }
                        if(portItr->second->getPort()->getName() == "D"){
                                D = portItr->second->getNode();
                        }
                }
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
                        inputItr->second->getProp("num", outNodeNum);
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
#include "helper.h"

bool makeClause(hcmInstance* gate, vec<Lit>& clauseVec){
        bool result;
        string cellName = gate->masterCell()->getName();
        if(cellName.find("nand") != cellName.npos){
                result = true;
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = result && nodeVal;
                        }
                }
                result = !result;
        } else if (cellName.find("nor") != cellName.npos){
                result = false;
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = result || nodeVal;
                        }
                }
                result = !result;
        } else if (cellName.find("xor") != cellName.npos){
                result = false;
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = result ^ nodeVal;
                        }
                }
        } else if (cellName.find("buffer") != cellName.npos){
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = nodeVal;
                        }
                }
        } else if ((cellName.find("inv") != cellName.npos) || (cellName.find("not") != cellName.npos)){
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = !nodeVal;
                        }
                }
        } else if (cellName.find("or") != cellName.npos){
                result = false;
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = result || nodeVal;
                        }
                }
        } else if (cellName.find("and") != cellName.npos){
                result = true;
                for(auto inputItr = gate->getInstPorts().begin(); inputItr != gate->getInstPorts().end(); inputItr++){
                        if(inputItr->second->getPort()->getDirection() == IN){
                                bool nodeVal;
                                inputItr->second->getNode()->getProp("value", nodeVal);
                                result = result && nodeVal;
                        }
                }
        } else if (cellName.find("dff") != cellName.npos){
                hcmNode* CLK; hcmNode* D;
                bool clkRes, dRes, ffVal;
                gate->getProp("ff_value", ffVal);
                for(auto portItr = gate->getInstPorts().begin(); portItr != gate->getInstPorts().end(); portItr++){
                        if(portItr->second->getPort()->getName() == "CLK"){
                                CLK = portItr->second->getNode();
                        }
                        if(portItr->second->getPort()->getName() == "D"){
                                D = portItr->second->getNode();
                        }
                }
                CLK->getProp("value", clkRes);
                if(clkRes == false){
                        result = ffVal;
                } else {
                        D->getProp("prev_value", dRes);
                        result = dRes;
                        gate->setProp("ff_value", dRes);
                }
        }

        return result;
}
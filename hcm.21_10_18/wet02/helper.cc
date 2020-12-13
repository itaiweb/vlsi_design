#include "helper.h"

void simulateVector(queue<pair<hcmNode*, bool>>& eventQ){
        queue<hcmInstance*> gateQ;
        while(!eventQ.empty()){
                eventProcessor(eventQ, gateQ);
                if(!gateQ.empty()){
                        gateProcessor(eventQ, gateQ);
                }
        }
}

void eventProcessor(queue<pair<hcmNode*, bool>>& eventQ, queue<hcmInstance*>& gateQ){
        while(!eventQ.empty()){
                pair<hcmNode*, bool> event = eventQ.front();
                eventQ.pop();
                event.first->setProp("value", event.second);
                hcmNode* node = event.first;
                bool inQueue;
		for(auto innerInstPort = node->getInstPorts().begin(); innerInstPort != node->getInstPorts().end(); innerInstPort++){
			if(innerInstPort->second->getPort()->getDirection() == IN){
                                innerInstPort->second->getInst()->getProp("inQueue", inQueue);
                                if(!inQueue){
				        gateQ.push(innerInstPort->second->getInst());
                                        innerInstPort->second->getInst()->setProp("inQueue", true);
                                }
			}
		}
        }
}

void gateProcessor(queue<pair<hcmNode*, bool>>& eventQ, queue<hcmInstance*>& gateQ){
        while(!gateQ.empty()){
                hcmInstance* gate = gateQ.front();
                gateQ.pop();
                gate->setProp("inQueue", false);
                for(auto instPortItr = gate->getInstPorts().begin(); instPortItr != gate->getInstPorts().end(); instPortItr++){
                        if(instPortItr->second->getPort()->getDirection() == OUT){
                                bool prevVal;
                                instPortItr->second->getNode()->getProp("value", prevVal);
                                bool newVal = simulateGate(gate);
                                if(newVal != prevVal){
                                        eventQ.push(make_pair(instPortItr->second->getNode(), newVal));
                                }
                        }
                }
        }
}

bool simulateGate(hcmInstance* gate){
        bool result;
        string cellName = gate->masterCell()->getName();
        if(cellName.find("nand") != cellName.npos){

        } else if (cellName.find("nor") != cellName.npos){
                
        } else if (cellName.find("xor") != cellName.npos){
                
        } else if (cellName.find("buffer") != cellName.npos){
                
        } else if ((cellName.find("inv") != cellName.npos) || (cellName.find("not") != cellName.npos)){
                
        } else if (cellName.find("or") != cellName.npos){
                
        } else if (cellName.find("and") != cellName.npos){

        } else if (cellName.find("dff") != cellName.npos){
                
        } else {
                printf("bug in simulate gate madafaka\n");
        }
        

        return false;
}
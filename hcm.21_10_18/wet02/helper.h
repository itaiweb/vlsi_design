#ifndef _HELPER_H
#define _HELPER_H

#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <queue>
#include "hcm.h"
#include "flat.h"
#include "hcmvcd.h"
#include "hcmsigvec.h"

void simulateVector(queue<pair<hcmNode*, bool>>&);
void eventProcessor(queue<pair<hcmNode*, bool>>&, queue<hcmInstance*>&);
void gateProcessor(queue<pair<hcmNode*, bool>>&, queue<hcmInstance*>&);
bool simulateGate(hcmInstance*);


#endif _HELPER_H
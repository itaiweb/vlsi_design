#ifndef _HELPER_H
#define _HELPER_H

#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "hcm.h"
#include "flat.h"
#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "utils/Options.h"
#include "core/Dimacs.h"
#include "core/Solver.h"

#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS

using namespace Minisat;

void addGateClause(hcmInstance*, Solver&);
void findInOut(hcmInstance*, int&, vector<int>&);
void addXorClause(Solver&, vector<int>&, int&);
void addInvClause(Solver&, vector<int>&, int&);
void addBuffClause(Solver&, vector<int>&, int&);
void connectCircuitOutputs(Solver&, int&, hcmCell*, hcmCell*);
void makeOutputXor(Solver&, hcmNode*, hcmNode*, int&, bool&, int&);
void makeFFXor(Solver&, int&, hcmCell*, hcmCell*);
void setGlobalNodes(Solver&, int&, hcmCell*, hcmCell*);
void setSpecCellNodes(Solver&, int&, hcmCell*, map<string,int>&);
void setImpCellNodes(Solver&, int&, hcmCell*, map<string,int>&);
void addCommonNodes(hcmNode*, map<string,int>&, int);
bool findCommonNodes(hcmNode*, map<string,int>&);

#endif
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
#define KBLU  "\x1B[34m"
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
using namespace Minisat;

bool isOutputDiff(hcmCell*, hcmCell*);
void getOutputMap(hcmCell*, set<string>&);
void addGateClause(hcmInstance*, Solver&);
void findInOut(hcmInstance*, int&, vector<int>&);
void addXorClause(Solver&, vector<int>&, int&);
void addInvClause(Solver&, vector<int>&, int&);
void addBuffClause(Solver&, vector<int>&, int&);
void connectCircuitOutputs(Solver&, int&, hcmCell*, hcmCell*, vector<int>&);
void makeOutputXor(Solver&, hcmNode*, hcmNode*, int);
void connectFFInputs(Solver&, int&, hcmCell*, hcmCell*, vector<int>&);
void setGlobalNodes(Solver&, int&, hcmCell*, hcmCell*);
void setSpecCellNodes(Solver&, int&, hcmCell*, map<string,int>&);
void setImpCellNodes(Solver&, int&, hcmCell*, map<string,int>&);
void addCommonNodes(hcmNode*, map<string,int>&, int);
bool findCommonNodes(hcmNode*, map<string,int>&);
void makeFinalOrClause(Solver&, int, vector<int>&);
void printInputs(hcmCell*, hcmCell*, Solver&, map<string,int>&);

#endif
#ifndef _HELPER_H
#define _HELPER_H

#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
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

bool makeClause(hcmInstance*, vec<Lit>&);

#endif
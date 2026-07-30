#pragma once
//#include <GxEPD2_BW.h>
#include <GxEPD2_4C.h>

//using DisplayType = GxEPD2_BW<GxEPD2_579_GDEY0579T93, GxEPD2_579_GDEY0579T93::HEIGHT>;
using DisplayType = GxEPD2_4C<GxEPD2_579c_GDEY0579F51, GxEPD2_579c_GDEY0579F51::HEIGHT>;
extern DisplayType display;
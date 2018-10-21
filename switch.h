#ifndef switch_h
#define switch_h
#include "variables.h"
// extern vector<flowEntry> flowtable;

void detectSwitch(char **argv, SWI *swi);
void startSwitchLoop(SWI *swi, vector<flowEntry> &flowtable);


#endif
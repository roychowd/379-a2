#ifndef switch_h
#define switch_h
#include "variables.h"


void detectSwitch(char **argv, SWI *swi);
void initializeCurrentFlowEntry(SWI *swi, vector<flowEntry> &flowtable);
void readFILE(string filename);

#endif
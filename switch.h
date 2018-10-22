#ifndef switch_h
#define switch_h
#include "variables.h"

void detectSwitch(char **argv, SWI *swi);
void initializeCurrentFlowEntry(SWI *swi, vector<flowEntry> &flowtable);
void startFIFOSwitchToController(SWI *swi);
void readFILE(string filename);
void startFifoSwitchToSwitch(SWI *swi);

#endif
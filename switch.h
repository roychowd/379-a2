#ifndef switch_h
#define switch_h
#include "variables.h"

void detectSwitch(char **argv, SWI *swi);
void initializeCurrentFlowEntry(SWI *swi, vector<flowEntry> &flowtable);
void startFIFOSwitchToController(SWI *swi);
void readFILE(string filename, SWI *swi, packetStats *stats, vector<flowEntry> flowtable);
void startFifoSwitchToSwitch(SWI *swi);
void sendMessageToController();

#endif
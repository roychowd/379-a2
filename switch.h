#ifndef switch_h
#define switch_h
#include "variables.h"

typedef struct
{
    int fdSend;
    int fdRecieve;
    string fifoFromSwi;
    string fifoToSwi;
} switchFifos;

void detectSwitch(char **argv, SWI *swi);
void initializeCurrentFlowEntry(SWI *swi, vector<flowEntry> &flowtable);
void startFIFOSwitchToController(SWI *swi);
void readFILE(string filename, SWI *swi, packetStats *stats, vector<flowEntry> flowtable);
void startFifoSwitchToSwitch(SWI *swi);
// void sendMessageToController();
void switchLoop(SWI *swi);

#endif
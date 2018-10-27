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

typedef struct
{
    int ADMIT;
    int ACK;
    int ADDRULE;
    int RELAYIN;
} RECEIVEDswi;

typedef struct
{
    int OPEN;
    int QUERY;
    int RELAYOUT;
} TRANSMITTEDswi;

typedef struct
{
    RECEIVEDswi recievedPacket;
    TRANSMITTEDswi transmittedPacket;
} PACKETSWI;

// extern PACKETSWI switchPacketCounts;

void detectSwitch(char **argv, SWI *swi);
void createNewFlowEntry(vector<flowEntry> &flowtable, int destIPLow, int destIPHigh);
void switchLoop(SWI *swi, vector<flowEntry> &flowtable);

#endif
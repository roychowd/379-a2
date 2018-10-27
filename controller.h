#ifndef controller_h
#define controller_h
#include "variables.h"



typedef struct {
    int OPEN;
    int QUERY;
} RECEIVEDcont;

typedef struct {
    int ACK;
    int ADD;
} TRANSMITTEDcont;

typedef struct {
    RECEIVEDcont receivedPackets;
    TRANSMITTEDcont transmittedPackets;
} PACKETCONT;

// static int maxFDS = 1;

void detectController(char **argv, Controller *controller);
void startFIFOControllerToSwitch(SWI *swi);
void ControllerLoop(int nswitch);
#endif
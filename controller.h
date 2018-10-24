#ifndef controller_h
#define controller_h
#include "variables.h"



static int maxFDS = 1;

void detectController(char **argv, Controller *controller);
void startFIFOControllerToSwitch(SWI *swi);
void ControllerLoop(int nswitch);
#endif
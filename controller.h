#ifndef controller_h
#define controller_h
#include "variables.h"

void detectController(char **argv, Controller *controller);
void startFIFOControllerToSwitch(SWI *swi);
#endif
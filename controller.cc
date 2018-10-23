#include "controller.h"

static void err_sys(const char *x)
{
	perror(x);
	exit(1);
}

void detectController(char **argv, Controller *controller)
{
	cout << "detect control called " << endl;
	if (strcmp(argv[1], "cont") == 0)
	{
		if (!(controller->nSwitch = atoi(argv[2])))
		{
			err_sys("nswitch not a valid integer");
		}
		else if ((controller->nSwitch > MAX_NSW) || (controller->nSwitch < 0))
		{
			err_sys("nswitch larger than MAX_NSW (7) or less than 1. Please make sure nSwitch is: 1 <= nSwitch <= 7!");
		}
	}
	else
	{
		err_sys("invalid keyword: No 'cont'. Please run with ./a2sdn cont nswitch");
	}
}

void startFIFOControllerToSwitch(SWI *swi)
{
	string fifoname = "fifo-0-" + swi->swi;
	
}
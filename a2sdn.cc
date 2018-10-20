#define _BSD_SOURCE
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define MAX_NSW 7

typedef struct
{
	int nSwitch;
} Controller;

typedef struct
{
	string filename;
	int position;
	string IP_ADDR;
	int IP_LOW;
	int IP_HIGH;
	string swj;
	string swk;
} SWI;

void err_sys(const char *x);
void detectController(char **argv, Controller *controller);
void detectSwitch(char **argv, SWI *swi);
void grabIP(SWI **swi, char *arg);

int main(int argc, char **argv)
{
	if (argc != 3 && argc != 6)
	{
		err_sys("Not Enough Arguments Given");
	}
	if (argc == 3)
	{
		Controller controller;
		detectController(argv, &controller);
		// CONTROLLER LOOP //
	}
	if (argc == 6)
	{
		SWI swi;
		detectSwitch(argv, &swi);
		// switch loop //
	}
	return 1;
}

void err_sys(const char *x)
{
	perror(x);
	exit(1);
}

void detectController(char **argv, Controller *controller)
{
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

void detectSwitch(char **argv, SWI *swi)
{
	// given: as2dn swi trafficfile [null|swj] [null|swk] IPlow-IPhigh //

	if (strstr(argv[1], "sw"))
	{

		swi->IP_ADDR = string(argv[5]);
		// implement grabIP ranges later
		grabIP(&swi, argv[5]);
		swi->filename = string(argv[2]);
		if (strcmp(argv[3], "null") == 0)
		{
			swi->swj = "";
		}
		else
		{
			swi->swj = string(argv[3]);
		}
		if (strcmp(argv[4], "null") == 0)
		{
			swi->swk = "";
		}
		else
		{
			swi->swk = string(argv[4]);
		}
	}
	return;
}

void grabIP(SWI **swi, char *arg)
{

	int index = 0;
	char *token = strdup(arg);
	token = strtok(token, "-\n");
	while (token != NULL)
	{
		cout << token << endl;
		if (index == 0)
		{
			(*swi)->IP_LOW = atoi(token);
			index++;
		}
		else if (index == 1)
		{
			(*swi)->IP_HIGH = atoi(token);
		}
		token = strtok(NULL, "-\n");
	}
	return;
}
#include "switch.h"

static void err_sys(const char *x)
{
    perror(x);
    exit(1);
}

static void grabIP(SWI **swi, char *arg)
{
    int index = 0;
    char *token = strdup(arg);
    token = strtok(token, "-\n");
    while (token != NULL)
    {
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

static void grabPositions(char *argument, char *argument2, char *argument3, SWI **swi)
{
    char *position = &argument[strlen(argument) - 1];
    (*swi)->position = atoi(position);
    (*swi)->swi = string(position);

    if (strcmp(argument2, "null") == 0)
    {
        (*swi)->positionLeft = 0;
        (*swi)->swj = "";
    }
    else
    {
        char *positionLeft = &argument2[strlen(argument2) - 1];
        (*swi)->positionLeft = atoi(positionLeft);
        (*swi)->swj = string((positionLeft));
    }
    if (strcmp(argument3, "null") == 0)
    {
        (*swi)->positionRight = 0;
        (*swi)->swk = "";
    }
    else
    {
        char *positionRight = &argument3[strlen(argument3) - 1];
        (*swi)->positionRight = atoi(positionRight);
        (*swi)->swk = string(positionRight);
    }
    return;
}

void detectSwitch(char **argv, SWI *swi)
{
    // given: as2dn swi trafficfile [null|swj] [null|swk] IPlow-IPhigh //
    if (strstr(argv[1], "sw"))
    {
        // swi->swi = string(argv[1]);
        swi->IP_ADDR = string(argv[5]);
        grabIP(&swi, argv[5]);
        swi->filename = string(argv[2]);
        grabPositions(argv[1], argv[3], argv[4], &swi);
    }

    return;
}

void initializeCurrentFlowEntry(SWI *swi, vector<flowEntry> &flowtable)
{
    // create an entry in the flow table
    flowtable.push_back(flowEntry());
    // set up flowtable entry
    flowtable[0].srcIP = "0-1000";
    flowtable[0].destIP = swi->IP_ADDR;
    flowtable[0].srcIP_lo = 0;
    flowtable[0].srcIP_lo = 1000;
    flowtable[0].destIP_lo = swi->IP_LOW;
    flowtable[0].destIP_hi = swi->IP_HIGH;
    flowtable[0].actionType = "FORWARD";
    flowtable[0].pri = 4;
    flowtable[0].actionVal = 3;
    flowtable[0].pktcount = 0;
    flowtable[0].stats = {0};

    // now that the flowtable is set up we need to use io multiplexing and

    // read and process trafficfile
    // read the file and ignore # and empty lines and ones that dont have the swi
}

void startFIFOSwitchToController(SWI *swi)
{
    string fifoname = "fifo-" + swi->swi + "-0";
    cout << fifoname;
    // mkfifo and convert string to char *
}

void startFifoSwitchToSwitch(SWI *swi)
{
    string fifonameLeft = "fifo-" + swi->swi + "-" + swi->swj;
}

void readFILE(string filename)
{
    cout << filename;
}
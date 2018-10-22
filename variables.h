#ifndef variables_h
#define variables_h

#define _BSD_SOURCE
#include <iostream>
#include <string>
#include <vector>

// C libraries that i need
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
#define MAX_NSW 7

typedef struct
{
    int ADMIT;
    int ACK;
    int ADDRULE;
    int RELAYIN;
    int OPEN;
    int QUERY;
    int RELAYOUT;
} packetStats;

typedef struct
{
    string srcIP;
    string destIP;
    string actionType;
    int actionVal;
    int srcIP_lo;
    int srcIP_hi;
    int destIP_lo;
    int destIP_hi;
    int pri;
    int pktcount;
    packetStats stats;
} flowEntry;

typedef struct
{
    int nSwitch;
} Controller;

typedef struct
{
    string filename;
    int position;
    int positionLeft;
    int positionRight;
    string IP_ADDR;
    int IP_LOW;
    int IP_HIGH;
    string swi;
    string swj;
    string swk;
} SWI;

#endif
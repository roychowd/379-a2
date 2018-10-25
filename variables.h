#ifndef variables_h
#define variables_h

#define _BSD_SOURCE
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>

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
    int fileDescriptorSwiToCont;
    int fileDescriptorContToSwi;
    string fifoNameSwiToCont;
    string fifoNameContToSwi;
} fifoStruct;

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

// Inspiration taken from http://webdocs.cs.ualberta.ca/~c379/F18/379only/lab-messages.html //
typedef enum
{
    OPEN,
    ACK,
    QUERY,
    ADD,
    RELAY,
    NONE
} KIND; // Message kinds

typedef struct
{
    string msg; // port3
    string swi;
    string port1; //swj
    string port2; // swk
    KIND kind;
} Packet;

#endif
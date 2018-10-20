// REVIEW CONTROLLER LOOP
// 		EACH ITERATION OF THE MAIN LOOP OF THE CONTROLLER DOES THE FOLLOWING:
// 				1. POLL KEYBOARD FOR USER COMMAND 
// 						LIST -> THE PROGRAM WRITES THE SOTRED INFORMATION ABOUT THE ATTACHED SWITCHES THAT HAVE OPENED A CONNECTION WITH THE CONTROLLER
//							AS WELL, FOR EACH TRANSMITTED AND RECIVED PACKET, THE PROGRAM WRITES AN AGGREGAGTE COUNT OF HANDELD PACKETS FOR THIS TYPE
// 				 		EXIT -> PROGRAM CALLS LIST AND EXITS
// 				2. POLL THE INCOMING FIFOS FROM THE ATTACHED SWITCHES
// 					THE CONTROLLER HANDLES EACH INCOMING PACKET AS DESCRIBED IN PACKET TYPES
// 			NOTE THAT WHEN A PACKET IS FORWARDED TO THE CONTROLLER BY A SWITCH THE LIST COMMAND IS INVOKED
// 
// REVIEW SWITCH LOOP
// THE PROGRAM INSTALLS AN INITIAL RULE IN ITS FLOW TABLE:  ( refer to assignment document ) (recall [IPLOW-IPHIGH] is from command line (this is the destIP))
// 		EACH ITERATION OF THE MAIN LOOP PERFORMS THESE STEPS:
// 				1. READ AND PROCESS SIGNLE LINE FROM THE TRAFFIC FILE 
// 				 		PROGRAM CONTINUES TO MONITOR AND PROCESS KEYBOARD COMMANDS 
//						A PACKET IS ADMITTED IF THE LINE SPECIFIES THE CURRENT SWITCH
// 
// 				2. POLL THE KEYBOARD FOR USER COMMANDS (LIST AND EXIT)
// 					LIST -> PROGRAM WRIES ALL ENTIRES IN THE FLOW TBALE AND FOR EACH TRASNMITTED OR RECIEVED PACKET TYPE THE PROGRAM WRITES AN COUNT OF HANDLE PACKETS OF THIS TYPE
// 					EXIT -> PROGRAM CALLS LIST COMMAND AND EXITS
// 
//				3. POLL THE INCOMING FIFOS FROM THE CONTROLLED AND THE ATTACHED SWITCHES
//						THE SWITCH HANDLES EACH INCOMING PACKET AS DESCBIED IN PACKET TYPES
// 
// 
// REVIEW FLOWTABLE 
// EACH ENTRY IN THE FLOW TABLE STORES  [srcIP_lo, srcIP_hi, destIP_lo, destIP_hi, actiontype, actionval, pri, pktcount]
// PACKET HEADER FIELDS :
// srcIP and destIP
//  				(srcIP_lo, srcIP_hi) and (destIP_lo, destIP_hi) specific range of IP 
//					A packet matches the pattern in a flow table if scrIP is an element of [srcIP_lo, srcIP_hi] and destIP is an element of [destIP_lo, destIP_hi]
// actiontype
// 					actiontype = forward -> then actionVal specifies the switch port to which the packet should be forwarded
// 					actiontype = drop -> then actionVal is not used (null)
// pri 
//				specifies rule priority (not that important)
// 				pri = 4; 		
//
// REVIEW  PACKET TYPES : 
//OPEN & ACK 
// 				OPEN = SWITCH SENDS OPEN PACKET TO CONTROLLER (CARRIED MESSAGE CONTAINS SWITCH NUMBER, THE NUMBERS OF ITS NEIHBORING SWITCHES (IF ANY) AND THE RANGE OF IP ADDRESSES SERVED BY THE SWITCH)
// 				ACK = UPON RECEIVING AN OPEN PACKET THE CONTROLLER UPDATES ITS STORED INFORMATIOON ABOUT THE SWITCH AND REPLIES WITH A PACKET OF TYPE ACK 
// QUERY & ADD
// 				WHEN PROCESSSING AN INCOMING PACKET HEADER THE HEADER MAY BE READ FROM THE TRAFFIC FFILE, OR RELAYED TO THE SWITCH BY ONE OF ITS NEIGHBORS
// 					QUERY = IF SWITCH DOES NOT FIND A MATCHING RULE IN THE FLOW TABLE, THE SWITCH SENDS A QUERY PACKET TO THE CONTROLLER
//					ADD = THE CONTROLLER REPLEIS WITH A RULE STORED IN A PACKET OF TYPE ADD
// RELAY
// 				RELAY = A SWITCH MAY FORWARD A RECIEVED PACKET HEADER TO A NEIGHBOR (AS INSTRUCTED BY A MATCHING RULE IN THE FLOW TABLE )
// 						THIS INFORMATION IS PASSED OT THE NEIGHBOUR IN A RELAY PACKET
// 			


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
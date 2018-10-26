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

#include "controller.h"
#include "switch.h"

void err_sys(const char *x);

int main(int argc, char **argv)
{
	if (argc != 3 && argc != 6)
	{
		err_sys("Not Enough Arguments Given");
	}
	if (argc == 3)
	{
		Controller controller;
		packetStats ControllerPacketStats;
		detectController(argv, &controller);
		// CONTROLLER LOOP //
		ControllerLoop(controller.nSwitch);

		// for each switch (0- nswitch-1) connect to fifos
	}
	if (argc == 6)
	{
		SWI swi;
		packetStats stats;
		detectSwitch(argv, &swi);
		vector<flowEntry> flowtable;
		initializeCurrentFlowEntry(&swi, flowtable);
		// start a loop and monitor process via poll/select
		// readFILE(swi.filename, &swi, &stats, flowtable);
		// start loop
		switchLoop(&swi);
	}
	return 1;
}

void err_sys(const char *x)
{
	perror(x);
	exit(1);
}

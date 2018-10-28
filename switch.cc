// ============================================== SWITCH.cc ========================================================== //

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

// void initializeCurrentFlowEntry(SWI *swi, vector<flowEntry> &flowtable)
// {
//     // create an entry in the flow table
//     flowtable.push_back(flowEntry());
//     // set up flowtable entry for first entry
//     flowtable[0].srcIP = "0-1000";
//     flowtable[0].destIP = swi->IP_ADDR;
//     flowtable[0].srcIP_lo = 0;
//     flowtable[0].srcIP_lo = 1000;
//     flowtable[0].destIP_lo = swi->IP_LOW;
//     flowtable[0].destIP_hi = swi->IP_HIGH;
//     flowtable[0].actionType = "FORWARD";
//     flowtable[0].pri = 4;
//     flowtable[0].actionVal = 3;
//     flowtable[0].pktcount = 0;
// }

// I know its code duplication but im running low on sleep and i just dont care anymore at this point !!! im sorry.
void createNewFlowEntry(vector<flowEntry> &flowtable, int destIPLow, int destIPHigh)
{
    // cout << "teh index is .. " << index <<
    flowtable.push_back(flowEntry());
    flowtable.back().srcIP = "0-1000";
    flowtable.back().destIP = to_string(destIPLow) + "-" + to_string(destIPHigh);
    flowtable.back().srcIP_lo = 0;
    flowtable.back().srcIP_lo = 1000;
    flowtable.back().destIP_lo = destIPLow;
    flowtable.back().destIP_hi = destIPHigh;
    flowtable.back().actionType = "FORWARD:1";
    flowtable.back().pri = 4;
    flowtable.back().actionVal = 3;
    flowtable.back().pktcount = 0;
}

static void sendToFifo(string fifoname, string msg, int fd)
{
    assert(fd >= 0);
    // cout << sizeof(&packet) << "ad" << sizeof(Packet);
    // write(fd, &packet, sizeof(Packet));
    // string mg = packet->msg + " " + packet->port1 + " " + packet->port2 + " " + packet->swi + " " + packet->kind;
    write(fd, msg.c_str(), 1024);
    close(fd);
    return;
}

static string prepareMessage(string type, SWI *swi)
{
    // ANCHOR  Prepare Message for OPEN
    // Packet packet;
    // memset((char *) &packet,0,sizeof(packet));
    // packet.swi = swi->IP_ADDR + "sw" + swi->swi;

    // string message = "sw" + swi->swi;
    // packet.port1 = swi->swj;
    // packet.port2 = swi->swk;
    cout << "THE IP IS " << swi->IP_ADDR << endl;
    string message = swi->IP_ADDR + " sw" + swi->swi;
    if (strcmp(swi->swj.c_str(), "") == 0)
    {
        message += " -1";
        // packet.port1 = "-1";
    }
    else
    {
        message += " sw" + swi->swj;
    }
    if (strcmp(swi->swk.c_str(), "") == 0)
    {
        message += " -1";
    }
    else
    {
        message += " sw" + swi->swk;
    }
    message += " " + type;
    cout << message << " is the messae" << endl;
    // packet.msg = swi->IP_ADDR;

    // packet.kind = type;
    return message;
}

static void grabIntsFromLine(int *IP, int n, char *line)
{
    char *tok = strtok(line, " \n");
    int counter = 0;
    while (tok != NULL)
    {
        if (counter > 0 && counter <= n)
        {
            int x = atoi(tok);
            IP[counter - 1] = x;
        }
        counter++;
        tok = strtok(NULL, " \n");
    }
    return;
}

// static void sendMessageToFifo(string message, int fd)
// {
//     assert(fd >= 0);
//     write(fd, message.c_str(), 1024);
//     close(fd);
//     return;
// }

static void readTrafficFile(FILE **fdTrafficFile, string toController, SWI **swi, vector<flowEntry> &flowtable, PACKETSWI *switchPacketCounts)
{
    // ANCHOR  READTRAFFICFILE
    char line[1024];
    string currentSWI = "sw" + (*swi)->swi;
    while (fgets(line, sizeof(line), *fdTrafficFile))
    {
        if (!strstr(line, "#"))
        {
            if (strstr(line, currentSWI.c_str()))
            {
                // cout << line << endl;
                int IP[2]; // create a array of 2 ints specified for both ip ranges
                grabIntsFromLine(IP, 2, line);
                // cout << IP[0] << IP[1] << endl;
                int inRange = 0;
                for (size_t i = 0; i < flowtable.size(); i++)
                {
                    if (flowtable[i].destIP_lo <= IP[0] && flowtable[i].destIP_hi >= IP[1])
                    {
                        inRange = 1;
                        flowtable[i].pktcount++;
                        cout << "pktcount is " << flowtable[i].pktcount << endl;
                    }
                }
                if (!inRange)
                {
                    // create a new entry in the flow table and send a message of query to the controller
                    // createNewFlowEntry(flowtable, IP[1], IP[1]);
                    // flowtable.back().pktcount++;
                    // flowtable.back().actionType = "DROP:0";
                    // SWI *sw = &(**swi);
                    // string packet = prepareMessage("QUERY", sw);
                    string fifoname = "fifo-" + (*swi)->swi + "-0";
                    string message = to_string(IP[0]) + " " + to_string(IP[1]) + " QUERY";
                    switchPacketCounts->transmittedPacket.QUERY++;
                    int fd = open(fifoname.c_str(), O_WRONLY | O_NONBLOCK);
                    // i need to send the controller a message of the form "switchDestIP fileDESTIPlo fileDestIPHigh QUERY"
                    sendToFifo("fifo-" + (*swi)->swi + "-0", message, fd);
                    // sendMsgToFifo(fifoname, message, fd);
                    close(fd);
                }
            }
        }
    }
    // fclose(*fdTrafficFile);
}

static void printFlowTable(vector<flowEntry> &flowtable)
{
    for (size_t i = 0; i < flowtable.size(); i++)
    {
        printf("[%d] (srcIP= %s, destIP= %s, action= %s, pri= %d, pktCount= %d\n", (int)i, flowtable[i].srcIP.c_str(), flowtable[i].destIP.c_str(), flowtable[i].actionType.c_str(), flowtable[i].pri, flowtable[i].pktcount);
    }
}
// =============================================================== SWITCH LOOP =====================================================================================
void switchLoop(SWI *swi, vector<flowEntry> &flowtable)
{
    // ANCHOR SWITCHLOOP
    // switchPacketCounts.recievedPacket = {0};
    // switchPacketCounts.transmittedPacket = {0};
    PACKETSWI switchPacketCounts = {{0}, {0}};
    int fd = 0;
    int maxFDS = 0;  // stdin = 0;
    char buf[1025];  // buffer to listen to list and exit
    int ret, sret;   // ret - return and sret - select return
    fd_set readFds;  // readfds - fdset data strucutre
    timeval timeout; // time out structure for switchloop
    vector<int> fileDesc;
    fileDesc.push_back(fd);

    // TODO  CREATE A SEPEPARATE FUNCTION THAT DOES THIS

    int fdToCont, fdFromCont;
    string fifoToSwitchLeft, fifoFromSwitchLeft, fifoToSwitchRight, fifoFromSwitchRight = "";

    // creates controller fifos
    // open for writing to controller
    string fifoToController = "fifo-" + swi->swi + "-0";
    // open for reading from controller
    string fifoControllerToSwitch = "fifo-0-" + swi->swi;

    // ================================ OPEN FIFO To Controller FOR WRITING TO CONTROLLER (fifo-swi-0) ===============================================
    fdToCont = open(fifoToController.c_str(), O_RDWR | O_NONBLOCK); // may not need this but works so im not going to change it lol
    if (fdToCont == -1)
        err_sys("unable to open fifo to controller ");
    // maxFDS++;

    // openpacket needs to be sent to controller! - prepare the message to be sent to controller
    switchPacketCounts.transmittedPacket.OPEN++;
    string openpacket = prepareMessage("OPEN", swi);
    sendToFifo(fifoToController, openpacket, fdToCont);
    close(fdToCont);

    // ================================ OPEN FIFO From Controller (fifo-0-swi)  FOR READING FROM CONTROLLER =========================================

    fdFromCont = open(fifoControllerToSwitch.c_str(), O_RDONLY | O_NONBLOCK);
    if (fdFromCont == -1)
    {
        err_sys("unable to open fifo from controller");
    }
    // maxFDS++;
    fileDesc.push_back(fdFromCont);

    int fdTrafficFile = open(swi->filename.c_str(), O_RDONLY | O_NONBLOCK);
    if (fdTrafficFile == -1)
    {
        err_sys("unable to open traffic file");
    }

    FILE *trafficFile = fdopen(fdTrafficFile, "r");
    fileDesc.push_back(fdTrafficFile);

    int fdleft, fdRight = 0;
    if (swi->positionLeft)
    {
        // open file for positonleft of switch
        string fifoleft = "fifo-" + swi->swk + "-" + swi->swi;
        fdleft = open(fifoleft.c_str(), O_RDONLY | O_NONBLOCK);
        fileDesc.push_back(fdleft);
    }
    if (swi->positionRight)
    {
        string fifoRight = "fifo-" + swi->swj + "-" + swi->swi;
        fdRight = open(fifoRight.c_str(), O_RDONLY | O_NONBLOCK);
        fileDesc.push_back(fdRight);
    }

    for (std::vector<int>::size_type i = 0; i != fileDesc.size(); i++)
    {
        cout << fileDesc[i] << endl;
        if (fileDesc[i] > maxFDS)
        {
            maxFDS = fileDesc[i];
        }
    }

    while (1)
    {
        FD_ZERO(&readFds);
        FD_SET(fd, &readFds);
        // FD_SET(fdToCont, &readFds);
        FD_SET(fdFromCont, &readFds);
        FD_SET(fdTrafficFile, &readFds);
        if (fdleft != 0)
        {
            FD_SET(fdleft, &readFds);
        }
        if (fdRight != 0)
        {
            FD_SET(fdRight, &readFds);
        }
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        sret = select(maxFDS + 1, &readFds, NULL, NULL, &timeout);
        if (sret == -1)
            err_sys("select call error");
        else if (sret)
        {

            if (FD_ISSET(0, &readFds))
            {
                memset(buf, 0, sizeof(buf));
                ret = read(fd, (void *)buf, sizeof(buf));
                if (ret != -1)
                {
                    if (strcmp(buf, "list\n") == 0)
                    {
                        cout << "list" << endl;
                        printFlowTable(flowtable);
                    }
                    else if (strcmp(buf, "exit\n") == 0)
                    {
                        cout << "list then exit" << endl;
                    }
                }
            }
            if (FD_ISSET(fdTrafficFile, &readFds))
            {
                readTrafficFile(&trafficFile, fifoToController, &swi, flowtable, &switchPacketCounts);
            }
            if (FD_ISSET(fdFromCont, &readFds))
            {
                int len = 0;
                char *recieve = (char *)calloc(100, sizeof(char));
                len = read(fdFromCont, recieve, 100);
                if (len == 100)
                {
                    cout << recieve << endl;
                    cout << "bout to give up";
                    cout << recieve << endl;
                    // case ACK, ADD, RELAYIN, RELAYOUT
                    if (strcmp(recieve, "ACK") == 0)
                    {
                        switchPacketCounts.recievedPacket.ACK++;
                    }
                    if (strstr(recieve, "ADD"))
                    {
                        int IP[2];
                        grabIntsFromLine(IP, 2, recieve);
                        switchPacketCounts.recievedPacket.ADDRULE++;
                        createNewFlowEntry(flowtable, IP[0], IP[1]);
                        flowtable.back().pktcount++;
                        flowtable.back().actionType = "DROP:0";
                        // parseReceived(&recieve);
                        // create a new entry in the flow table
                    }
                }
            }
        }
    }
}

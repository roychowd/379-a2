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
    // now that the flowtable is set up we need to use io multiplexing and

    // read and process trafficfile
    // read the file and ignore # and empty lines and ones that dont have the swi
}

// I know its code duplication but im running low on sleep and i just dont care anymore at this point !!! im sorry.
static void createNewFlowEntry(size_t index, vector<flowEntry> &flowtable, string destIPLow, string destIPHigh)
{
    // cout << "teh index is .. " << index <<
    flowtable.push_back(flowEntry());
    flowtable[flowtable.size() - 1].srcIP = "0-1000";
    flowtable[flowtable.size() - 1].destIP = destIPLow + "-" + destIPHigh;
    flowtable[flowtable.size() - 1].srcIP_lo = 0;
    flowtable[flowtable.size() - 1].srcIP_lo = 1000;
    flowtable[flowtable.size() - 1].destIP_lo = atoi(destIPLow.c_str());
    flowtable[flowtable.size() - 1].destIP_hi = atoi(destIPHigh.c_str());
    flowtable[flowtable.size() - 1].actionType = "FORWARD";
    flowtable[flowtable.size() - 1].pri = 4;
    flowtable[flowtable.size() - 1].actionVal = 3;
    flowtable[flowtable.size() - 1].pktcount = 1;
}

void readFILE(string filename, SWI *swi, packetStats *stats, vector<flowEntry> flowtable)
{
    // re do this with io multiplexing and query and stuff

    string line;
    ifstream myfile;
    myfile.open(filename.c_str());
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if ((line.find("sw" + swi->swi) != std::string::npos) && (strstr(line.c_str(), "#") == NULL))
            {
                cout << line << endl;
                // NOTE i separated whitespace using this resource: https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string?rq=1
                // all created for separating whitespace from string goes to user Zunino on stack overflow
                istringstream li(line);
                vector<string> tokens{istream_iterator<string>{li}, istream_iterator<string>{}};
                if (atoi(tokens.at(1).c_str()) >= swi->IP_LOW && atoi(tokens.at(2).c_str()) <= swi->IP_HIGH)
                {
                    stats->ADMIT++;
                    flowtable[0].pktcount++;
                }
                else
                {
                    // create a new entry in flowtable
                    int size = flowtable.size();
                    for (int i = 0; i < size; i++)
                    {
                        if (atoi(tokens.at(1).c_str()) >= flowtable.at(i).destIP_lo && atoi(tokens.at(2).c_str()) <= flowtable.at(i).destIP_hi)
                        {
                            flowtable.at(i).pktcount++;
                            break;
                        }
                        else if (i == size - 1)
                        {
                            createNewFlowEntry(flowtable.size(), flowtable, tokens.at(1), tokens.at(2));
                        }
                    }
                }
            }
        }
    }
}

static void sendToFifo(string fifoname, Packet packet, int fd)
{
    assert(fd >= 0);
    write(fd, &packet, sizeof(packet));
}

static Packet prepareMessage(KIND type, string msg)
{
    Packet packet;
    packet.msg = msg;
    packet.kind = type;
    return packet;
}

void switchLoop(SWI *swi)
{
    // ANCHOR SWITCHLOOP
    int fd = 0;
    int maxFDS = 1;  // stdin = 0;
    char buf[1025];  // buffer to listen to list and exit
    int ret, sret;   // ret - return and sret - select return
    fd_set readFds;  // readfds - fdset data strucutre
    int type = 0;    //  may need this later
    timeval timeout; // time out structure for switchloop
                     // vector<fifoStruct> fifos = setupSwitchFifos(swi->);
    int fifoCont, fifoLeft, fifoRight;
    string fifoToSwitchLeft, fifoToSwitchRight = "";
    string fifoToController = "fifo-" + swi->swi + "-0";
    fifoCont = open(fifoToController.c_str(), O_RDWR | O_NONBLOCK);
    maxFDS++;
    if (fifoCont == -1)
    {
        err_sys("unable to open fifo to controller ");
    }
    Packet openpacket = prepareMessage(OPEN, swi->IP_ADDR);
    sendToFifo(fifoToController, openpacket, fifoCont);

    if (swi->swk.compare("") != 0)
    {
        fifoToSwitchRight = "fifo-" + swi->swi + "-" + swi->swk;
        fifoRight = open(fifoToSwitchRight.c_str(), O_RDWR | O_NONBLOCK);
        if (fifoRight == -1)
        {
            err_sys("Unable to open fifo to right switch");
        }
        maxFDS++;
    }
    if (swi->swj.compare("") != 0)
    {
        fifoToSwitchLeft = "fifo-" + swi->swi + "-" + swi->swj;
        fifoLeft = open(fifoToSwitchLeft.c_str(), O_RDWR | O_NONBLOCK);
        if (fifoLeft == -1)
        {
            err_sys("Unable to open fifo to left switch ");
        }
        maxFDS++;
    }
    while (1)
    {
        FD_ZERO(&readFds);
        FD_SET(fd, &readFds);
        FD_SET(fifoCont, &readFds);
        if (fifoToSwitchLeft.compare("") != 0)
            FD_SET(fifoLeft, &readFds);
        if (fifoToSwitchRight.compare("") != 0)
            FD_SET(fifoLeft, &readFds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        sret = select(maxFDS + 1, &readFds, NULL, NULL, &timeout);
        if (sret == -1)
            err_sys("select call error");
        if (sret == 0)
        {
            if (type == 0)
            {
                continue;
            }
        }
        else
        {
            memset(buf, 0, sizeof(buf));
            ret = read(fd, (void *)buf, sizeof(buf));
            if (ret != -1)
            {
                // cout << "buf = " << buf << endl;

                if (strcmp(buf, "list\n") == 0)
                {
                    cout << "list" << endl;
                }
                else if (strcmp(buf, "exit\n") == 0)
                {
                    cout << "list then exit" << endl;
                }
            }
        }
    }
}

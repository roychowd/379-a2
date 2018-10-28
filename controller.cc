#include "controller.h"

static void err_sys(const char *x)
{
	perror(x);
	exit(EXIT_FAILURE);
}

// a function that opens all fifos needed specified by user parameter nSwitch
// returns a vector that contains the name of the fifo as well as the
static vector<fifoStruct> setUpFifos(int nswitch)
{
	vector<fifoStruct> fifos;
	string fifoname = "fifo-";
	for (int index = 1; index <= nswitch; index++)
	{
		fifoStruct fifo;
		fifo.fifoNameContToSwi = fifoname + "0-" + to_string(index); // means fifo-0-x
		fifo.fifoNameSwiToCont = fifoname + to_string(index) + "-0"; // means fifo-x-0 <-- needs to be open for reading
		// fifo.fileDescriptorContToSwi = open(fifo.fifoNameContToSwi.c_str(), O_RDONLY | O_NONBLOCK);
		fifo.fileDescriptorSwiToCont = open(fifo.fifoNameSwiToCont.c_str(), O_RDONLY | O_NONBLOCK);

		if (fifo.fileDescriptorSwiToCont == -1)
		{
			err_sys("Error opening FIFO's. MAKE CLEAN and then run MAKE");
		}
		fifos.push_back(fifo);
	}
	return fifos;
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

// READs incoming signals
static vector<string> readPacket(std::vector<fifoStruct>::iterator it)
{
	// ANCHOR  READ-PACKET
	int len = 0;
	std::vector<string> tokens;
	char *c = (char *)calloc(1024, sizeof(char));
	len = read(it->fileDescriptorSwiToCont, c, 1024);
	if (len != -1 && strlen(c) != 0)
	{
		// cout << c << endl;
		string messageString = string(c);
		// char *token = strtok(c, " \n");
		// while (token != NULL)
		// {
		// 	cout << token << " AESFDDfs" << endl;
		// 	token = strtok(NULL, " \n");
		// }
		size_t pos = messageString.find(" ");
		size_t initialPosition = 0;

		while (pos != std::string::npos)
		{
			tokens.push_back(messageString.substr(initialPosition, pos - initialPosition));
			initialPosition = pos + 1;
			pos = messageString.find(" ", initialPosition);
		}
		tokens.push_back(messageString.substr(initialPosition, std::min(pos, messageString.size()) - initialPosition + 1));

		return tokens;
	}
	else
	{
		return tokens;
	}
}

static void sendToSwitch(string fifoname, string senderPacket)
{
	int fd = open(fifoname.c_str(), O_WRONLY | O_NONBLOCK);
	write(fd, senderPacket.c_str(), 100);
	close(fd);
}

static void createNewFLowTableEntry(vector<flowEntryForController> &flowtable, vector<string> &recieved)
{
	cout << recieved.at(0) << "wtf " << endl;
	flowEntryForController newEntry;
	newEntry.port3 = recieved[0];
	newEntry.port1 = recieved[2];
	newEntry.port2 = recieved[3];
	newEntry.swi = recieved[1];
	flowtable.push_back(newEntry);
}
static void testType(string kind, string fifoname, vector<string> &packRecieve, PACKETCONT *controllerPacketCount, vector<flowEntryForController> &flowtable)
{
	cout << kind;
	if (strcmp(kind.c_str(), "OPEN") == 0)
	{
		string s = "ACK";
		createNewFLowTableEntry(flowtable, packRecieve);
		controllerPacketCount->receivedPackets.OPEN++;
		controllerPacketCount->transmittedPackets.ACK++;
		sendToSwitch(fifoname, s);
	}
	else if (strcmp(kind.c_str(), "QUERY") == 0)
	{
		cout << "WE GOT QUEEERY" << endl;
		// iterate through my flowtable and send drop or add but for now ill do add
		controllerPacketCount->receivedPackets.QUERY++;
		controllerPacketCount->transmittedPackets.ADD++;
		string s = packRecieve[0] + " " + packRecieve[1] + " ADD";
		sendToSwitch(fifoname, s);
		// return;
	}
}

static void printFlowTable(vector<flowEntryForController> &flowtable)
{
	for (size_t i = 0; i < flowtable.size(); i++)
	{
		printf("[%s] port1= %s, port2= %s, port3= %s\n", flowtable[i].swi.c_str(), flowtable[i].port1.c_str(), flowtable[i].port2.c_str(), flowtable[i].port3.c_str());
	}
}

void ControllerLoop(int nswitch, vector<flowEntryForController> &flowtable)
{
	// ANCHOR  Controller Loop
	// use IO multiplexing Select() and poll() to handle IO from the keyboard and the attached switches in a nonblocking manner
	// set of file descriptors ( need to monitor conncections to stdin and piping information!!! )
	// controllerPacketCounts.receivedPackets = {0};
	// controllerPacketCounts.transmittedPackets = {0};
	PACKETCONT controllerPacketCount = {{0}, {0}};
	int fd = 0; // stdin = 0;
	char buf[1025];
	// int maxFDS = 1;
	int ret, sret;
	fd_set readFds;
	int type = 0;
	timeval timeout;
	vector<fifoStruct> fifos = setUpFifos(nswitch);
	int maxfd = 0;
	for (size_t x = 0; x < fifos.size(); x++)
	{
		// FD_SET(fifos.at(x).fileDescriptorContToSwi, &readFds);
		if (fifos.at(x).fileDescriptorSwiToCont > maxfd)
		{
			maxfd = fifos.at(x).fileDescriptorSwiToCont;
		}
	}
	cout << "maxfd is " << maxfd << endl;

	while (1)
	{
		FD_ZERO(&readFds);
		FD_SET(fd, &readFds);

		for (int x = 0; x < fifos.size(); x++)
		{
			// FD_SET(fifos.at(x).fileDescriptorContToSwi, &readFds);
			FD_SET(fifos.at(x).fileDescriptorSwiToCont, &readFds);
		}
		timeout.tv_sec = 9;
		timeout.tv_usec = 0;

		sret = select(maxfd + 1, &readFds, NULL, NULL, &timeout);
		if (sret == -1)
		{
			err_sys("Select call error");
		}
		if (sret == 0)
		{
			if (type == 0)
			{
			}
		}
		else
		{

			if (FD_ISSET(0, &readFds))
			{
				memset(buf, 0, sizeof(buf));
				ret = read(fd, (void *)buf, sizeof(buf));
				if (ret != -1)
				{
					if (strcmp(buf, "list\n") == 0)
					{
						
						printFlowTable(flowtable);
					}
					else if (strcmp(buf, "exit\n") == 0)
					{
						exit(EXIT_SUCCESS);
					}
				}
			}

			for (std::vector<fifoStruct>::iterator it = fifos.begin(); it != fifos.end(); ++it)
			{

				if (FD_ISSET(it->fileDescriptorSwiToCont, &readFds))
				{
					vector<string> packRecieve = readPacket(it);

					// cout << packRecieve.kind << endl;
					// if (strcmp(packRecieve.kind.c_str(), "") != 0)
					if (packRecieve.size() != 0)
					{
						testType(packRecieve.back(), it->fifoNameContToSwi, packRecieve, &controllerPacketCount, flowtable);
					}
					else
					{
						continue;
					}
					cout << "written to " << it->fifoNameContToSwi << endl;
				}
			}
		}
	}
}
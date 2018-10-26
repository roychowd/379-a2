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
static Packet readPacket(std::vector<fifoStruct>::iterator it)
{
	// ANCHOR  READ-PACKET
	int len = 0;
	// Packet pkt;
	// memset((Packet *) &pkt, 0, sizeof(pkt));
	Packet newPacket;
	char *c = (char *)calloc(100, sizeof(char));
	len = read(it->fileDescriptorSwiToCont, c, 100);
	if (len != -1 && strlen(c) != 0)
	{
		string messageString = string(c);
		size_t pos = messageString.find(" ");
		size_t initialPosition = 0;
		std::vector<string> tokens;
		while (pos != std::string::npos)
		{
			tokens.push_back(messageString.substr(initialPosition, pos - initialPosition));
			initialPosition = pos + 1;
			pos = messageString.find(" ", initialPosition);
		}
		tokens.push_back(messageString.substr(initialPosition, std::min(pos, messageString.size()) - initialPosition + 1));
		newPacket.kind = tokens.at(4);
		newPacket.msg = tokens.at(0);
		newPacket.port1 = tokens.at(1);
		newPacket.port2 = tokens.at(2);
		newPacket.swi = tokens.at(3);

		return newPacket;
	}
	else
	{

		newPacket.kind = "";
		return newPacket;
	}
}

static void sendToSwitch(string fifoname, string senderPacket)
{
	int fd = open(fifoname.c_str(), O_WRONLY | O_NONBLOCK);
	write(fd, senderPacket.c_str(), 100);
	close(fd);
}
static void testType(string kind, string fifoname)
{
	cout << kind;
	if (strcmp(kind.c_str(), "OPEN") == 0)
	{
		cout << "exeted";
		string s = "ACK";
		sendToSwitch(fifoname, s);
	}
	else
	{
		return;
	}
}

void ControllerLoop(int nswitch)
{
	// ANCHOR  Controller Loop
	// use IO multiplexing Select() and poll() to handle IO from the keyboard and the attached switches in a nonblocking manner
	// set of file descriptors ( need to monitor conncections to stdin and piping information!!! )
	int fd = 0; // stdin = 0;
	char buf[1025];
	// int maxFDS = 1;
	int ret, sret;
	fd_set readFds;
	int type = 0;
	timeval timeout;
	vector<fifoStruct> fifos = setUpFifos(nswitch);

	// if (nswitch == 1)
	// {
	// 	maxFDS = 4;
	// }
	// cout << maxFDS << endl;
	int maxfd = 0;
	for (int x = 0; x < fifos.size(); x++)
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
		timeout.tv_sec = 1;
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
						cout << "list" << endl;
					}
					else if (strcmp(buf, "exit\n") == 0)
					{
						cout << "list then exit" << endl;
					}
				}
			}

			for (std::vector<fifoStruct>::iterator it = fifos.begin(); it != fifos.end(); ++it)
			{

				if (FD_ISSET(it->fileDescriptorSwiToCont, &readFds))
				{
					Packet packRecieve;
					packRecieve = readPacket(it);
					// cout << packRecieve.kind << endl;
					if (strcmp(packRecieve.kind.c_str(), "") != 0)
					{
						testType(packRecieve.kind, it->fifoNameContToSwi);
					}
					else
					{
						continue;
					}
					cout << "written to " << it->fifoNameContToSwi << endl;
					// cout << "returned here" << endl;
					// packSend = createPacket(type);
					// cout << "writing to fifo" << endl;
					// int asd = open("fifo-0-1", O_WRONLY | O_NONBLOCK);
					// assert(asd >= 0);
					// write(asd, &packSend, sizeof(packSend));
				}
			}
		}
	}
}
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
		fifo.fifoNameContToSwi = fifoname + "0-" + to_string(index); // means fifo-0-x <-- needs to be open for writing
		fifo.fifoNameSwiToCont = fifoname + to_string(index) + "-0"; // means fifo-x-0 <-- needs to be open for reading
		fifo.fileDescriptorContToSwi = open(fifo.fifoNameContToSwi.c_str(), O_RDONLY | O_NONBLOCK);
		fifo.fileDescriptorSwiToCont = open(fifo.fifoNameSwiToCont.c_str(), O_RDWR | O_NONBLOCK);
		// fifoStruct fifo, fifo2;
		// fifo2.FifoName = fifoname + "0-" + to_string(index);
		// fifo.FifoName = fifoname + to_string(index) + "-0";
		// cout << fifo2.FifoName << endl;
		// fifo.fileDescriptor = open(fifo.FifoName.c_str(), O_RDONLY | O_NONBLOCK);
		// fifo2.fileDescriptor = open(fifo2.FifoName.c_str(), O_RDWR | O_NONBLOCK);
		if (fifo.fileDescriptorContToSwi == -1 || fifo.fileDescriptorSwiToCont == -1)
		{
			err_sys("Error opening FIFO's. MAKE CLEAN and then run MAKE");
		}
		fifos.push_back(fifo);
		maxFDS += 2;
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

// void startFIFOControllerToSwitch(SWI *swi)
// {
// 	string fifoname = "fifo-0-" + swi->swi;
// }

// READs incoming signals
static KIND readPacket(std::vector<fifoStruct>::iterator it)
{
	int len = 0;
	Packet packet;
	memset(&packet, 0, sizeof(packet));
	len = read(it->fileDescriptorSwiToCont, (void *)&packet, sizeof(packet));
	// cout << len << endl;
	if (len != -1)
	{
		// cout << packet.swi << endl;
		// cout << packet.port1 << endl;
		// cout << packet.port2 << endl;
		// cout << packet.kind << endl;
		// cout << packet.msg << endl;

		switch (packet.kind)
		{
		case OPEN:
			return ACK;
		default:
			return NONE;
		}
	}
	return NONE;
}

static Packet createPacket(KIND type)
{
	Packet packet;
	packet.kind = type;
	switch (type)
	{
	case ACK:
		packet.msg = "";
		packet.port1 = "";
		packet.port2 = "";
		packet.swi = "";
	default:
		return packet;
	}

	return packet;
}
void ControllerLoop(int nswitch)
{
	// ANCHOR  Controller Loop
	// use IO multiplexing Select() and poll() to handle IO from the keyboard and the attached switches in a nonblocking manner
	// set of file descriptors ( need to monitor conncections to stdin and piping information!!! )
	int fd = 0; // stdin = 0;
	char buf[1025];
	int ret, sret;
	fd_set readFds;
	int type = 0;
	timeval timeout;
	vector<fifoStruct> fifos = setUpFifos(nswitch);
	while (1)
	{
		FD_ZERO(&readFds);
		FD_SET(fd, &readFds);
		for (int x = 0; x < fifos.size(); x++)
		{
			// set fds for select includes all open fifos
			FD_SET(fifos.at(x).fileDescriptorContToSwi, &readFds);
			FD_SET(fifos.at(x).fileDescriptorSwiToCont, &readFds);
		}
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		sret = select(maxFDS + 1, &readFds, NULL, NULL, &timeout);
		if (sret == -1)
		{
			err_sys("Select call error");
		}
		if (sret == 0)
		{

			// }

			// may need to delete this

			if (type == 0)
			{
			}
		}
		else
		{

			if (FD_ISSET(0, &readFds))
			{
				cout << sret << endl;
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
				/* std::cout << *it; ... */
				if (FD_ISSET(it->fileDescriptorSwiToCont, &readFds))
				{
					Packet packSend;
					KIND type = readPacket(it);
					packSend = createPacket(type);
					cout << "type " << type << "kind " << packSend.kind << endl;
					write(it->fileDescriptorContToSwi, &packSend, sizeof(packSend));

					// if (FD_ISSET(it->fileDescriptorContToSwi, &readFds))
					// {
					// }
				}
			}
		}
	}
}
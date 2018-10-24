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
		fifoStruct fifo, fifo2;
		fifo2.FifoName = fifoname + "0-" + to_string(index);
		fifo.FifoName = fifoname + to_string(index) + "-0";
		cout << fifo2.FifoName << endl;
		fifo.fileDescriptor = open(fifo.FifoName.c_str(), O_RDONLY | O_NONBLOCK);
		fifo2.fileDescriptor = open(fifo2.FifoName.c_str(), O_RDWR | O_NONBLOCK);
		if (fifo2.fileDescriptor == -1 || fifo.fileDescriptor == -1)
		{
			cout << fifo2.fileDescriptor << fifo.fileDescriptor << endl;
			err_sys("Error opening FIFO's. MAKE CLEAN and then run MAKE");
		}
		fifos.push_back(fifo);
		fifos.push_back(fifo2);
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

void startFIFOControllerToSwitch(SWI *swi)
{
	string fifoname = "fifo-0-" + swi->swi;
}

void ControllerLoop(int nswitch)
{
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
			FD_SET(fifos.at(x).fileDescriptor, &readFds);
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
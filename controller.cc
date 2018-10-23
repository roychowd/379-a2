#include "controller.h"

static void err_sys(const char *x)
{
	perror(x);
	exit(1);
}
static vector<fifoStruct> setUpFifos(int nswitch)
{
	vector<fifoStruct> fifos;
	string fifoname = "fifo-";
	for (int index = 1; index <= nswitch; index++)
	{
		fifoStruct fifo;
		fifo.FifoName = fifoname + to_string(index) + "-0";
		cout << fifo.FifoName << endl;
		fifo.fileDescriptor = open(fifo.FifoName.c_str(), O_RDONLY | O_NONBLOCK);
		fifos.push_back(fifo);
		maxFDS++;
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
			FD_SET(fifos.at(x).fileDescriptor, &readFds);
		}
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		sret = select(maxFDS + 1, &readFds, NULL, NULL, &timeout);
		cout << maxFDS << endl;
		if (sret == -1)
		{
			perror("Select call error");
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
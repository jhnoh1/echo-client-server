#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

void usage() {
	printf("tcp server v1.0.0.5\n");
	printf("\n");
	printf("syntax: ts <port> [-e]\n");
	printf("  -e : echo\n");
	printf("sample: ts 1234\n");
}

struct Param {
	bool echo{false};
    bool broadcast{false};
	uint16_t port{0};

	bool parse(int argc, char* argv[]) {
		for (int i = 1; i < argc;) {
			if (strcmp(argv[i], "-e") == 0) {
				echo = true;
				i++;
				continue;
			}
            if (strcmp(argv[i], "-b") == 0) { 
                broadcast = true;
                i++;
                continue;
            }
			if (i < argc) port = atoi(argv[i++]);
		}
		return port != 0;
	}
} param;

std::vector<int> client_sockets;

void recvThread(int sd) {
	printf("connected\n");
	fflush(stdout);
	static const int BUFSIZE = 65536;
	char buf[BUFSIZE];
	while (true) {
		ssize_t res = ::recv(sd, buf, BUFSIZE - 1, 0);
		if (res == 0 || res == -1) {
			fprintf(stderr, "recv return %ld", res);
			myerror(" ");
			break;
		}
		buf[res] = '\0';
		printf("%s", buf);
		fflush(stdout);
		if (param.echo) {
			res = ::send(sd, buf, res, 0);
			if (res == 0 || res == -1) {
				fprintf(stderr, "send return %ld", res);
				myerror(" ");
				break;
			}
		}
        if (param.broadcast) { 
            for (int client_sd : client_sockets) {
                res = ::send(client_sd, buf, res, 0);
                if (res == 0 || res == -1) {
                    fprintf(stderr, "broadcast send return %ld", res);
                    myerror(" ");
                    break;
                }
            }
        }
	}
	printf("disconnected\n");
	fflush(stdout);
	::close(sd);
}

int main(int argc, char* argv[]) {
	if (!param.parse(argc, argv)) {
		usage();
		return -1;
	}

	//
	// socket
	//
	int sd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		myerror("socket");
		return -1;
	}
	//
	// setsockopt
	//
	{
		int optval = 1;
		int res = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		if (res == -1) {
			myerror("setsockopt");
			return -1;
		}
	}
	//
	// bind
	//
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = param.srcIp;
		addr.sin_port = htons(param.port);

		ssize_t res = ::bind(sd, (struct sockaddr *)&addr, sizeof(addr));
		if (res == -1) {
			myerror("bind");
			return -1;
		}
	}

	//
	// listen
	//
	{
		int res = listen(sd, 5);
		if (res == -1) {
			myerror("listen");
			return -1;
		}
	}

	while (true) {
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		int newsd = ::accept(sd, (struct sockaddr *)&addr, &len);
		if (newsd == -1) {
			myerror("accept");
			break;
		}
		client_sockets.push_back(newsd);
		std::thread* t = new std::thread(recvThread, newsd);
		t->detach();
	}
	::close(sd);
}

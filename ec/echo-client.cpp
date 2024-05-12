#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <thread>


void recvThread(int sd) {
	printf("connected\n");
	static const int BUFSIZE = 65536;
	char buf[BUFSIZE];
	while (true) {
		ssize_t res = ::recv(sd, buf, BUFSIZE - 1, 0);
		if (res == 0 || res == -1) {
			fprintf(stderr, "recv return %ld", res);
			break;
		}
		buf[res] = '\0';
		printf("%s", buf);
	}
	printf("disconnected\n");
	::close(sd);
	exit(0);
}


int main(int argc, char **argv){
	if (argc !=3){
		return -1;
	}
	int sd = ::socket(AF_INET, SOCK_STREAM, 0);
	char* ip{nullptr};
	char* port{nullptr};
	ip = argv[1];
	port =argv[2];
	struct sockaddr_in server_add;
	memset(&server_add,0,sizeof(server_add));
	server_add.sin_family = AF_INET;
	server_add.sin_port = htons((unsigned short)atoi(argv[2]));
	server_add.sin_addr.s_addr=inet_addr(argv[1]);


	sd = socket(AF_INET, SOCK_STREAM, 0);
	int optval = 1;
	int setso = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (setso == -1) {
		return -1;
	}
	int con = ::connect(sd, (struct sockaddr*)&server_add, sizeof(server_add));
	if (con == -1) {
		return -1;
	}
	std::thread t(recvThread, sd);
	t.detach();

	while (true) {
		std::string s;
		std::getline(std::cin, s);
		s += "\n";
		ssize_t res = ::send(sd, s.data(), s.size(), 0);
		if (res == 0 || res == -1) {
			fprintf(stderr, "send return %ld", res);
			break;
		}
	}
	::close(sd);
	
}

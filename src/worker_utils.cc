#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "util.hh"
#define timen currentTime().c_str()
using std::string;
using std::vector;

void disconnectClient(int sock, string reason, string username) {
	uint8_t byte = 0x0E;
	char str[64];
	memcpy(str, padString(reason).c_str(), 64);
	send(sock, &byte, 1, MSG_NOSIGNAL);
	send(sock, &str, 64, MSG_NOSIGNAL);

	// log disconnection
	if (username == "") {
		sockaddr_in client_info = { 0 };
		socklen_t addrsize = sizeof(client_info);
		getpeername(sock, reinterpret_cast<sockaddr*>(&client_info), &addrsize);
		string client_ip(inet_ntoa(client_info.sin_addr));
		printf("[%s] %s kicked: %s\n", timen, client_ip.c_str(), reason.c_str());
	}
	else {
		printf("[%s] %s kicked: %s\n", timen, username.c_str(), reason.c_str());
	}
}

void messageClient(int sock, string message) {
	char msg[64];
	memcpy(msg, padString(message).c_str(), 64);
	uint8_t byte = 0x0d;
	send(sock, &byte, 1, MSG_NOSIGNAL);
	byte = 0;
	send(sock, &byte, 1, MSG_NOSIGNAL);
	send(sock, &msg, 64, MSG_NOSIGNAL);
	printf("[%s] %s\n", timen, message.c_str());
}

void messageClientRaw(int sock, char message[64]) {
	uint8_t byte = 0x0d;
	send(sock, &byte, 1, MSG_NOSIGNAL);
	byte = 0;
	send(sock, &byte, 1, MSG_NOSIGNAL);
	send(sock, &message, 64, MSG_NOSIGNAL);
	printf("[%s] %s\n", timen, depadString(message).c_str());
}
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <dotproperties.hh>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "util.hh"
#include "fs.hh"
#include "player.hh"
#include "level.hh"
#include "worker_utils.hh"
#define timen currentTime().c_str()
using std::string;
using std::vector;

void worker::disconnectClient(player client, vector <player> &clients, string reason, string username) {
	uint8_t byte = 0x0E;
	char str[64];
	memcpy(str, padString(reason).c_str(), 64);
	send(client.sock, &byte, 1, MSG_NOSIGNAL);
	send(client.sock, &str, 64, MSG_NOSIGNAL);

	// log disconnection
	if (username == "") {
		sockaddr_in client_info = { 0 };
		socklen_t addrsize = sizeof(client_info);
		getpeername(client.sock, reinterpret_cast<sockaddr*>(&client_info), &addrsize);
		string client_ip(inet_ntoa(client_info.sin_addr));
		printf("[%s] %s kicked: %s\n", timen, client_ip.c_str(), reason.c_str());
	}
	else {
		printf("[%s] %s kicked: %s\n", timen, username.c_str(), reason.c_str());
	}

	// remove socket from list
	for (size_t i = 0; i<clients.size(); ++i) {
		if (clients[i].sock == client.sock) {
			clients.erase(clients.begin() + i);
		}
		else {
			if (username != "") worker::messageClient(clients[i], "&e" + username + " kicked: " + reason, false);
		}
	}
}

int worker::messageClient(player client, string message, bool log) {
	char msg[64];
	memcpy(msg, padString(message).c_str(), 64);
	uint8_t byte = 0x0d;
	send(client.sock, &byte, 1, MSG_NOSIGNAL);
	byte = 0;
	send(client.sock, &byte, 1, MSG_NOSIGNAL);
	if (log) printf("[%s] %s\n", timen, message.c_str());
	if (send(client.sock, &msg, 64, MSG_NOSIGNAL) != 64)
		return 1;
	else return 0;
}

void messageClientRaw(int sock, char message[64]) {
	uint8_t byte = 0x0d;
	send(sock, &byte, 1, MSG_NOSIGNAL);
	byte = 0;
	send(sock, &byte, 1, MSG_NOSIGNAL);
	send(sock, &message, 64, MSG_NOSIGNAL);
	printf("[%s] %s\n", timen, depadString(message).c_str());
}

void worker::createPlayer(player user) {
	fcreate("./players/" + user.username + ".properties");
	Properties userprops;
	userprops["rank"]   = "0";
	userprops["banned"] = "false";
	f_write("./players/" + user.username + ".properties", userprops.stringify());
}

void worker::startLoadingLevel(player user) {
	uint8_t byte = 0x02;
	send(user.sock, &byte, 1, MSG_NOSIGNAL);
}

void worker::endLoadingLevel(player user, level lvl) {
	uint8_t byte = 0x04;
	uint16_t size;
	send(user.sock, &byte, 1, MSG_NOSIGNAL);
	size = htons(lvl.size.x);
	send(user.sock, &size, 2, MSG_NOSIGNAL);
	size = htons(lvl.size.y);
	send(user.sock, &size, 2, MSG_NOSIGNAL);
	size = htons(lvl.size.z);
	send(user.sock, &size, 2, MSG_NOSIGNAL);
}

void worker::sendPlayerPosUpdate(player user, int8_t playerID, vec3 pos) {
	uint8_t  byte;
	int8_t   sbyte;
	uint16_t shrt;
	byte = 0x09;
	send(user.sock, &byte, 1, MSG_NOSIGNAL);
	send(user.sock, &playerID, 1, MSG_NOSIGNAL);
	shrt = htons(user.pos.x);
	send(user.sock, &shrt, 2, MSG_NOSIGNAL);
	shrt = htons(user.pos.y);
	send(user.sock, &shrt, 2, MSG_NOSIGNAL);
	shrt = htons(user.pos.z);
	send(user.sock, &shrt, 2, MSG_NOSIGNAL);
	byte = htons(user.pos.yaw);
	send(user.sock, &byte, 1, MSG_NOSIGNAL);
	byte = htons(user.pos.pitch);
	send(user.sock, &byte, 1, MSG_NOSIGNAL);
}

void worker::sendNewPlayer(player user, int8_t playerID, vec3 spawnpos) {
	uint8_t  byte;
	int8_t   sbyte;
	uint16_t shrt;
	char     str[64];
	byte = 0x09;
	send(user.sock, &byte, 1, MSG_NOSIGNAL);
	send(user.sock, &playerID, 1, MSG_NOSIGNAL);
	memcpy(str, padString(user.username).c_str(), 64);
	send(user.sock, str, 64, MSG_NOSIGNAL);
	shrt = htons(user.pos.x);
	send(user.sock, &shrt, 2, MSG_NOSIGNAL);
	shrt = htons(user.pos.y);
	send(user.sock, &shrt, 2, MSG_NOSIGNAL);
	shrt = htons(user.pos.z);
	send(user.sock, &shrt, 2, MSG_NOSIGNAL);
	send(user.sock, &user.pos.yaw, 1, MSG_NOSIGNAL);
	send(user.sock, &user.pos.pitch, 1, MSG_NOSIGNAL);
}
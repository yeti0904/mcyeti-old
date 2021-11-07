#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>
#include <dotproperties.hh>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "fs.hh"
#include "player.hh"
#include "util.hh"
#include "worker_utils.hh"
#include "worker.hh"
#define timen currentTime().c_str()
using std::vector;
using std::string;
using std::to_string;

void client::worker(int sock, Properties props, string salt, vector <int> &client_sockets) {
	bool   clientIdentified = false;
	bool   clientConnected  = true;
	player client;

	Properties playerProps;

	client.online = true;
	client.sock   = sock;

	sockaddr_in client_info = { 0 };
	socklen_t addrsize = sizeof(client_info);
	getpeername(sock, reinterpret_cast<sockaddr*>(&client_info), &addrsize);
	string client_ip(inet_ntoa(client_info.sin_addr));

	// temp vars
	uint8_t tmpByte;
	int8_t  tmpSByte;
	int16_t tmpShort;
	char    tmpString[64];
	uint8_t tmpByteArray[1024];
	vec3    tmpVec3;

	// packet vars
	uint8_t packetID;

	while (clientConnected) {
		recv(sock, &packetID, 1, 0);
		if (clientIdentified) {
			switch (packetID) {
				case 0x05: {
					recv(sock, &tmpVec3.x, 2, 0);
					recv(sock, &tmpVec3.y, 2, 0);
					recv(sock, &tmpVec3.z, 2, 0);
					recv(sock, &tmpByte, 1, 0);
					recv(sock, &tmpByte, 1, 0);
					break;
				}
				case 0x08: {
					recv(sock, &tmpByte, 1, 0);
					recv(sock, &tmpVec3.x, 2, 0);
					recv(sock, &tmpVec3.y, 2, 0);
					recv(sock, &tmpVec3.z, 2, 0);
					recv(sock, &tmpByte, 1, 0);
					recv(sock, &tmpByte, 1, 0);
					break;
				}
				case 0x0d: { // message
					recv(sock, &tmpByte, 1, 0);
					recv(sock, &tmpString, 64, 0);
					if (clientConnected && (
						depadString(tmpString)[0] != '/'
					)) for (size_t i = 0; i<client_sockets.size(); ++i) {
						if (
							worker::messageClient(client_sockets[i], client.username + ": " + depadString(tmpString), false)
						== 1) {
							worker::disconnectClient(sock, client_sockets, "left the game", client.username);
							clientConnected = false;
						}
						printf("[%s] %s: %s\n", timen, client.username.c_str(), depadString(tmpString).c_str());
					}
					break;
				}
				default: {
					worker::disconnectClient(sock, client_sockets, "Unexpected packet: " + to_string(packetID), "");
					clientConnected = false;
				}
			}
		}
		else {
			switch (packetID) {
				case 0x00: { // authentication packet
					string username;
					string mppass;
					recv(sock, &tmpByte, 1, 0); // protocol version
					if (tmpByte != 0x07) worker::disconnectClient(sock, client_sockets, "Old protocol version", "");
					recv(sock, &tmpString, 64, 0);                           // username
					username = depadString(tmpString);
					recv(sock, &tmpString, 64, 0);                           // mppass
					mppass = depadString(tmpString);
					recv(sock, &tmpByte, 1, 0);                              // unused
					if (md5(salt + username) != mppass) {
						worker::disconnectClient(sock, client_sockets, "Your mppass is incorrect, log out and log back in again", username);
						clientConnected = false;
					}
					
					// send server identification
					tmpByte = 0x00;                                          // Packet ID
					if (send(sock, &tmpByte, 1, MSG_NOSIGNAL) != 1) {
						worker::disconnectClient(sock, client_sockets, "left the game", client.username);
						clientConnected = false;
					}
					tmpByte = 0x07;                                          // Protocol version
					send(sock, &tmpByte, 1, MSG_NOSIGNAL);
					memcpy(tmpString, padString(props["name"]).c_str(), 64); // Server name
					send(sock, tmpString, 64, MSG_NOSIGNAL);
					memcpy(tmpString, padString(props["motd"]).c_str(), 64); // Server MOTD
					send(sock, tmpString, 64, MSG_NOSIGNAL);
					tmpByte = 0x00;                                          // User type
					send(sock, &tmpByte, 1, MSG_NOSIGNAL);
					clientIdentified = true;
					client_sockets.push_back(sock);
					client.username = username;

					playerProps.readFromFile("./players/" + client.username + ".properties");
					if (playerProps["banned"] == "true") {
						worker::disconnectClient(sock, client_sockets, "You're banned!", client.username);
						clientConnected = false;
					}

					for (size_t i = 0; i<client_sockets.size(); ++i) {
						if (!fexists("./players/" + client.username + ".properties")) {
							worker::messageClient(client_sockets[i], "&e" + client.username + " joined for the first time", false);
							printf("[%s] %s joined for the first time\n", timen, client.username.c_str());
							worker::createPlayer(client);
						}
						else {
							worker::messageClient(client_sockets[i], "&e" + client.username + " joined the game", false);
							printf("[%s] %s joined the game\n", timen, client.username.c_str());
						}
					}
					break;
				}
				default: {
					worker::disconnectClient(sock, client_sockets, "Unexpected packet: " + to_string(packetID), "");
					clientConnected = false;
				}
			}
		}
	}
}

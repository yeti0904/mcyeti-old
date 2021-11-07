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

void client::worker(player client, Properties props, string salt, vector <player> &clients) {
	bool   clientIdentified = false;
	bool   clientConnected  = true;

	Properties playerProps;

	client.online = true;

	sockaddr_in client_info = { 0 };
	socklen_t addrsize = sizeof(client_info);
	getpeername(client.sock, reinterpret_cast<sockaddr*>(&client_info), &addrsize);
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
		recv(client.sock, &packetID, 1, 0);
		if (clientIdentified) {
			switch (packetID) {
				case 0x05: {
					recv(client.sock, &tmpVec3.x, 2, 0);
					recv(client.sock, &tmpVec3.y, 2, 0);
					recv(client.sock, &tmpVec3.z, 2, 0);
					recv(client.sock, &tmpByte, 1, 0);
					recv(client.sock, &tmpByte, 1, 0);
					break;
				}
				case 0x08: {
					recv(client.sock, &tmpByte, 1, 0);
					recv(client.sock, &tmpVec3.x, 2, 0);
					recv(client.sock, &tmpVec3.y, 2, 0);
					recv(client.sock, &tmpVec3.z, 2, 0);
					recv(client.sock, &tmpByte, 1, 0);
					recv(client.sock, &tmpByte, 1, 0);
					break;
				}
				case 0x0d: { // message
					recv(client.sock, &tmpByte, 1, 0);
					recv(client.sock, &tmpString, 64, 0);
					if (clientConnected && (
						depadString(tmpString)[0] != '/'
					)) for (size_t i = 0; i<clients.size(); ++i) {
						if (
							worker::messageClient(clients[i], client.username + ": " + depadString(tmpString), false)
						== 1) {
							worker::disconnectClient(client, clients, "left the game", client.username);
							clientConnected = false;
						}
						if (clientConnected) printf("[%s] %s: %s\n", timen, client.username.c_str(), depadString(tmpString).c_str());
					}
					break;
				}
				default: {
					worker::disconnectClient(client, clients, "Unexpected packet: " + to_string(packetID), "");
					clientConnected = false;
				}
			}
		}
		else {
			switch (packetID) {
				case 0x00: { // authentication packet
					string username;
					string mppass;
					recv(client.sock, &tmpByte, 1, 0); // protocol version
					if (tmpByte != 0x07) worker::disconnectClient(client, clients, "Old protocol version", "");
					recv(client.sock, &tmpString, 64, 0);                           // username
					username = depadString(tmpString);
					recv(client.sock, &tmpString, 64, 0);                           // mppass
					mppass = depadString(tmpString);
					recv(client.sock, &tmpByte, 1, 0);                              // unused
					if (md5(salt + username) != mppass) {
						worker::disconnectClient(client, clients, "Your mppass is incorrect, log out and log back in again", username);
						clientConnected = false;
					}
					
					// send server identification
					tmpByte = 0x00;                                          // Packet ID
					if (send(client.sock, &tmpByte, 1, MSG_NOSIGNAL) != 1) {
						worker::disconnectClient(client, clients, "left the game", client.username);
						clientConnected = false;
					}
					tmpByte = 0x07;                                          // Protocol version
					send(client.sock, &tmpByte, 1, MSG_NOSIGNAL);
					memcpy(tmpString, padString(props["name"]).c_str(), 64); // Server name
					send(client.sock, tmpString, 64, MSG_NOSIGNAL);
					memcpy(tmpString, padString(props["motd"]).c_str(), 64); // Server MOTD
					send(client.sock, tmpString, 64, MSG_NOSIGNAL);
					tmpByte = 0x00;                                          // User type
					send(client.sock, &tmpByte, 1, MSG_NOSIGNAL);
					clientIdentified = true;
					clients.push_back(client);
					client.username = username;

					playerProps.readFromFile("./players/" + client.username + ".properties");
					if (playerProps["banned"] == "true") {
						worker::disconnectClient(client, clients, "You're banned!", client.username);
						clientConnected = false;
					}

					for (size_t i = 0; i<clients.size(); ++i) {
						if (!fexists("./players/" + client.username + ".properties")) {
							worker::messageClient(clients[i], "&e" + client.username + " joined for the first time", false);
							printf("[%s] %s joined for the first time\n", timen, client.username.c_str());
							worker::createPlayer(client);
						}
						else {
							worker::messageClient(clients[i], "&e" + client.username + " joined the game", false);
							printf("[%s] %s joined the game\n", timen, client.username.c_str());
						}
					}
					break;
				}
				default: {
					worker::disconnectClient(client, clients, "Unexpected packet: " + to_string(packetID), "");
					clientConnected = false;
				}
			}
		}
	}
}

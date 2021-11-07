#pragma once
#include <string>
#include <vector>
#include "player.hh"
using std::string;
using std::vector;

namespace worker {
	void disconnectClient(int sock, vector <int> &sockets, string reason, string username);
	int messageClient(int sock, string message, bool log);
	void messageClientRaw(int sock, char message[64]);
	void createPlayer(player user);
}
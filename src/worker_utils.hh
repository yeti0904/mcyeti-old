#pragma once
#include <string>
#include <vector>
#include "player.hh"
using std::string;
using std::vector;

namespace worker {
	void disconnectClient(player client, vector <player> &clients, string reason, string username);
	int messageClient(player client, string message, bool log);
	void messageClientRaw(int sock, char message[64]);
	void createPlayer(player user);
}
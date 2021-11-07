#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "player.hh"
#include "level.hh"
using std::string;
using std::vector;

namespace worker {
	void disconnectClient(player client, vector <player> &clients, string reason, string username);
	int messageClient(player client, string message, bool log);
	void messageClientRaw(int sock, char message[64]);
	void createPlayer(player user);
	void startLoadingLevel(player user);
	void endLoadingLevel(player user, level lvl);
	void sendPlayerPosUpdate(player user, int8_t playerID, vec3 pos);
	void sendNewPlayer(player user, int8_t playerID, vec3 spawnpos);
}
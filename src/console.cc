#include <string>
#include <vector>
#include <cstdint>
#include <unistd.h>
#include <readline/readline.h>
#include "worker_utils.hh"
#include "util.hh"
#include "player.hh"
#include "console.hh"
using std::string;
using std::vector;
#define timen currentTime().c_str()

void console::worker(vector <player> &clients, bool &run) {
	char*  rinput;
	string input;
	while (run) {
		rinput = (char*) malloc(64);
		read(0, rinput, 64);
		input = rinput;
		free(rinput);
		if (input[0] != '/') {
			for (size_t i = 0; i<clients.size(); ++i) {
				worker::messageClient(clients[i], "(console): " + input, false);
				printf("[%s] (console): %s\n", timen, input);
			}
		}
	}
}
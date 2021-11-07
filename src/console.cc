#include <string>
#include <vector>
#include <cstdint>
#include <unistd.h>
#include <readline/readline.h>
#include "worker_utils.hh"
#include "util.hh"
#include "console.hh"
using std::string;
using std::vector;
#define timen currentTime().c_str()

void console::worker(vector <int> &socks, bool &run) {
	char*  rinput;
	string input;
	while (run) {
		rinput = (char*) malloc(64);
		read(0, rinput, 64);
		input = rinput;
		free(rinput);
		if (input[0] != '/') {
			for (size_t i = 0; i<socks.size(); ++i) {
				worker::messageClient(socks[i], "(console): " + input, false);
				printf("[%s] (console): %s\n", timen, input);
			}
		}
	}
}
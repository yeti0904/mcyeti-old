#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <dotproperties.hh>
#include <unistd.h>
#include "util.hh"
using std::string;
using std::vector;
using std::to_string;
using std::ostringstream;
#define timen currentTime().c_str()

void* heartbeat(Properties props, string salt, vector <int> &client_sockets, bool &run) {
	string send;
	string name; // URL friendly server name (replaces spaces)
	while (run) {
		name = "";
		for (size_t i = 0; i<props["name"].length(); ++i) {
			if (props["name"][i] == ' ') {
				name += "%20";
			}
			else {
				name += props["name"][i];
			}
		}
		// create URL
		send =  props["heartbeatURL"];
		send += "?port=" + props["port"];
		send += "?max=" + props["maxPlayers"];
		send += "?name" + name;
		send += "?public=True";
		send += "?version=7";
		send += "?salt=" + salt;
		send += "?users=" + to_string(client_sockets.size());
		// Send data to heartbeat URL
		curlpp::Cleanup cleanup;
		ostringstream os;
		os << curlpp::options::Url(string(send));
		printf("[%s] Sent heartbeat: %s\n", timen, os.str().c_str());
		for (uint8_t i = 0; i<120; ++i) {
			sleep(1);
			if (!run) {
				printf("[%s] Shutting off heartbeat\n", timen);
				return NULL;
			}
		}
	}
	return 0;
}
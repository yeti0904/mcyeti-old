#include <dotproperties.hh>
#include <vector>
#include <string>
#include <cstdint>
#include "player.hh"
using std::string;
using std::vector;

void* heartbeat(Properties props, string salt, vector <player> &clients, bool &run);
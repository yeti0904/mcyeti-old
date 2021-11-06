#include <dotproperties.hh>
#include <vector>
#include <string>
#include <cstdint>
using std::string;
using std::vector;

void* heartbeat(Properties props, string salt, vector <int> &client_sockets, bool &run);
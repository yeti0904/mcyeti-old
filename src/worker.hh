#include <vector>
#include <unordered_map>
#include <dotproperties.hh>
#include "player.hh"
#include "level.hh"
using std::vector;
using std::unordered_map;

namespace client {
    void worker(player client, unordered_map <string, level> maps, Properties props, string salt, vector <player> &clients);
}
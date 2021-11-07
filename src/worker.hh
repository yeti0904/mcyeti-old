#include <vector>
#include <dotproperties.hh>
#include "player.hh"
using std::vector;

namespace client {
    void worker(player client, Properties props, string salt, vector <player> &clients);
}
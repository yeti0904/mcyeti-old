#pragma once
#include <vector>
#include "player.hh"
using std::vector;

namespace console {
    void worker(vector <player> &clients, bool &run);
}
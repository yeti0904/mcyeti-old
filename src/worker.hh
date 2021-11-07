#include <vector>
#include <dotproperties.hh>
using std::vector;

namespace client {
    void worker(int sock, Properties props, string salt, vector <int> &client_sockets);
}
cc    = g++
src   = src/*.cc
hdr   = lib/dotproperties.hh src/fs.hh src/util.hh src/worker.hh src/heartbeat.hh src/worker_utils.hh src/console.hh
out   = release/MCYeti
flags = -s -I./lib --std=c++17 -lpthread -lcurlpp -lcurl -lcrypto -lssl -lreadline -lminiupnpc

build: $(src) $(hdr)
	mkdir -p release
	$(cc) $(src) $(flags) -o $(out)

debug: $(src) $(hdr)
	mkdir -p release
	$(cc) $(src) $(flags) -o $(out) -Wall -Wextra -pedantic -Werror



release: $(src) $(hdr)
	$(cc) $(src) $(flags) -o $(out) -O3
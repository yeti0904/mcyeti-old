cc    = g++
src   = src/main.cc src/fs.cc src/util.cc src/worker.cc src/heartbeat.cc src/worker_utils.cc
hdr   = lib/dotproperties.hh src/fs.hh src/util.hh src/worker.hh src/heartbeat.hh src/worker_utils.hh
out   = release/MCYeti
flags = -s -I./lib --std=c++17 -lpthread -lcurlpp -lcurl -lcrypto -lssl

build: $(src) $(hdr)
	mkdir -p release
	$(cc) $(src) $(flags) -o $(out)

release: $(src) $(hdr)
	$(cc) $(src) $(flags) -o $(out) -O3
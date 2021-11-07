#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>
using std::string;
#define timen currentTime().c_str()

const string currentTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

string padString(string str) {
	string ret = str;
	while (ret.length() != 64) {
		ret += ' ';
	}
	return ret;
}

string depadString(char str[64]) {
	int i;
	for (i = 63; i >= 0; --i) {
		if (str[i] != ' ') {
			break;
		}
	}
	if (str[i] != ' ') {
		++i;
	}
	str[i] = '\0';

	std::string ret = str;
	return ret;
}

string md5(string src) {
	unsigned char md[MD5_DIGEST_LENGTH];
	MD5((unsigned char*) src.c_str(), src.size(), md);

	char toAdd[64];
	string sum;
	for (size_t i = 0; i<MD5_DIGEST_LENGTH; ++i) {
		sprintf(toAdd, "%02x", md[i]);
		sum += toAdd;
	}
	return sum;
}

string generateSalt() {
	unsigned char rand[8];
	RAND_bytes(rand, 8);
	string ret;
	char toAdd[64];
	for (size_t i = 0; i < 8; ++i) {
		sprintf(toAdd, "%02x", rand[i]);
		ret += toAdd;
	}
	return ret;
}

void openupnpport(string sport) {
	const char* port = sport.c_str();
	int discoverport = UPNP_LOCAL_PORT_ANY;
	struct UPNPDev *devlist;
	int err;

	if ((devlist = upnpDiscover(2000, NULL, NULL, discoverport, 0, 1, &err)) == NULL) {
		printf("[%s] Failed to find any IGD UPnP Devices.\n", timen);
		return;
	}

	{
		struct UPNPDev *dev;
		size_t i = 0;
		for (dev = devlist; dev != NULL; dev = dev->pNext) {
			//printf("[%s] UPnP Device %lu: description: %s\nst: %s\n\n", timen, i, dev->descURL, dev->st);
			++i;
		}
	}

	struct UPNPUrls urls;
	struct IGDdatas data;
	char lanaddr[64] = "unset";
	char reservedPort[6];
	int i;
	if ((i = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr))) > 0) {
		switch (i) {
			case 1:
				break;
			default:
				return;
		}

		if ((i = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, port, port, lanaddr, "MCSC Server Port", "TCP", NULL, "0")) != UPNPCOMMAND_SUCCESS) {
			printf("[%s] Failed to map port! [%d](%s)\n", timen, i, strupnperror(i));
			return;
		}
		freeUPNPDevlist(devlist);
        FreeUPNPUrls(&urls);
	}
	else {
		freeUPNPDevlist(devlist);
		FreeUPNPUrls(&urls);
	}

	return;
}
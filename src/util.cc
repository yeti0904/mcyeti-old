#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <openssl/md5.h>
using std::string;

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
	string str;
	srand((int)time(NULL));
	for (uint8_t i = 0; i<8; ++i) {
		str += 'a' + rand()%26;
	}
	string ret;
	char toAdd[64];
	for (size_t i = 0; i<8; ++i) {
		sprintf(toAdd, "%02x", str[i]);
		ret += toAdd;
	}
	return ret;
}

#pragma once
#include <string>
using std::string;

const string currentTime();
string padString(string str);
string depadString(char str[64]);
string md5(string src);
string generateSalt();
void openupnpport(string sport);
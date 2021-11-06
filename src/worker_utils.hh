#pragma once
#include <string>
using std::string;

void disconnectClient(int sock, string reason, string username);
void messageClient(int sock, string message);
void messageClientRaw(int sock, char message[64]);
#ifndef _NULL_SVR_MP_
#define _NULL_SVR_MP_
#include <afxsock.h>
#include <vector>
#include <iostream>
#include <thread>
#include "NullServer.h"
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>
#include <codecvt>
#define GMT_7 25200
using namespace std;
DWORD WINAPI accessProcessing(LPVOID lpParam); 
string makeTextResponse(int length, string type);
string getFileContent(string path);
#endif

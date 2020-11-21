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
#define CHUNK_SIZE 4096
#define GMT_7 25200
#define CSS_EXT ".css"
#define JS_EXT ".js"
#define HTML_EXT ".html"
#define ICO_EXT ".ico"
#define JPEG_EXT ".jpeg"
#define JPG_EXT ".jpg"
#define JPE_EXT ".jpe"
#define PNG_EXT ".png"
#define PATH_404 "web-source/404.html"
using namespace std;
DWORD WINAPI accessProcessing(LPVOID lpParam); 
string makeOKResponseHeader(int length, string type);
string getBinaryFileContent(string path);
string uriCutter(vector<char> buffer);
string getMIMEType(string uri);
string fixedTime(string t);
string makeDate();
string toLowercase(string in);
string getRequestMethod(vector<char> buffer);
bool isDownload(vector<vector<string>> queryVars);
string decToHex(size_t n);
#endif

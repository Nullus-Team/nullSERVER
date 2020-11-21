#ifndef _NULL_SVR_RT_
#define _NULL_SVR_RT_
#include "MessagesProcessing.h"
#include <filesystem>
#define FILE_MANAGER_PATH "web-source/files.html"
DWORD WINAPI accessProcessing(LPVOID lpParam);
void uriDecor(string& uri);
void addFiles(string& content);
string filenameCutter(string path);
string extCutter(string path);
vector<string> getDownloadable();
#endif

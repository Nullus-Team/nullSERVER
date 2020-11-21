#ifndef _NULL_SVR_RT_
#define _NULL_SVR_RT_
#include "MessagesProcessing.h"
#include <filesystem>
#define FILE_MANAGER_PATH "web-source/files.html"
#define ICON_PATH "web-source/img/icon/"
#define PATH_404 "web-source/404.html"
#define WEB_SOURCE_PATH "web-source/"
DWORD WINAPI accessProcessing(LPVOID lpParam);
void uriDecor(string& uri);
void addFiles(string& content);
string filenameCutter(string path);
string extCutter(string path);
vector<string> getDownloadable();
#endif

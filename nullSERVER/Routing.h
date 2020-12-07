#ifndef _NULL_SVR_RT_
#define _NULL_SVR_RT_
#include "MessagesProcessing.h"
#include <filesystem>
DWORD WINAPI accessProcessing(LPVOID lpParam);
void uriDecor(string& uri);
void addFiles(string& content);
string filenameCutter(string path);
string extCutter(string path);
vector<string> getDownloadable();
string dToString(double size);
string ifModifiedHeaderStringTime(vector<char> buffer);
bool modifiedFile(string uri, string cacheTime);
string filesizeDecor(uintmax_t oSize);
void cleanBuffer(vector<char>buffer);
#endif

#ifndef _NULL_SVR_RT_
#define _NULL_SVR_RT_
#include "MessagesProcessing.h"
#include <filesystem>
#define HTML_MIME "text/html"
#define COOKIE_HEADER "Cookie: "
#define IF_MODIFIED_HEADER "If-Modified-Since: "
DWORD WINAPI accessProcessing(LPVOID lpParam);
void uriDecor(string& uri);
void addFiles(string& content);
string filenameCutter(string path);
string extCutter(string path);
vector<string> getDownloadable();
string dToString(double size);
string filesizeDecor(uintmax_t oSize);
vector<vector<string>> cookieSplitter(string buffer);
string removeCookie();
bool accountChecker(vector<vector<string>> POSTvars);
bool isSession(vector<vector<string>> POSTvars);
#endif

#ifndef _NULL_SVR_MP_
#define _NULL_SVR_MP_
#define _CRT_SECURE_NO_WARNINGS
#include <afxsock.h>
#include <vector>
#include <iostream>
#include "nullSERVER.h"
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#define CHUNK_SIZE 4096
#define TIME_STAMP "%a, %d %b %G %T GMT+7"
#define GET_TIME_STAMP "%a, %d %b %Y %T GMT+7"
#define CSS_EXT ".css"
#define JS_EXT ".js"
#define HTML_EXT ".html"
#define ICO_EXT ".ico"
#define JPEG_EXT ".jpeg"
#define JPG_EXT ".jpg"
#define JPE_EXT ".jpe"
#define PNG_EXT ".png"
#define TWOWEEK 1209600
#define GMT_7 25200
#define ONEDAY_SEC 86400
using namespace std;
using namespace std::chrono_literals;
string makeOKResponseHeader(int length, string type, std::filesystem::path path, bool nowTime = 0);
string getBinaryFileContent(string path);
string uriCutter(vector<char> buffer);
string getMIMEType(string uri);
string toLowercase(string in);
string getRequestMethod(vector<char> buffer);
bool isDownload(vector<vector<string>> queryVars);
string decToHex(size_t n);
vector<vector<string>> queryStringParser(string& uri);
string makeChunkedOKResponseHeader(string type, std::filesystem::path path);
string make404ResponseHeader(int length);
string make304ResponseHeader(std::filesystem::path path);
string make303ResponseHeader(string dest, string cookie);
string setCookieString(vector<vector<string>> POSTvars, bool isSession);
vector<vector<string>>getPOSTVar(vector<char> buffer);
vector<string> querySplitter(string query);
tm getFileModifiedDate(std::filesystem::path path);
template <typename TP>
std::time_t to_time_t(TP tp);
string makeDate(int secFromNow = 0);
bool modifiedFile(string uri, string cacheTime);
void cleanBuffer(vector<char>& buffer);
string getHeaderValue(vector<char> buffer, string header);
vector<vector<string>> cookieSplitter(string buffer);
bool cookieHashCheck(vector<vector<string>> cookie);
#endif

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
#define CSS_EXT ".css"
#define JS_EXT ".js"
#define HTML_EXT ".html"
#define ICO_EXT ".ico"
#define JPEG_EXT ".jpeg"
#define JPG_EXT ".jpg"
#define JPE_EXT ".jpe"
#define PNG_EXT ".png"
using namespace std;
using namespace std::chrono_literals;
string makeOKResponseHeader(int length, string type, std::filesystem::path path);
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
vector<string> querySplitter(string query);
string getCurrentDateString();
tm getFileModifiedDate(std::filesystem::path path);
template <typename TP>
std::time_t to_time_t(TP tp);
#endif

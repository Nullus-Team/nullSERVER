#include "pch.h"
#include "MessagesProcessing.h"
#include "Md5.h"
template <typename TP>
std::time_t to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
		+ system_clock::now());
	return system_clock::to_time_t(sctp);
}
tm getFileModifiedDate(std::filesystem::path path)
{
	auto ftime = std::filesystem::last_write_time(path);
	time_t modifiedDate = to_time_t(ftime);
	tm tmDate;
	localtime_s(&tmDate, &modifiedDate);
	return tmDate;
}
string makeOKResponseHeader(int length, string type, std::filesystem::path path, bool nowTime)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 200 OK\r\nDate: ";
	tm modifiedTime;
	if (nowTime == 0)
	{
		modifiedTime = getFileModifiedDate(path);
	}
	else
	{
		time_t now;
		time(&now);
		localtime_s(&modifiedTime, &now);
	}
	strftime(temp, 50, TIME_STAMP, &modifiedTime);
	result = result + makeDate() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp + "\r\nContent-Length: " + to_string(length) + "\r\nExpires: " + makeDate(TWOWEEK) +
		"\r\nContent-Type: "+ type +"; charset=utf-8\r\nConnection: Keep-Alive\r\n\r\n";
	return result;
}
string make303ResponseHeader(string dest, string cookie) {
	string result = "HTTP/1.1 303 See Other\r\n" + cookie + "Location: " + dest;
	return result;
}
string makeChunkedOKResponseHeader(string type, std::filesystem::path path)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 200 OK\r\nDate: ";
	tm modifiedTime = getFileModifiedDate(path);
	strftime(temp, 50, TIME_STAMP, &modifiedTime);
	result = result + makeDate() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp + "\r\nTransfer-Encoding: chunked\r\nContent-Disposition: attachment" +
		"\r\nContent-Type: " + type + "\r\nConnection: Keep-Alive\r\n\r\n";
	return result;
}
string makeDate(int secFromNow)
{
	time_t t; //Thoi gian hien tai
	time(&t);
	t += secFromNow;
	tm curTime;
	localtime_s(&curTime, &t); //gio VN
	char expDate[50];
	strftime(expDate, 50, TIME_STAMP, &curTime);
	return expDate;
}
string setCookieString(vector<vector<string>> POSTvars, bool isSession)
{
	string id, pwd;
	for (vector<vector<string>>::iterator i = POSTvars.begin(); i < POSTvars.end(); i++)
	{
		if ((*i)[0] == "username") id = (*i)[1];
		if ((*i)[0] == "password") pwd = (*i)[1];
	}
	string expDate = makeDate(TWOWEEK); // tm to string
	string result;
	result += "Set-Cookie: id=" + id + "; Path=/";
	if (!isSession)
	{
		result += "; Expires=";
		result += expDate;
	}
	result += "; HttpOnly\r\n";
	result += "Set-Cookie: hash=" + md5(pwd) + "; Path=/";
	if (!isSession)
	{
		result += "; Expires=";
		result += expDate;
	}
	result += "; HttpOnly\r\n";
	return result;
}
string make304ResponseHeader(std::filesystem::path path)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 304 Not Modified\r\nDate: ";
	tm modifiedTime = getFileModifiedDate(path);
	strftime(temp, 50, TIME_STAMP, &modifiedTime);
	result = result + makeDate() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp +
		"\r\nConnection: Keep-Alive\r\n\r\n";
	return result;
}
vector<string> querySplitter(string query)
{
	vector<string> result;
	string temp = "";
	int i = 0;
	for (i; i < query.length(); i++)
	{
		if (query[i] == '=')
		{
			i++;
			break;
		}
		temp += query[i];
	}
	result.push_back(temp);
	temp = "";
	for (i; i < query.length(); i++)
	{
		temp += query[i];
	}
	result.push_back(temp);
	return result;
}
vector<vector<string>> queryStringParser(string& uri)
{
	vector<vector<string>> GETvars;
	string temp = "";
	size_t breakpoint = 0;
	for (int i = uri.length() - 1; i >= 0; i--)
	{
		if (uri[i] == '?' || uri[i] == '&')
		{
			GETvars.push_back(querySplitter(temp));
			temp = "";
			if (uri[i] == '?')
			{
				breakpoint = i;
				break;
			}
		}
		temp = uri[i] + temp;
	}
	if (breakpoint != 0) uri.erase(uri.begin() + breakpoint, uri.end());
	return GETvars;
}
vector<vector<string>>getPOSTVar(vector<char> buffer) {
	vector<vector<string>>GETacc;
	string buffer_str;
	for (vector<char>::iterator i = buffer.begin(); i < buffer.end(); i++) {
		buffer_str += (*i);
	}
	buffer_str.erase(0, buffer_str.find("\r\n\r\n") + 4);
	std::string delimiter = "&";
	size_t pos = 0;
	string token;
	while ((pos = buffer_str.find(delimiter)) != std::string::npos) {
		token = buffer_str.substr(0, pos);
		GETacc.push_back(querySplitter(token));
		buffer_str.erase(0, pos + delimiter.length());
	}
	GETacc.push_back(querySplitter(buffer_str));
	return GETacc;
}
string make404ResponseHeader(int length)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 404 Not Found\r\nDate: ";
	tm modifiedTime = getFileModifiedDate(PATH_404);
	strftime(temp, 50, TIME_STAMP, &modifiedTime);
	result = result + makeDate() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp + "\r\nContent-Length: " + to_string(length) +"\r\nExpires: " + 
		makeDate(TWOWEEK) + "\r\nContent-Type: text/html; charset=utf-8" + "\r\nConnection: Keep-Alive\r\n\r\n";
	return result;
}
string getBinaryFileContent(string path)
{
	ifstream f(path, ios::binary);
	if (f)
	{
		//f.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
		ostringstream ss;
		ss << f.rdbuf();
		f.close();
		return ss.str();
	}
	else return "\0";
}
string toLowercase(string in)
{
	for (int i = 0; i < in.length() - 1; i++)
		in[i] = tolower(in[i]);
	return in;
}
string getMIMEType(string uri)
{
	string destination = "";
	for (int i = uri.length() - 1; i > 0; i--)
	{
		destination = uri[i] + destination;
		if (uri[i] == '.' || uri[i] == '/' || uri[i]=='\\') break;
	}
	destination = toLowercase(destination);
	if (destination == PNG_EXT) return "image/png";
	if (destination == ICO_EXT) return "image/icon-x";
	if (destination == CSS_EXT) return "text/css";
	if (destination == HTML_EXT) return "text/html";
	if (destination == JS_EXT) return "text/javascript";
	if (destination == JPEG_EXT || destination == JPG_EXT || destination == JPE_EXT) return "image/jpeg";
	return "application/octet-stream";
}
string getRequestMethod(vector<char> buffer)
{
	string temp = "";
	for (vector<char>::iterator i = buffer.begin(); i < buffer.end(); i++)
	{
		if (*i == ' ') break;
		temp += *i;
	}
	return temp;
}
string uriCutter(vector<char> buffer)
{
	int counter = 0;
	string result = "";
	for (vector<char>::iterator i = buffer.begin(); i < buffer.end(); i++)
	{
		if (*i == ' ')
		{
			counter++;
			if (counter == 2) break;
			continue;
		}
		if (counter == 1)
		{
			result += *i;
		}
	}
	return result;
}
bool isDownload(vector<vector<string>> queryVars)
{
	for (vector<vector<string>>::iterator i = queryVars.begin(); i < queryVars.end(); i++)
	{
		if ((*i)[0] == "download" && (*i)[1] == "1") return 1;
	}
	return 0;
}
string decToHex(size_t n)
{
	string hex;
	size_t temp;
	while (n != 0)
	{
		temp = n % 16;
		if (temp < 10)
		{
			char t = temp + 48;
			hex = t + hex;
		}
		else
		{
			char t = temp + 55;
			hex = t + hex;
		}
		n = n / 16;
	}
	return hex;
}
string getHeaderValue(vector<char> buffer, string header)
{
	string result = "", sep = "\r\n";
	vector<char>::iterator found = search(buffer.begin(), buffer.end(), header.begin(), header.end(), [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
	if (found == buffer.end()) return "";
	vector<char>::iterator separator = search(found, buffer.end(), sep.begin(), sep.end());
	for (vector<char>::iterator i = found + header.length(); i < separator; i++)
	{
		result += *i;
	}
	return result;
}
bool cookieHashCheck(vector<vector<string>> cookie)
{
	bool a = false, b = false;
	for (vector<vector<string>>::iterator i = cookie.begin(); i < cookie.end(); i++)
	{
		if ((*i)[0] == USERNAME_COOKIE_FIELD_NAME && (*i)[1] == USERNAME) a = true;
		if ((*i)[0] == PASSWORD_COOKIE_FIELD_NAME) if ((*i)[1] == md5(PASSWORD)) b = true;
	}
	if (a && b) return 1;
	return 0;
}
vector<vector<string>> cookieSplitter(string buffer)
{
	vector<vector<string>> cookie;
	if (buffer == "") return cookie;
	std::string delimiter = "; ";
	size_t pos = 0;
	string token;
	while ((pos = buffer.find(delimiter)) != std::string::npos) {
		token = buffer.substr(0, pos);
		cookie.push_back(querySplitter(token));
		buffer.erase(0, pos + delimiter.length());
	}
	cookie.push_back(querySplitter(buffer));
	return cookie;
}
bool modifiedFile(string uri, string cacheTime)
{
	time_t cache, modified;
	tm tmCache;
	tm tmModified = getFileModifiedDate(uri);
	modified = mktime(&tmModified);
	std::istringstream ss(cacheTime);
	ss.imbue(std::locale("en_US.utf-8"));
	ss >> std::get_time(&tmCache, GET_TIME_STAMP);
	cache = mktime(&tmCache);
	if (difftime(modified, cache) > 0.0) return 1;
	return 0;
}
void cleanBuffer(vector<char>& buffer)
{
	int count = 0;
	for (vector<char>::iterator i = buffer.begin(); i < buffer.end(); i++)
	{
		if (*i == '\0') break;
		count++;
	}
	buffer.resize(count);
}
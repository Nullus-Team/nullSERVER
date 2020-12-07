#include "pch.h"
#include "MessagesProcessing.h"
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
string getCurrentDateString()
{
	time_t t;
	time(&t);
	//t += TIME_ZONE;
	tm curTime;
	localtime_s(&curTime, &t);
	char result[50];
	strftime(result, 50, TIME_STAMP, &curTime);
	return result;
}
string makeOKResponseHeader(int length, string type, std::filesystem::path path)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 200 OK\r\nDate: ";
	strftime(temp, 50, TIME_STAMP, &getFileModifiedDate(path));
	result = result + getCurrentDateString() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp + "\r\nContent-Length: " + to_string(length) +
		"\r\nContent-Type: "+ type +"; charset=utf-8\r\nConnection: Keep-Alive\r\n\r\n";
	return result;
}
string makeChunkedOKResponseHeader(string type, std::filesystem::path path)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 200 OK\r\nDate: ";
	strftime(temp, 50, TIME_STAMP, &getFileModifiedDate(path));
	result = result + getCurrentDateString() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp + "\r\nTransfer-Encoding: chunked\nContent-Disposition: attachment" +
		"\r\nContent-Type: " + type + "\r\nConnection: Keep-Alive\r\n\r\n";
	return result;
}
string make304ResponseHeader(std::filesystem::path path)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 304 Not Modified\r\nDate: ";
	strftime(temp, 50, TIME_STAMP, &getFileModifiedDate(path));
	result = result + getCurrentDateString() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp +
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
string make404ResponseHeader(int length)
{
	string result;
	char temp[50];
	result = "HTTP/1.1 404 Not Found\r\nDate: ";
	strftime(temp, 50, TIME_STAMP, &getFileModifiedDate(PATH_404));
	result = result + getCurrentDateString() + "\r\nServer: " + SERVER_INFO + "\r\nLast-Modified: " + temp + "\r\nContent-Length: " + to_string(length) +
		"\r\nContent-Type: text/html; charset=utf-8" + "\r\nConnection: Keep-Alive\r\n\r\n";
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

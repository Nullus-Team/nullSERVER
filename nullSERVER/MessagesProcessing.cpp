#include "pch.h"
#include "MessagesProcessing.h"
string fixedTime(string t)
{
	if (t.length() == 1) t = "0" + t;
	return t;
}
string makeDate()
{
	string temp;
	time_t t;
	time(&t);
	t += GMT_7;
	tm curTime;
	gmtime_s(&curTime, &t);
	temp = "";
	//Add weekday
	switch (curTime.tm_wday)
	{
	case 0:
		temp += "Sun, ";
		break;
	case 1:
		temp += "Mon, ";
		break;
	case 2:
		temp += "Tue, ";
		break;
	case 3:
		temp += "Wed, ";
		break;
	case 4:
		temp += "Thu, ";
		break;
	case 5:
		temp += "Fri, ";
		break;
	case 6:
		temp += "Sat, ";
		break;
	}
	//Add day
	temp += to_string(curTime.tm_mday);
	//Add month
	switch (curTime.tm_mon)
	{
	case 0:
		temp += " Jan ";
		break;
	case 1:
		temp += " Feb ";
		break;
	case 2:
		temp += " Mar ";
		break;
	case 3:
		temp += " Apr ";
		break;
	case 4:
		temp += " May ";
		break;
	case 5:
		temp += " Jun ";
		break;
	case 6:
		temp += " Jul ";
		break;
	case 7:
		temp += " Aug ";
		break;
	case 8:
		temp += " Sep ";
		break;
	case 9:
		temp += " Oct ";
		break;
	case 10:
		temp += " Nov ";
		break;
	case 11:
		temp += " Dec ";
		break;
	}
	//Add year and time
	temp = temp + to_string(curTime.tm_year + 1900) + " " + fixedTime(to_string(curTime.tm_hour)) + ":" + fixedTime(to_string(curTime.tm_min)) + ":" +
		fixedTime(to_string(curTime.tm_sec)) + " GMT+7\n";
	return temp;
}
string makeOKResponseHeader(int length, string type)
{
	string result, temp;
	result = "HTTP/1.1 200 OK\nDate: ";
	temp = makeDate();
	result = result + temp + "Server: nullSERVER/0.0.1 (Win64)\nLast-Modified: " + temp + "Content-Length: " + to_string(length) +
		"\nContent-Type: "+ type +"\nConnection: Keep-Alive\n\n";
	return result;
}
string makeChunkedOKResponseHeader(string type)
{
	string result, temp;
	result = "HTTP/1.1 200 OK\nDate: ";
	temp = makeDate();
	result = result + temp + "Server: nullSERVER/0.0.1 (Win64)\nLast-Modified: " + temp + "Transfer-Encoding: chunked\nContent-Disposition: attachment" +
		"\nContent-Type: " + type + "\nConnection: Keep-Alive\n\n";
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
	string result, temp;
	result = "HTTP/1.1 404 Not Found\nDate: ";
	temp = makeDate();
	result = result + temp + "Server: nullSERVER/0.0.1 (Win64)\nLast-Modified: " + temp + "Content-Length: " + to_string(length) +
		"\nContent-Type: text/html" + "\nConnection: Keep-Alive\n\n";
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
	return "folder";
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
DWORD WINAPI accessProcessing(LPVOID lpParam)
{
	CSocket client;
	client.Attach(*(SOCKET*)lpParam);
	vector<char>buffer(MAX_BUFFER_LENGTH);
	client.Receive(&buffer[0], MAX_BUFFER_LENGTH, 0);
	//Requests from client
	//URI cutter
	string uri = uriCutter(buffer);
	uri.erase(uri.begin()); // delete '/' at first place
	uri = WEB_SOURCE_PATH + uri;
	//Routing
	//HTML indexes
	vector<vector<string>> queryVars = queryStringParser(uri);
	string rtype = getMIMEType(uri);
	string rmethod = getRequestMethod(buffer);
	if (rmethod == "GET")
	{
		cout << rtype << " " << uri << "\n";
		if (isDownload(queryVars))
		{
			string separator = "\r\n", sizeSeparator;
			string header = makeChunkedOKResponseHeader(rtype);
			client.Send(header.c_str(), header.length(), 0); //send header
			ifstream fi(uri, ios::binary);
			fi.seekg(0, fi.end);
			unsigned long long int fileSize = fi.tellg(); //get File Size
			fi.seekg(0, fi.beg);
			vector<char> content(CHUNK_SIZE);
			for (int i = 0; i < fileSize / CHUNK_SIZE; i++)
			{
				if (!fi.read(content.data(), CHUNK_SIZE))
				{
					cout << "Can't read file!\n";
					break;
				}
				cout << content.size() << endl;
				sizeSeparator = decToHex(content.size()) + "\r\n";
				client.Send(sizeSeparator.c_str(), sizeSeparator.length(), 0); //send chunk size
				client.Send(&content[0], content.size(), 0); //send chunk content
				client.Send(separator.c_str(), separator.length(), 0); //send end chunk
			}
			if (fileSize % CHUNK_SIZE != 0)
			{
				content.resize(fileSize % CHUNK_SIZE); //resize vector to fit remaining part
				if (!fi.read(content.data(), fileSize%CHUNK_SIZE))
				{
					cout << "Can't read file!\n";
					return 0;
				}
				cout << content.size() <<"cc"<< endl;
				sizeSeparator = decToHex(content.size()) + "\r\n";
				client.Send(sizeSeparator.c_str(), sizeSeparator.length(), 0); //send chunk size
				client.Send(&content[0], content.size(), 0); //send chunk content
				client.Send(separator.c_str(), separator.length(), 0); //send end chunk
			}
			separator = "0\r\n\r\n";
			client.Send(separator.c_str(), separator.length(), 0);
		}
		else if (rtype != "folder") //HTML/CSS/JS //images
		{
			string content = getBinaryFileContent(uri);
			if (content == "\0")
			{
				content = getBinaryFileContent(PATH_404);
				string header = make404ResponseHeader(content.length());
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
			else
			{
				string header = makeOKResponseHeader(content.length(), rtype);
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
		}
		else if (rtype == "folder")
		{
			uri = uri + "/index.html";
			string content = getBinaryFileContent(uri);
			if (content == "\0")
			{
				content = getBinaryFileContent(PATH_404);
				string header = make404ResponseHeader(content.length());
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
			else
			{
				string header = makeOKResponseHeader(content.length(), "text/html");
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
		}
	}
	return 0; 
}
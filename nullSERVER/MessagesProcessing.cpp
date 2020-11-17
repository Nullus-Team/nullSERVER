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
string makeTextResponse(int length, string type)
{
	string result, temp;
	result = "HTTP/1.1 200 OK\nDate: ";
	temp = makeDate();
	result = result + temp + "Server: nullSERVER/0.0.1 (Win64)\nLast-Modified: " + temp + "Content-Length: " + to_string(length) +
		"\nContent-Type: text/"+ type +"\nConnection: Closed\n\n";
	return result;
}
string makeImageResponse(int length, string type)
{
	string result, temp;
	result = "HTTP/1.1 200 OK\nDate: ";
	temp = makeDate();
	result = result + temp + "Server: nullSERVER/0.0.1 (Win64)\nLast-Modified: " + temp + "Content-Length: " + to_string(length) +
		"\nContent-Type: image/" + type + "\nConnection: Closed\n\n";
	return result;
}
string makeFileResponse(string type)
{
	string result, temp;
	result = "HTTP/1.1 200 OK\nDate: ";
	temp = makeDate();
	result = result + temp + "Server: nullSERVER/0.0.1 (Win64)\nLast-Modified: " + temp + "Transfer-Encoding: chunked" +
		"\nContent-Type: application/" + type + "\nConnection: Closed\n\n";
	return result;
}
string getTextFileContent(string path)
{
	ifstream f(path);
	if (f)
	{
		//f.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
		ostringstream ss;
		ss << f.rdbuf();
		return ss.str();
	}
	else return "\0";
}
string getBinaryFileContent(string path)
{
	ifstream f(path, ios::binary);
	if (f)
	{
		//f.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
		ostringstream ss;
		ss << f.rdbuf();
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
string getRequestType(string uri)
{
	string destination = "";
	for (int i = uri.length() - 1; i > 0; i--)
	{
		destination = uri[i] + destination;
		if (uri[i] == '.' || uri[i] == '/' || uri[i]=='\\') break;
	}
	destination = toLowercase(destination);
	if (destination == PNG_EXT) return "png";
	if (destination == ICO_EXT) return "icon-x";
	if (destination == CSS_EXT) return "css";
	if (destination == HTML_EXT) return "html";
	if (destination == JS_EXT) return "javascript";
	if (destination == JPEG_EXT || destination == JPG_EXT || destination == JPE_EXT) return "jpeg";
	return "folder";
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

DWORD WINAPI accessProcessing(LPVOID lpParam)
{
	CSocket client;
	client.Attach(*(SOCKET*)lpParam);
	vector<char>buffer(MAX_BUFFER_LENGTH);
	client.Receive(&buffer[0], MAX_BUFFER_LENGTH, 0);
	//Requests from client
	//for (vector<char>::iterator i = buffer.begin(); i < buffer.end(); i++) cout << *i;
	//cout << "\n";
	//URI cutter
	string uri = uriCutter(buffer);
	uri.erase(uri.begin()); // delete '/' at first place
	uri = WEB_SOURCE_PATH + uri;
	//Routing
	//HTML indexes
	string rtype = getRequestType(uri);
	cout << rtype << " " << uri << "\n";
	if (rtype == "css" || rtype == "html" || rtype == "javascript") //HTML/CSS/JS
	{
		string content = getTextFileContent(uri);
		if (content == "\0")
		{
			// redirect to 404
		}
		else
		{
			string header = makeTextResponse(content.length(), rtype);
			client.Send(header.c_str(), header.length(), 0);
			client.Send(content.c_str(), content.length(), 0);
		}
	}
	else if (rtype == "jpeg" || rtype == "png" || rtype == "icon-x") //Image
	{
		string content = getBinaryFileContent(uri);
		if (content == "\0")
		{
			// redirect to 404
		}
		else
		{
			string header = makeImageResponse(content.length(), rtype);
			client.Send(header.c_str(), header.length(), 0);
			client.Send(content.c_str(), content.length(), 0);
		}
	}
	else if (rtype == "folder")
	{
		uri = uri + "index.html";
		string content = getTextFileContent(uri);
		if (content == "\0")
		{
			// redirect to 404
		}
		else
		{
			string header = makeTextResponse(content.length(), "html");
			client.Send(header.c_str(), header.length(), 0);
			client.Send(content.c_str(), content.length(), 0);
		}
	} 
	return 0; 
}
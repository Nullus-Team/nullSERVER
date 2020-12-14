#include "pch.h"
#include "Routing.h"
#include "Md5.h"
vector<string> getDownloadable()
{
	vector<string> result;
	if (!filesystem::exists(DOWNLOADABLE_FILES_PATH)) std::filesystem::create_directory(DOWNLOADABLE_FILES_PATH);
	std::string path = DOWNLOADABLE_FILES_PATH;
	for (const auto& entry : std::filesystem::directory_iterator(path))
		result.push_back(entry.path().u8string());
	return result;
}
string extCutter(string path)
{
	string temp = "";
	for (int i = path.length() - 1; i >= 0; i--)
	{
		if (path[i] == '.' || path[i] == '/' || path[i] == '\\') break;
		temp = path[i] + temp;
	}
	return temp;
}
string filenameCutter(string path)
{
	string temp = "";
	for (int i = path.length() - 1; i >= 0; i--)
	{
		if (path[i] == '\\\\' || path[i] == '/' || path[i] == '\\') break;
		temp = path[i] + temp;
	}
	return temp;
}
string dToString(double size)
{
	stringstream stream;
	stream << fixed << setprecision(2) << size;
	return stream.str();
}
string filesizeDecor(uintmax_t oSize)
{
	string unit = "";
	double size = oSize;
	if (oSize / 1099511627776 != 0)
	{
		unit = "TB";
		size /= 1099511627776;
	}
	else if (oSize / 1073741824 != 0) 
	{
		unit = "GB";
		size /= 1073741824;
	}
	else if (oSize / 1048576 != 0) 
	{
		unit = "MB";
		size /= 1048576;
	}
	else if (oSize / 1024 != 0) 
	{
		unit = "KB";
		size /= 1024;
	}
	else unit = "bytes";
	return dToString(size) + unit;
}
void addFiles(string& content)
{
	string code = "\t\t\t<//nullSERVER?FILES?//>";
	size_t marked = content.find(code); //marked code for adding files into
	vector<string> files = getDownloadable();
	string first_part = content.substr(0, marked);
	if (files.empty()) first_part += "<h1 style=\"text-align: center; color: #5153ff;\">Empty!</h1>\n";
	content.erase(0, marked + code.length());
	for (vector<string>::iterator i = files.begin(); i < files.end(); i++)
	{
		string imgPath = extCutter(*i);
		if (!filesystem::exists(ICON_PATH + imgPath + ".png")) imgPath = "unknown.png"; else imgPath += ".png";
		string path = (*i).substr(strlen(WEB_SOURCE_PATH));
		string newFile = "\t\t\t<div class=\"content-box\">\n\t\t\t\t<img class=\"file-icon\" src=\"/img/icon/" + imgPath +
			"\">\n\t\t\t\t<div class=\"file-details\">\n\t\t\t\t\t<a href=\"" + path + "?download=1\">" + filenameCutter(*i) + "</a>\n\t\t\t\t\t<h2>Size: " + 
			filesizeDecor(std::filesystem::file_size(*i)) + "</h2>\n\t\t\t\t</div>\n\t\t\t</div>\n";
		first_part += newFile;
	}
	content = first_part + content;
}
void uriDecor(string& uri)
{
	//turns %20 into [SPACE]
	size_t found = uri.find("%20");
	while (found != string::npos)
	{
		uri.replace(found, 3, " ");
		found = uri.find("%20");
	}
}
bool isSession(vector<vector<string>> POSTvars)
{
	for (vector<vector<string>>::iterator i = POSTvars.begin(); i < POSTvars.end(); i++)
	{
		if ((*i)[0] == "remember" && (*i)[1] == "remember") return 0;
	}
	return 1;
}
bool accountChecker(vector<vector<string>> POSTvars)
{
	bool a = false, b = false;
	for (vector<vector<string>>::iterator i = POSTvars.begin(); i < POSTvars.end(); i++)
	{
		if ((*i)[0] == USERNAME_FIELD_NAME && (*i)[1] == USERNAME) a = true;
		if ((*i)[0] == PASSWORD_FIELD_NAME && (*i)[1] == PASSWORD) b = true;
	}
	if (a && b) return 1;
	return 0;
}
string removeCookie()
{
	string expDate = makeDate(-ONEDAY_SEC);
	string result;
	result += "Set-Cookie: id=null; Path=/";
	result += "; Expires=";
	result += expDate;
	result += "; HttpOnly\r\n";
	result += "Set-Cookie: hash=null; Path=/";
	result += "; Expires=";
	result += expDate;
	result += "; HttpOnly\r\n";
	return result;
}
DWORD WINAPI accessProcessing(LPVOID lpParam)
{
	CSocket client;
	client.Attach(*(SOCKET*)lpParam);
	vector<char>buffer(MAX_BUFFER_LENGTH);
	client.Receive(&buffer[0], MAX_BUFFER_LENGTH, 0);
	cleanBuffer(buffer);
	string rmethod = getRequestMethod(buffer);
	//Requests from client
	//URI cutter
	string uri = uriCutter(buffer);
	uriDecor(uri);
	uri.erase(uri.begin()); // delete '/' at first place
	uri = WEB_SOURCE_PATH + uri;
	//Routing
	//HTML indexes
	vector<vector<string>> queryVars = queryStringParser(uri);
	string rtype = getMIMEType(uri);
	string cached = getHeaderValue(buffer, IF_MODIFIED_HEADER);
	if (rmethod == "GET")
	{
		if (uri == SIGNOUT)
		{
			string header = make303ResponseHeader(LOGIN_PATH_PUBLIC, removeCookie());
			client.Send(header.c_str(), header.length(), 0);
			return 0;
		} 
		else
		if (uri == FILE_MANAGER_PATH)
		{
			if (!cookieHashCheck(cookieSplitter(getHeaderValue(buffer, COOKIE_HEADER))))
			{
				string header = make303ResponseHeader(LOGIN_PATH_PUBLIC, "");
				client.Send(header.c_str(), header.length(), 0);
				return 0;
			}
			string content = getBinaryFileContent(uri);
			if (content == "\0")
			{
				content = getBinaryFileContent(PATH_404);
				string header = makeOKResponseHeader(content.length(), HTML_MIME, uri);
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
			else
			{
				addFiles(content);
				//no cache because this content is automatically made at the time requests come.
				string header = makeOKResponseHeader(content.length(), rtype, uri, 1);
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
		}
		else
		if (isDownload(queryVars))
		{
			if (!cookieHashCheck(cookieSplitter(getHeaderValue(buffer, COOKIE_HEADER))))
			{
				string header = make303ResponseHeader(LOGIN_PATH_PUBLIC, "");
				client.Send(header.c_str(), header.length(), 0);
				return 0;
			}
			string separator = "\r\n", sizeSeparator;
			string header = makeChunkedOKResponseHeader(rtype, uri);
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
				sizeSeparator = decToHex(content.size()) + "\r\n";
				client.Send(sizeSeparator.c_str(), sizeSeparator.length(), 0); //send chunk size
				client.Send(&content[0], content.size(), 0); //send chunk content
				client.Send(separator.c_str(), separator.length(), 0); //send end chunk
			}
			if (fileSize % CHUNK_SIZE != 0)
			{
				content.resize(fileSize % CHUNK_SIZE); //resize vector to fit remaining part
				if (!fi.read(content.data(), fileSize % CHUNK_SIZE))
				{
					cout << "Can't read file!\n";
					return 0;
				}
				sizeSeparator = decToHex(content.size()) + "\r\n";
				client.Send(sizeSeparator.c_str(), sizeSeparator.length(), 0); //send chunk size
				client.Send(&content[0], content.size(), 0); //send chunk content
				client.Send(separator.c_str(), separator.length(), 0); //send end chunk
			}
			separator = "0\r\n\r\n";
			client.Send(separator.c_str(), separator.length(), 0);
			fi.close();
		}
		else //HTML/CSS/JS //images
		{
			if (uri == LOGIN_PATH || uri == WEB_SOURCE_PATH)
			{
				if (cookieHashCheck(cookieSplitter(getHeaderValue(buffer, COOKIE_HEADER))))
				{
					string header = make303ResponseHeader(INFO_PATH_PUBLIC, "");
					client.Send(header.c_str(), header.length(), 0);
					return 0;
				}
			}
			if (uri == INFO_PATH)
			{
				if (!cookieHashCheck(cookieSplitter(getHeaderValue(buffer, COOKIE_HEADER))))
				{
					string header = make303ResponseHeader(LOGIN_PATH_PUBLIC, "");
					client.Send(header.c_str(), header.length(), 0);
					return 0;
				}
			}
			string content = getBinaryFileContent(uri);
			if (content == "\0")
			{
				//try to parse like folder
				uri = uri + "/index.html";
				content = getBinaryFileContent(uri);
				if (content == "\0")
				{
					content = getBinaryFileContent(PATH_404);
					string header = make404ResponseHeader(content.length());
					client.Send(header.c_str(), header.length(), 0);
					client.Send(content.c_str(), content.length(), 0);
				}
				else //if it isn't a folder then return 404
				{
					if (cached != "" && !modifiedFile(uri, cached))
					{
						string header = make304ResponseHeader(uri);
						client.Send(header.c_str(), header.length(), 0);
					}
					else
					{
						string header = makeOKResponseHeader(content.length(), HTML_MIME, uri);
						client.Send(header.c_str(), header.length(), 0);
						client.Send(content.c_str(), content.length(), 0);
					}
				}
			}
			else
			{
				if (cached != "" && !modifiedFile(uri, cached))
				{
					cout << cached << '\n';
					string header = make304ResponseHeader(uri);
					client.Send(header.c_str(), header.length(), 0);
				}
				else
				{
					string header = makeOKResponseHeader(content.length(), rtype, uri);
					client.Send(header.c_str(), header.length(), 0);
					client.Send(content.c_str(), content.length(), 0);
				}
			}

		}
	}
	if (rmethod == "POST") 
	{
		vector<vector<string>>pvars;
		string content = getBinaryFileContent(uri);
		//get account
		pvars = getPOSTVar(buffer);
		//check account -> redirect
		if (accountChecker(pvars)) { 
			string header = make303ResponseHeader(INFO_PATH_PUBLIC, setCookieString(pvars, isSession(pvars)));
			client.Send(header.c_str(), header.length(), 0);
		}
		else {
			string header = make303ResponseHeader(PATH_404_PUBLIC, "");
			client.Send(header.c_str(), header.length(), 0);
		}
	}
	return 0;
}
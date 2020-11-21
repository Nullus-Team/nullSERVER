#include "pch.h"
#include "Routing.h"
vector<string> getDownloadable()
{
	vector<string> result;
	std::string path = "web-source/files/";
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
	content.erase(0, marked + code.length());
	for (vector<string>::iterator i = files.begin(); i < files.end(); i++)
	{
		string path = (*i).substr(strlen(WEB_SOURCE_PATH));
		string newFile = "\t\t\t<div class=\"content-box\">\n\t\t\t\t<img class=\"file-icon\" src=\"/img/icon/" + extCutter(*i) +
			".png\">\n\t\t\t\t<div class=\"file-details\">\n\t\t\t\t\t<a href=\"" + path + "\">" + filenameCutter(*i) + "</a>\n\t\t\t\t\t<h2>Size: " + filesizeDecor(std::filesystem::file_size(*i)) + "</h2>\n\t\t\t\t</div>\n\t\t\t</div>\n";
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
DWORD WINAPI accessProcessing(LPVOID lpParam)
{
	CSocket client;
	client.Attach(*(SOCKET*)lpParam);
	vector<char>buffer(MAX_BUFFER_LENGTH);
	client.Receive(&buffer[0], MAX_BUFFER_LENGTH, 0);
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
	string rmethod = getRequestMethod(buffer);
	if (rmethod == "GET")
	{
		//cout << rtype << " " << uri << "\n";
		if (uri == FILE_MANAGER_PATH)
		{
			string content = getBinaryFileContent(uri);
			if (content == "\0")
			{
				content = getBinaryFileContent(PATH_404);
				string header = makeOKResponseHeader(content.length(), "text/html");
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
			else
			{
				addFiles(content);
				string header = makeOKResponseHeader(content.length(), rtype);
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
		}
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
					string header = makeOKResponseHeader(content.length(), "text/html");
					client.Send(header.c_str(), header.length(), 0);
					client.Send(content.c_str(), content.length(), 0);
				}
			}
			else
			{
				string header = makeOKResponseHeader(content.length(), rtype);
				client.Send(header.c_str(), header.length(), 0);
				client.Send(content.c_str(), content.length(), 0);
			}
		}
	}
	return 0;
}
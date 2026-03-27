#pragma once
#include <string>
class ArgumentsSettings {
public:
	ArgumentsSettings(int numArguments, char** arguments);
	std::string getExternalPath();
	std::string getLevelName();
	std::string getServerKey();
	std::string getCachePath();
	std::string getLevelDir();
	bool getShowHelp();
	int getPort();
private:
	std::string cachePath;
	std::string externalPath;
	std::string levelName;
	std::string levelDir;
	std::string serverKey;
	bool showHelp;
	int port;
};


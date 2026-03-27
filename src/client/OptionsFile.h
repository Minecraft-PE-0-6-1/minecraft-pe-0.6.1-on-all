#pragma once

//package net.minecraft.client;
#include <string>
#include <vector>
typedef std::vector<std::string> StringVector;
class OptionsFile
{
public:
	OptionsFile();
    void save(const StringVector& settings);
	StringVector getOptionStrings();
	void setOptionsPath(const std::string& path);
	std::string getOptionsPath() const;

private:
	std::string settingsPath;
};


#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <climits>

class Utils
{
private:
	static void split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
			if (item != "") elems.push_back(item);
	}

public:
	template<typename T>
	static std::string ToString(T arg)
	{
		std::stringstream ss;
		ss << arg;
		return ss.str();
	}
	
	static std::string ReadLine()
	{
		std::string res;
		std::getline(std::cin, res);
		return res;
	}

	static std::string ReadTextFromFile(const char* file_name)
	{
		std::ifstream f(file_name);
		if (!f) return "TFDNE!"; // The File Does Not Exist! 
		std::stringstream buffer;
		buffer << f.rdbuf();
		return buffer.str();
	}

	static inline std::vector<std::string> Split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	// Converts string to a number. Return INT_MIN, if conversion was failed
	template<typename T>
	static T StringToNum(std::string str)
	{
		std::stringstream stream;
   		stream << str;
   		T res;
   		if (stream >> res) return res;
   		else return INT_MIN;
	}

	static std::string Trim(std::string& str, char ch1 = ' ', char ch2 = '\t', char ch3 = ' ')
	{
		int len = str.length();
		int start = 0, end = len - 1;
		for (int i = 0; i < len; ++i)
		{
			if (str[i] != ch1 && str[i] != ch2 && str[i] != ch3)
				break;
			start++;
		}
		for (int i = len - 1; i >= 0; --i)
		{
			if (str[i] != ch1 && str[i] != ch2 && str[i] != ch3)
				break;
			end--;
		}
		return str.substr(start, end - start + 1);
	}

	static inline bool StartsWith(std::string s1, std::string s2)
	{
		return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
	}
};

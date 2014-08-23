#pragma once

#include <string>

namespace Cell
{
	class CellToken
	{
	public:
		std::string Command;
		std::string Arg;

		CellToken() { }
		CellToken(std::string com, std::string arg) :
			Command(com), Arg(arg)
		{ }
	};
};
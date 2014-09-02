#pragma once 

#include <string>

namespace Cell
{
	class CellFunction
	{
	public:
		std::string Name;
		int StartLine, EndLine;


		CellFunction() { }
		CellFunction(std::string name, int sline, int eline) :
			Name(name), StartLine(sline), EndLine(eline)
		{ }
	};
};
#pragma once

#include <string>
#include <vector>

#include "cell_token.h"

namespace Cell
{
	class CellLang
	{
	public:
		void Interpret(std::vector<std::string> code_lines);
		static void WriteError(std::string description, int line);

	private:
		void RefactorCode(std::vector<std::string>& code_lines);
		void Link(std::vector<std::string>& toks);
		std::vector<CellToken> GetTokens(std::vector<std::string> code_lines);
		void ProcessCommands(std::vector<Cell::CellToken> toks, std::string& memory, std::string& acc,
			std::vector<std::string> cells, int& ccell);
	};
};

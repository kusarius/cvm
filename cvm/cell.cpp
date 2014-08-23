#include "cell.h"
#include "utils.h"
#include <iostream>

#define REMOVE_BRACKETS(str) Utils::Trim(str, '\"', '\"', '\"')

// Убирает из кода комментарии и пустые строки
void Cell::CellLang::RefactorCode(std::vector<std::string>& code_lines)
{
	int lcount = code_lines.size();
	for (int i = 0; i < lcount; ++i)
	{
		code_lines[i] = Utils::Trim(code_lines[i]);
		if (code_lines[i] == "" || Utils::StartsWith(code_lines[i], "#"))
		{
			code_lines.erase(code_lines.begin() + i);
			lcount--;
			i--;
		}
	}
}

// Выводит описание ошибки в консоль
void Cell::CellLang::WriteError(std::string com, std::string arg, std::string description, int comnumber)
{
	std::cout << std::endl << "Error in command " << comnumber << ": "
		<< com << " " << arg << std::endl
		<< description << std::endl;
}

// Обрабатывает команды import
void Cell::CellLang::Link(std::vector<CellToken>& toks)
{
	// ...
}

// Разделяет код на токены
std::vector<Cell::CellToken> Cell::CellLang::GetTokens(std::vector<std::string> code_lines)
{
	std::vector<Cell::CellToken> toks;
	int lcount = code_lines.size();
	for (int i = 0; i < lcount; ++i)
	{
		int wspos = code_lines[i].find(" "); // Позиция первого пробела в строке
		Cell::CellToken tok = Cell::CellToken();
		tok.Command = code_lines[i].substr(0, wspos);
		tok.Arg = code_lines[i].substr(wspos + 1, code_lines[i].size() - wspos - 1);
		toks.push_back(tok);
	}
	return toks;
}

void Cell::CellLang::ProcessCommands(std::vector<Cell::CellToken> toks)
{
	int toks_size = toks.size();
	for (int i = 0; i < toks_size; ++i)
		if (toks[i].Command == "print") {
			if (toks[i].Command == toks[i].Arg) WriteError(toks[i].Command, "", "Argument expected", i + 1);
			std::cout << REMOVE_BRACKETS(toks[i].Arg);
		}
		else if (toks[i].Command == "println") {
			if (toks[i].Command == toks[i].Arg) WriteError(toks[i].Command, "", "Argument expected", i + 1);
			std::cout << REMOVE_BRACKETS(toks[i].Arg) << std::endl;
		}

}

void Cell::CellLang::Interpret(std::vector<std::string> code_lines)
{
	std::vector<std::string> cells(10, "");
	std::string memory = "", acc = "";

	RefactorCode(code_lines);
	std::vector<Cell::CellToken> toks = GetTokens(code_lines);
	Link(toks);

	ProcessCommands(toks);
}
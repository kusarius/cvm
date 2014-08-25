#include "cell.h"
#include "utils.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>

#define REMOVE_BRACKETS(str) Utils::Trim(str, '\"', '\'', '\"')

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
void Cell::CellLang::WriteError(std::string description, int comnumber)
{
	std::cout << std::endl << "Error in command " << comnumber << ": "
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

void Cell::CellLang::ProcessCommands(std::vector<Cell::CellToken> toks, std::string& memory, std::string& acc,
	std::vector<std::string> cells, int& acell)
{
	int toks_size = toks.size();
	for (int i = 0; i < toks_size; ++i)
		if (toks[i].Command == "print" || toks[i].Command == "println") {
			std::string lend = toks[i].Command == "println" ? "\n" : "";
			if (toks[i].Arg == "data") std::cout << cells[acell] << lend;
			else if (toks[i].Arg == "memory") std::cout << memory << lend;
			else if (toks[i].Arg == "acc") std::cout << acc << lend;
			else {
				if (toks[i].Command == toks[i].Arg || toks[i].Arg.length() < 2)
					WriteError("Argument expected", i + 1);
				else std::cout << REMOVE_BRACKETS(toks[i].Arg) << lend;
			}
		}
		else if (toks[i].Command == "set") {
			if (toks[i].Command == toks[i].Arg) WriteError("Argument expected", i + 1);
			else {
				if (toks[i].Arg == "memory") cells[acell] = memory;
				else if (toks[i].Arg == "acc") cells[acell] = acc;
				else if (toks[i].Arg == "read") cells[acell] = Utils::ReadLine();
				else cells[acell] = REMOVE_BRACKETS(toks[i].Arg);
			}
		} 
		else if (toks[i].Command == "memory") {
			if (toks[i].Command == toks[i].Arg) WriteError("Argument expected", i + 1);
			else {
				if (toks[i].Arg == "data") memory = cells[acell];
				else if (toks[i].Arg == "acc") memory = acc;
				else if (toks[i].Arg == "read") memory = Utils::ReadLine();
				else memory = REMOVE_BRACKETS(toks[i].Arg);
			}
		}
		else if (toks[i].Command == "right" || toks[i].Command == "left") {
			int direction = toks[i].Command == "right" ? 1 : -1;
			if (toks[i].Arg == toks[i].Command) acell += direction;
			else {
				char* endptr;
				const char* ptr = toks[i].Arg.c_str();
				int val = strtol(ptr, &endptr, 10);
				if (ptr == endptr) WriteError("Invalid argument", i + 1);
				else acell += val * direction;
			}
			if (acell < 0) { WriteError("Number of active cell cannot be less than 0", i + 1); acell = 0; }
			if (acell >= (int)cells.size()) { 
				WriteError("Number of active cell cannot be greater than allowable; use \"cells\" command", i + 1);
				acell = cells.size() - 1; 
			}
		}
		else if (toks[i].Command == "add" || toks[i].Command == "sub" ||
				toks[i].Command == "mul" || toks[i].Command == "div" ||
				toks[i].Command == "sqrt" || toks[i].Command == "pow" ||
				toks[i].Command == "min" || toks[i].Command == "max")
		{
			float arg1, arg2; // Операнды
			arg1 = Utils::StringToNum<float>(cells[acell]);
			if (toks[i].Command != "sqrt") arg2 = Utils::StringToNum<float>(memory);
			if (arg1 == INT_MIN || arg2 == INT_MIN) { // Если недопустимые аргументы
				WriteError("Invalid arguments!", i + 1);
				continue;
			}
			std::string com = toks[i].Command;
			if (com == "add") acc = Utils::ToString<float>(arg1 + arg2);
			else if (com == "sub") acc = Utils::ToString<float>(arg1 - arg2);
			else if (com == "mul") acc = Utils::ToString<float>(arg1 * arg2);
			else if (com == "div") acc = Utils::ToString<float>(arg1 / arg2);
			else if (com == "sqrt") acc = Utils::ToString<float>(sqrtf(arg1));
			else if (com == "pow") acc = Utils::ToString<float>(powf(arg1, arg2));
			else if (com == "min") acc = Utils::ToString<float>(arg1 < arg2 ? arg1 : arg2);
			else if (com == "max") acc = Utils::ToString<float>(arg1 > arg2 ? arg1 : arg2);
		}
		else if (toks[i].Command == "if" || toks[i].Command == "ifn") {
			if (toks[i].Command == toks[i].Arg) WriteError("Argument expected", i + 1);
			else {
				std::string arg = REMOVE_BRACKETS(toks[i].Arg);
				bool val = toks[i].Command == "if" ? false : true;
				if ((cells[acell] == arg) == val) {
					bool found = false;
					for (int c = i + 1; c < toks_size; ++c)
						if (toks[c].Command == "endif") {
							i = c;
							found = true;
							break;
						}
					if (found == false) WriteError("\"endif\" expected", i + 1);
				}
			}
		}
		else if (toks[i].Command == "endif") { }
		else if (toks[i].Command == "exit") break;
		else WriteError("Unknown command", i + 1);
}

void Cell::CellLang::Interpret(std::vector<std::string> code_lines)
{
	std::vector<std::string> cells(10, "");
	std::string memory = "", acc = "";
	int ccell = 0; // Номер активной ячейки

	RefactorCode(code_lines);
	std::vector<Cell::CellToken> toks = GetTokens(code_lines);
	Link(toks);

	ProcessCommands(toks, memory, acc, cells, ccell);
}

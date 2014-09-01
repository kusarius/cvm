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
	this->isError = true;
}

// Обрабатывает команды import
void Cell::CellLang::Link(std::vector<std::string>& toks)
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
	std::string com = ""; // Команда в текущей строке
	for (int i = 0; i < toks_size; ++i) {
		if (isError) break;
		com = toks[i].Command;
		if (com == "print" || com == "println") {
			std::string lend = com == "println" ? "\n" : "";
			if (toks[i].Arg == "data") std::cout << cells[acell] << lend;
			else if (toks[i].Arg == "memory") std::cout << memory << lend;
			else if (toks[i].Arg == "acc") std::cout << acc << lend;
			else {
				if (com == toks[i].Arg || toks[i].Arg.length() < 2)
					WriteError("Argument expected", i + 1);
				else std::cout << REMOVE_BRACKETS(toks[i].Arg) << lend;
			}
		}
		else if (com == "set") {
			if (com == toks[i].Arg) WriteError("Argument expected", i + 1);
			else {
				if (toks[i].Arg == "memory") cells[acell] = memory;
				else if (toks[i].Arg == "acc") cells[acell] = acc;
				else if (toks[i].Arg == "read") cells[acell] = Utils::ReadLine();
				else cells[acell] = REMOVE_BRACKETS(toks[i].Arg);
			}
		} 
		else if (com == "memory") {
			if (com == toks[i].Arg) WriteError("Argument expected", i + 1);
			else {
				if (toks[i].Arg == "data") memory = cells[acell];
				else if (toks[i].Arg == "acc") memory = acc;
				else if (toks[i].Arg == "read") memory = Utils::ReadLine();
				else memory = REMOVE_BRACKETS(toks[i].Arg);
			}
		}
		else if (com == "right" || com == "left") {
			int direction = com == "right" ? 1 : -1;
			if (toks[i].Arg == com) acell += direction;
			else {
				int val = Utils::StringToNum<int>(toks[i].Arg);
				if (val == INT_MIN) WriteError("Invalid argument", i + 1);
				else acell += val * direction;
			}
			if (acell < 0) { WriteError("Number of active cell cannot be less than 0", i + 1); acell = 0; }
			if (acell >= (int)cells.size()) { 
				WriteError("Number of active cell cannot be greater than allowable; use \"cells\" command", i + 1);
				acell = cells.size() - 1; 
			}
		}
		else if (com == "add" || com == "sub" ||
				com == "mul" || com == "div" ||
				com == "sqrt" || com == "pow" ||
				com == "min" || com == "max")
		{
			float arg1, arg2; // Операнды
			arg1 = Utils::StringToNum<float>(cells[acell]);
			if (com != "sqrt") arg2 = Utils::StringToNum<float>(memory);
			if (arg1 == INT_MIN || arg2 == INT_MIN) { // Если недопустимые аргументы
				WriteError("Invalid arguments!", i + 1);
				continue;
			}
			std::string action = com;
			if (action == "add") acc = Utils::ToString<float>(arg1 + arg2);
			else if (action == "sub") acc = Utils::ToString<float>(arg1 - arg2);
			else if (action == "mul") acc = Utils::ToString<float>(arg1 * arg2);
			else if (action == "div") acc = Utils::ToString<float>(arg1 / arg2);
			else if (action == "sqrt") acc = Utils::ToString<float>(sqrtf(arg1));
			else if (action == "pow") acc = Utils::ToString<float>(powf(arg1, arg2));
			else if (action == "min") acc = Utils::ToString<float>(arg1 < arg2 ? arg1 : arg2);
			else if (action == "max") acc = Utils::ToString<float>(arg1 > arg2 ? arg1 : arg2);
		}
		else if (com == "if" || com == "ifnot") {
			if (com == toks[i].Arg) WriteError("Argument expected", i + 1);
			else {
				std::string arg = REMOVE_BRACKETS(toks[i].Arg);
				bool val = com == "if" ? false : true;
				if ((cells[acell] == arg) == val) {
					int nesting = 1; // Степень вложенности
					bool found = false;
					for (int c = i + 1; c < toks_size; ++c)
						if (toks[c].Command == "if" || toks[c].Command == "ifnot") nesting++;
						else if (toks[c].Command == "endif") {
							nesting--;
							if (nesting < 1) {
								i = c; // Переходим на строку кода с endif
								found = true;
								break;
							}
						}
					if (found == false) WriteError("\"endif\" expected", i + 1);
				}
			}
		}
		else if (com == "repeat") {
			bool found = false;
			int nesting = 1; // Степень вложенности цикла
			for (int c = i + 1; c < toks_size; ++c) {
				if (toks[c].Command == "repeat") nesting++;
				if (toks[c].Command == "endrepeat") {
					nesting--;
					if (nesting < 1) {
						std::vector<Cell::CellToken> subtoks(toks.begin() + i + 1, toks.begin() + c);	
						if (com == toks[i].Arg)
							while (true) ProcessCommands(subtoks, memory, acc, cells, acell);
						else {
							int rtimes = Utils::StringToNum<int>(toks[i].Arg);
							if (rtimes == INT_MIN) WriteError("Invalid argument", i + 1);
							else for (int h = 0; h < rtimes - 1; ++h) ProcessCommands(subtoks, memory, acc, cells, acell);
						}	
						found = true;	
						break;
					}
				}
			}
			if (found == false) WriteError("\"endrepeat\" expected", i + 1);
		}
		else if (com == "function") {
			if (com == toks[i].Arg) WriteError("Function name expected", i + 1);
			else {
				int funclist_elem = functions.size() - 1;
				bool found_same_func = false;
				for (; funclist_elem >= 0; --funclist_elem)
					if (functions[funclist_elem].Name == toks[i].Arg) {
						WriteError("The function called " + toks[i].Arg + " is already defined", i + 1);
						found_same_func = true;
						break;
					}
				if (found_same_func) break;
				
				int endfunc_line = i + 1;
				while (toks[endfunc_line].Arg != "endfunc" 
					&& endfunc_line < toks_size - 1) endfunc_line++;
				if (toks[endfunc_line].Arg != "endfunc") {
					WriteError("\"endfunc\" expected", i + 1); 
					break;
				}
				
				Cell::CellFunction function = Cell::CellFunction(toks[i].Arg, i, endfunc_line);
				functions.push_back(function);
				i = function.EndLine; // Меняем текущую строку
			}
		}
		else if (com == "call") {
			if (com == toks[i].Arg) WriteError("Function name expected", i + 1);
			else {
							
			}
		}
		else if (com == "endif" || com == "endrepeat"|| com == "endfunc") { }
		else if (com == "exit") break;
		else WriteError("Unknown command", i + 1);
	}
}

void Cell::CellLang::Interpret(std::vector<std::string> code_lines)
{
	std::vector<std::string> cells(10, "");
	std::string memory = "", acc = "";
	int ccell = 0; // Номер активной ячейки
	functions.clear();

	RefactorCode(code_lines);
	Link(code_lines);
	std::vector<Cell::CellToken> toks = GetTokens(code_lines);

	ProcessCommands(toks, memory, acc, cells, ccell);
}

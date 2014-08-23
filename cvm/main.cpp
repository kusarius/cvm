#include <iostream>
#include "cell.h"
#include "utils.h"
#include <ctime>

using namespace std;
using namespace Cell;

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << "No input files!" << endl;
		return 0;
	}

	string code = Utils::ReadTextFromFile(argv[1]);
	if (code == "TFDNE!")
	{
		cout << "The file does not exist!" << endl;
		return 0;
	}

	vector<string> code_lines = Utils::Split(code, '\n');
	CellLang* cell = new CellLang();
	cell->Interpret(code_lines);

	unsigned int work_time = clock();
	cout << endl << work_time << " milliseconds" << endl;
	cin.get();
}
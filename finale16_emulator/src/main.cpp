#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "bus.h"
using namespace std;
Finale16Bus soc;
vector<string> cmds;
string cmdline;
stringstream parsing_ss;
int main(int argc, char* argv[])
{
	std::cout << "Finale16 emulator 0.0.1" << endl;
	while (true)
	{
		std::cout << ">";
		if (!getline(cin, cmdline))
			break;
		parsing_ss.clear();
		parsing_ss.str(cmdline);
		string cmd;
		parsing_ss >> cmd;
		if (cmd == "loadbin") {
			string filename;
			parsing_ss >> filename;
			fstream fs;
			fs.open(filename, ios::binary | ios::in);
			if (!fs.is_open()) {
				std::cout << "failed to open file " << filename << endl;
				continue;
			}
			fs.seekg(0, fs.end);
			int length = fs.tellg();
			fs.seekg(0, fs.beg);
			if (length > 65536) {
				std::cout << "file size "<< length << " is larger than the address space (0x0000 - 0xffff)" << endl;
				continue;
			}
			std::cout << "file "<<filename <<" loaded, size = "<< length << endl;
			fs.read(soc.get_ram_as_read_buffer(), length);
		} else if (cmd == "run") {
			soc.reset();
			while(!soc.get_exit_flag())
				soc.clock();
		} else if (cmd == "r8") {
			uint16_t addr;
			int lines;
			parsing_ss >> hex >> addr >> dec >>lines;
			cout << "       00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" << endl;
			cout.fill('0');
			addr &= (addr & 0xfff0);
			for (int j = 0; j < lines; ++j) {
				cout << "0x";
				cout.width(4);
				cout << hex << addr << " ";
				for (int i = 0; i <= 15; ++i) {
					cout.width(2);
					cout << hex << (int)soc.read8(addr) << " ";
					addr++;
				}
				cout << endl;
			}
			cout << endl;
		} else if (cmd == "loadelf") {
			cout << "not support yet." << endl;
		} else if (cmd == "help") {
			cout << "loadbin filename  : load an image file to RAM." << endl;
			cout << "run               : reset pc to 0x0000 and start emulation." << endl;
			cout << "r8 startAddr lc   : print lc lines of RAM, starting at address = startAddr & 0xfff0" << endl;
		} else {
			cout << "unknown command, please type \"help\" for help." << endl;
		}
	}
	return 0;
}
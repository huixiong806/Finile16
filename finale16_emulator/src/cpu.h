#pragma once
#include <array>
class Finale16Bus;
class CPU
{
private:
	Finale16Bus* bus;
	uint16_t pc;
	std::array<uint16_t, 16> gpr;
	uint16_t instr;
	//decode
	int rs, rt, rd;
	int opcode;
	int16_t si8, si11;
	uint16_t ui4, ui8;
	void parse_rs();
	void parse_rt();
	void parse_rd();
	void parse_ui4();
	void parse_ui8();
	void parse_si8();
	void parse_si11();
	
	//run instruction
	void instr_run_2R();    
	void instr_run_RI();
	void instr_run_3R();
	void instr_run_I();
	void instr_run_Spe();
	void cpu_error();
public:
	CPU();
	~CPU();
	void clock();
	void reset();
	void connect_to(Finale16Bus* bus);
};
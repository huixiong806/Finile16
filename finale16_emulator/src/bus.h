#pragma once
#include <memory>
#include <array>
#include "cpu.h"
class Finale16Bus{
private:
	//cpu
	CPU mycpu;
	//ram
	uint8_t ram[65536];
	bool exit_flag;
public:
	Finale16Bus();
	~Finale16Bus();
	uint16_t read16(uint16_t addr);
	void write16(uint16_t addr, uint16_t data);
	uint8_t read8(uint16_t addr);
	void write8(uint16_t addr, uint8_t data);
	void clock();
	void reset();
	void set_exit_flag();
	int get_exit_flag();
	char* get_ram_as_read_buffer();
};
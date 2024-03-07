#include "bus.h"
Finale16Bus::Finale16Bus() {
    memset(ram, 0, sizeof(ram));
    mycpu.connect_to(this);
}
Finale16Bus::~Finale16Bus() {}
uint16_t Finale16Bus::read16(uint16_t addr) {
    return ((uint16_t)ram[addr + 1] << 8) | ram[addr];
}
void Finale16Bus::write16(uint16_t addr, uint16_t data) {
    ram[addr + 1] = data >> 8;
    ram[addr] = data & 0xff;
}
uint8_t Finale16Bus::read8(uint16_t addr) {
    return ram[addr];
}
void Finale16Bus::write8(uint16_t addr, uint8_t data) {
    ram[addr] = data;
}
void Finale16Bus::clock() {
    mycpu.clock();
}

void Finale16Bus::reset() {
    mycpu.reset();
    exit_flag = false;
}

void Finale16Bus::set_exit_flag(){
    exit_flag = true;
}
int Finale16Bus::get_exit_flag()
{
    return exit_flag;
}
char* Finale16Bus::get_ram_as_read_buffer()
{
    return (char*)ram;
}
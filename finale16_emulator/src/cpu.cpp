#include "cpu.h"
#include "bus.h"
#include <iostream>
#include <cstdio>
CPU::CPU() {
    bus = nullptr;
    reset();
}
CPU::~CPU() {}
void CPU::parse_rs()
{
    rs = (instr >> 4) & 0b1111;
}
void CPU::parse_rt()
{
    rt = (instr >> 8) & 0b1111;
}
void CPU::parse_rd()
{
    rd = instr & 0b1111;
}
void CPU::parse_ui4()
{
    ui4 = (instr >> 4) & 0xf;
}
void CPU::parse_ui8()
{
    ui8 = instr & 0xff;
}
void CPU::parse_si8()
{
    si8 = (instr & 0xff) | ((instr & 0x80) ? 0xff00 : 0);
}
void CPU::parse_si11()
{
    si11 = (instr & 0x7ff) | ((instr & 0x400) ? 0xf800 : 0);
}

void CPU::clock() {
    instr = bus->read16(pc);
    opcode = instr >> 12;
    if (opcode <= 1) {
        instr_run_2R();
    }else if (opcode <= 7) {
        instr_run_RI();
    }else if (opcode == 8) {
        instr_run_3R();
    }else if (opcode <= 13) {
        instr_run_RI();
    }else if (opcode == 14) {
        instr_run_I();
    }else{
        instr_run_Spe();
    }
    gpr[0] = 0;
}
void CPU::reset(){
    pc = 0x0000;
}
void CPU::connect_to(Finale16Bus* bus) {
    this->bus = bus;
}
void CPU::instr_run_2R()
{
    parse_rs();
    parse_rt();
    int funct = (instr & 0xf) + ((instr >> 8) & 16);
    switch (funct)
    {
    case 0: gpr[rt] = gpr[rt] - gpr[rs]; break;
    case 1: gpr[rt] = gpr[rs] - gpr[rt]; break;
    case 2: gpr[rt] = gpr[rs] & gpr[rt]; break;
    case 3: gpr[rt] = gpr[rs] | gpr[rt]; break;
    case 4: gpr[rt] = gpr[rs] ^ gpr[rt]; break;
    case 5:gpr[rt] = gpr[rs] << (gpr[rt] & 0xf); break;
    case 6:gpr[rt] = gpr[rs] >> (gpr[rt] & 0xf); break;
    case 7:gpr[rt] = static_cast<int16_t>(gpr[rs]) >> (gpr[rt] & 0xf); break;
    case 8: gpr[rt] = (static_cast<uint32_t>(gpr[rs]) * static_cast<uint32_t>(gpr[rt])); break;
    case 10: gpr[rt] = (static_cast<int32_t>(static_cast<int16_t>(gpr[rs])) * static_cast<int32_t>(static_cast<int16_t>(gpr[rt]))) >> 16; break;
    case 11: gpr[rt] = (static_cast<uint32_t>(gpr[rs]) * static_cast<uint32_t>(gpr[rt])) >> 16; break;
    case 12: gpr[rt] = (static_cast<int16_t>(gpr[rs]) / static_cast<int16_t>(gpr[rt])); break;
    case 13: gpr[rt] = gpr[rs] / gpr[rt]; break;
    case 14: gpr[rt] = (static_cast<int16_t>(gpr[rs]) % static_cast<int16_t>(gpr[rt])); break;
    case 15: gpr[rt] =  gpr[rs] % gpr[rt]; break;
    case 16: gpr[10] = static_cast<int16_t>(gpr[rs]) < static_cast<int16_t>(gpr[rt]) ? 1 : 0; break;
    case 17: gpr[10] = static_cast<int16_t>(gpr[rs]) >= static_cast<int16_t>(gpr[rt]) ? 1 : 0; break;
    case 18: gpr[10] = gpr[rs] < gpr[rt] ? 1 : 0; break;
    case 19: gpr[10] = gpr[rs] >= gpr[rt] ? 1 : 0; break;
    case 20: gpr[rt] = pc + 2; pc = gpr[rs] - 2;    break;      //JRL
    case 21: gpr[10] = gpr[rs] != gpr[rt] ? 1 : 0; break;       //JCRL

    case 22: gpr[rt] = pc + 2; pc = gpr[rs] - 2;  break;
    case 23: if(gpr[10]) gpr[rt] = pc + 2; pc = gpr[rs] - 2;  break;
    case 24:if (rs != 0 || rd != 0) cpu_error();  pc = gpr[rt];  break;
    case 25:if (rs != 0 || rd != 0) cpu_error();  pc = gpr[rt];  break;
    case 26:if (rs != 0 || rd != 0) cpu_error();  pc = gpr[rt];  break;
    default:cpu_error(); break;
    }
    pc = pc + 2;
}
void CPU::instr_run_I6() {
    parse_rs();
    parse_rt();
    parse_ui6();
    switch (opcode){
    case 2: gpr[rt] = bus->read16(gpr[rs] + (ui6 << 1)); break;
    case 3: bus->write16(gpr[rs] + (ui6 << 1), gpr[rt]); break;
    case 4: gpr[rt] = bus->read8(gpr[rs] + (ui6 << 1)); break;
    case 5: gpr[rt] = static_cast<int8_t>(bus->read8(gpr[rs] + (ui6 << 1))); break;
    case 6: bus->write8(gpr[rs] + (ui6 << 1), static_cast<uint8_t>(gpr[rt])); break;
    default:cpu_error(); break;
    }
    pc += 2;
}
void CPU::instr_run_I8() {
    parse_rt();
    parse_si8();
    int funct = (opcode << 1) | (instr & 0b1);
    switch (funct){
    case 14: gpr[rt] = pc += gpr[rt] == 0 ? (si8 << 1) : 2; break;
    case 15: gpr[rt] = pc += gpr[rt] != 0 ? (si8 << 1) : 2; break;
    case 16: gpr[rt] = si8; pc += 2; break;
    case 17: gpr[rt] = (si8 & 0xff) << 8; pc += 2; break;
    case 18: gpr[rt] = pc + static_cast<uint16_t>((si8 & 0xff) << 8); pc += 2; break;
    case 19: gpr[rt] = gpr[rt] & si8; pc += 2; break;
    case 20: gpr[rt] = gpr[rt] | si8; pc += 2; break;
    case 21: gpr[rt] = gpr[rt] ^ si8; pc += 2; break;
    case 22: gpr[rt] = gpr[rt] + si8; pc += 2; break;
    default:cpu_error(); break;
    }
}
void CPU::instr_run_I4() {
    parse_rs();
    parse_rt();
    parse_ui4();
    int funct = (instr & 0b11);
    switch (funct){
    case 0: gpr[rt] = gpr[rs] << ui4; break;
    case 1: gpr[rt] = gpr[rs] >> ui4; break;
    case 2: gpr[rt] = static_cast<int16_t>(gpr[rt]) >> ui4; break;
    default:cpu_error(); break;
    }
    pc += 2;
}
void CPU::instr_run_I11(){
    parse_si11();
    if ((instr & 1) == 0) {
        pc += si11 << 1;
    } else {
        gpr[1] = pc;  
        pc += si11 << 1;
    }
}
void CPU::instr_run_Spe(){
    if (instr == 0xffff) {
        std::cout << "CPU info: This is a simexit instruction." << std::endl;
        bus->set_exit_flag();
    } else {
        cpu_error();
    }
    pc += 2;
}
void CPU::cpu_error()
{
    std::cout << "CPU error: Invalid instruction 0x" << std::hex << instr <<" at pc = 0x"<< pc << std::dec << std::endl;
    bus->set_exit_flag();
}
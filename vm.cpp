#include "vm.h"
#include <fstream>
#include <algorithm>

const uint8_t fonts[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

std::mt19937 Vm::gen{std::random_device()()};
std::uniform_int_distribution<> Vm::dist(0, 255);

Vm::Vm() : pc(START_ADDRES) {
	std::copy(fonts, fonts + 80, memory);
}

void Vm::load(std::string file_name) {
	std::ifstream fin(file_name);
	fin.read(reinterpret_cast<char*>(memory + START_ADDRES)
			,MEMORY_SIZE - START_ADDRES);
}

void Vm::cycle() {
	const uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
	uint8_t &vx = reg[(opcode & 0x0F00) >> 8];
	uint8_t &vy = reg[opcode & 0x000F];
	uint8_t &vf = reg[0xF];
	uint8_t &v0 = reg[0];
	const uint8_t val = opcode & 0x00FF;
	const uint8_t addr = opcode & 0x0FFF;

	switch ((opcode & 0xF000) >> 12) {
		case 0:
			switch (opcode) {
				case 0x00E0:
					std::fill_n(screen, WIDTH * HEIGHT, 0);
					pc += 2;
					break;
				case 0x00EE:
					pc = stack[sp];
					sp--;
					break;
			}
			break;
		case 1: //jump to address 1xxx
			pc = addr;
			break;
		case 2: //call func at address 2xxx
			sp++;
			stack[sp] = pc;
			pc = opcode & 0x0FFF;
			break;
		case 3: //skip next instruction if reg is eq to val
			if (vx == val)
				pc += 4;
			else
				pc += 2;
			break;
		case 4: //skip next instruction if reg is not eq to val
			if (vx != val)
				pc += 4;
			else
				pc += 2;
			break;
		case 5: //skip next instruction if reg0 is eq to reg1
			if (vx == vy)
				pc += 4;
			else
				pc += 2;
			break;
		case 6: //copy value into register
			vx = val;
			pc += 2;
			break;
		case 7: //add value to reg and store it
			vx += val;
			pc += 2;
			break;
		case 8: 
			switch (opcode & 0x000F) {
				case 0: //set reg0 to reg1
					vx = vy;
					break;
				case 1: //reg0 = reg0 | reg1
					vx |= vy;
					break;
				case 2: //reg0 = reg0 & reg1
					vx &= vy;
					break;
				case 3: //reg0 = reg0 ^ reg1
					vx ^= vy;
					break;
				case 4:
					if ((int)vx + vy > 255)
						vf = 1;
					else
						vf = 0;
					vx += vy;
					break;
				case 5:
					if (vx > vy)
						vf = 1;
					else
						vf = 0;
					vx += vy;
					break;
				case 6:
					vf = vx & 0x01;
					vx >>= 1;
					break;
				case 7:
					if (vy > vx)
						vf = 1;
					else
						vf = 0;
					vy -= vx;
					break;
				case 0xE:
					vf = vx & 0x80 >> 7;
					vx <<= 1;
					break;
			}
			pc += 2;
			break;
		case 9:
			if (vx != vy)
				pc += 4;
			else
				pc += 2;
			break;
		case 0xA:
			I = addr;
			pc += 2;
			break;
		case 0xB:
			pc = v0 + addr;
			break;
		case 0xC:
			vx = dist(gen) & val;
			pc += 2;
			break;
		case 0xD:
			const int n = opcode & 0x000F;
			for (int i = 0; i < n; i++) {
				uint8_t pixel = memory[I + i];
				for (int x = 0; x < 8; x++) {
					const int index = vx + x + (vy + i) * WIDTH;
					if (pixel & (0x80 >> x)) {
						if (screen[index]) {
							vf = 1;
						}
						screen[index] ^= 1;
					}
				}
			}
			pc += 2;
			break;
	}
}

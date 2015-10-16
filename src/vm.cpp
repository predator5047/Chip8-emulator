#include <fstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include "vm.h"

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

Vm::Vm() : drawFlag(true), I(0), pc(START_ADDRES), sp(0), dt(0), st(0) {
	std::fill(std::begin(memory), std::end(memory), 0);
	std::fill(std::begin(stack), std::end(stack), 0);
	std::copy(std::begin(fonts), std::end(fonts), memory);
	std::fill_n(screen, WIDTH * HEIGHT, 0);
}

const uint8_t* Vm::getScreen() {
	return screen;
}

bool* Vm::getKeyboard() {
	return keyboard;
}

void Vm::load(std::string file_name) {
	std::ifstream fin(file_name);
	fin.read(reinterpret_cast<char*>(memory + START_ADDRES)
			,MEMORY_SIZE - START_ADDRES);
}

void Vm::cycle() {
	const uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
	uint8_t &vx = reg[(opcode & 0x0F00) >> 8];
	uint8_t &vy = reg[(opcode & 0x00F0) >> 4];
	uint8_t &vf = reg[0xF];
	uint8_t &v0 = reg[0];
	const uint8_t val = opcode & 0x00FF;
	const uint16_t addr = opcode & 0x0FFF;
	std::cout << std::hex << "Executing " << std::uppercase << opcode
	<< " at 0" << pc << " , I:" << I << " SP:0" << sp << "\n";

	switch ((opcode & 0xF000) >> 12) {
		case 0:
			switch (opcode) {
				case 0x00E0:
					drawFlag = true;
					std::fill_n(screen, WIDTH * HEIGHT, 0);
					pc += 2;
					break;
				case 0x00EE:
					sp--;
					pc = stack[sp];
					pc += 2;
					break;
			}
			break;
		case 1: //jump to address 1xxx
			pc = addr;
			break;
		case 2: //call func at address 2xxx
			stack[sp] = pc;
			sp++;
			pc = addr;
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
					vf = (vx & 0x80) >> 7;
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
		case 0xD: {
			const int n = opcode & 0x000F;
			drawFlag = true;
			vf = 0;
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
		}
			break;
		case 0xE:
			switch (opcode & 0x00FF) {
				case 0x9E:
					if (keyboard[vx]) {
						pc += 4;
					} else {
						pc += 2;
					}
					break;
				case 0xA1:
					if (!keyboard[vx]) {
						pc += 4;
					} else {
						pc += 2;
					}
					break;
			}
			break;
		case 0xF:
			switch (opcode & 0x00FF) {
				case 0x07:
					vx = dt;
					break;
				case 0x0A: {
					auto *i = std::find(std::begin(keyboard)
						,std::end(keyboard), true);
					if (i != std::end(keyboard)) {
						vx = i - keyboard;
					} else {
						return;
					}
					break;
				}
				case 0x15:
					dt = vx;
					break;
				case 0x18:
					st = vx;
					break;
				case 0x1E:
					vf = 0;
					if ((int)I + vx > 0xFFF) {
						vf = 1;
					}
					I += vx;
					break;
				case 0x29:
					I = vx * 5;
					break;
				case 0x33:
					memory[I] = vx / 100;
					memory[I + 1] = (vx / 10) % 10;
					memory[I + 2] = vx % 10;
					break;
				case 0x55: {
					const int n = (opcode & 0x0F00) >> 8;
					std::copy(reg, reg + n, memory + I);
					break;
				}
				case 0x65: {
					const int n = (opcode & 0x0F00) >> 8;
					std::copy(memory + I , memory + I + n, reg);
					break;
				}
			}
			pc += 2;
			break;
	}
	if (dt > 0)
		dt--;
	if (st > 0)
		st--;
}

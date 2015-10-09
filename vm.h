#ifndef VM_H_
#define VM_H_

#include <string>
#include <random>

class Vm {
private:
	uint8_t memory[4096];
	uint8_t reg[16];
	uint16_t stack[16];
	uint16_t I; // register used for memory operations
	uint16_t pc; // program counter
	uint16_t sp; // stack pointer
	std::mt19937 gen;
	std::uniform_int_distribution<> dist;
public:
	Vm();
	void cycle();
	void load(std::string file_name);
};

#endif

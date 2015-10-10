#ifndef VM_H_
#define VM_H_

#include <string>
#include <random>

class Vm {
public:
	const static int WIDTH = 64, HEIGHT = 32;
	Vm();
	void cycle();
	void load(std::string file_name);

private:
	const static int MEMORY_SIZE = 4096;
	const static int NUMBER_OF_REGISTER = 16;
	const static int STACK_LEVELS = 16;
	const static int START_ADDRES = 0x200;
	static std::mt19937 gen;
	static std::uniform_int_distribution<> dist;

	uint8_t memory[MEMORY_SIZE];
	uint8_t reg[NUMBER_OF_REGISTER];
	uint8_t screen[WIDTH * HEIGHT];
	uint16_t stack[STACK_LEVELS];
	uint16_t I; // register used for memory operations
	uint16_t pc; // program counter
	uint16_t sp; // stack pointer
};

#endif

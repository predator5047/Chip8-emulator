Vm::Vm() : pc(0x200) {
}


void Vm::cycle() {
	uint16_t opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000 >> 12) {
		case 0:
			switch (opcode) {
				case 0x00E0:
					//to do clear screen
					pc += 2;
					break;
				case 0x00EE:
					pc = stack[sp];
					sp--;
					break;
			}
			break;
		case 1: //jump to address 1xxx
			pc = opcode & 0x0FFF;
			break;
		case 2: //call func at address 2xxx
			sp++;
			stack[sp] = pc;
			pc = opcode & 0x0FFF;
			break;
		case 3:
			uint8_t r = reg[opcode & 0x0F00] >> 8;
			uint8_t val = opcode & 0x00FF;
			if (r == val)
				pc += 4;
			break;
		case 4:
			break;
	}
}

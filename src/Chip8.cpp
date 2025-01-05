#include <cstdint>
#include <fstream>
#include <vector>
#include "Chip8.h"
#include <iostream>
#include <random>

// constructor
Chip8::Chip8() {
	// Load fontset into memory (0x000 to 0x1FF)
	uint8_t fontset[]{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};

	for (unsigned int i = 0; i < sizeof(fontset); ++i) {
		// fontset shouldn't be stored beyond address 0x1FF
		if (i > 0x1FF) {
			return;
		}
		memory[i] = fontset[i];
	}; // load the fontset into memory

	// random seed
	srand(time(NULL));

	// initialize sound buffer
	if (!sound.openFromFile("assets/sound.mp3")) {
		// sound didn't load
	}
}

// random number generator
uint8_t Chip8::RNG() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);
    return dis(gen);
}

void Chip8::LoadROM(const char* filename) {
	std::ifstream file(
		filename, std::ios::binary |
		std::ios::ate); // read file from end, in binary format

	if (file.is_open()) {
		int size = file.tellg(); // check position of cursor (file size)

		std::vector<char> buffer(
			size); // prepare buffer with size = size of file

		file.seekg(0, std::ios::beg); // move cursor to beginning of file
		file.read(buffer.data(), size);

		for (long i = 0; i < size; ++i) {
			// TODO: make sure rom doesnt take up more space than it should
			memory[ROM_START_ADDRESS + i] = buffer[i];
		}
		program_counter = ROM_START_ADDRESS; // initialize program counter
	}
	is_rom_loaded = true;
};

void Chip8::ExecuteInstruction(uint16_t opcode) {
	// for debugging only
	// opcodes.push_back(opcode);

	uint16_t nnn = opcode & 0x0FFF;     // lowest 12 bits
	uint8_t n = opcode & 0x000F;        // lowest 4 bits
	uint8_t x = (opcode >> 8) & 0x000F; // low 4 bits of high byte
	uint8_t y = (opcode >> 4) & 0x000F; // high 4 bits of low byte
	uint8_t kk = opcode & 0x00FF;       // lowest 8 bits

	uint8_t instruction = (opcode >> 12) & 0x000F;

	switch (instruction) { // beginning of switch case
	case 0:
		switch (nnn) {
		case 0x0E0:
			std::fill(std::begin(display), std::end(display), 0);
			break;
		case 0x0EE:
			// return from subroutine
			stack_pointer -= 1;
			program_counter = stack[stack_pointer];
			break;
		};
		break;

	case 1:
		program_counter = nnn;
		break;

	case 2:
		stack[stack_pointer] = program_counter;
		++stack_pointer;
		program_counter = nnn;
		break;

	case 3:
		if (V[x] == kk) {
			program_counter += 2;
		}
		break;

	case 4:
		if (V[x] != kk) {
			program_counter += 2;
		}
		break;

	case 5:
		if (V[x] == V[y]) {
			program_counter += 2;
		}
		break;

	case 6:
		V[x] = kk;
		break;

	case 7:
		V[x] += kk;
		break;

	case 8:
		switch (n) {
		case 0:
			V[x] = V[y];
			break;

		case 1:
			V[x] = V[x] | V[y];
			break;

		case 2:
			V[x] = V[x] & V[y];
			break;

		case 3:
			V[x] = V[x] ^ V[y];
			break;

		case 4: {
			uint16_t result = V[x] + V[y];
			uint8_t result_8bit = result & 0xFF;
			V[x] = result_8bit;

			if (result > 255) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			break;
		}

		case 5: {
			bool carry = V[x] >= V[y];
			V[x] -= V[y];

			if (carry) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			//
			break;
		}

		case 6: {
			// optional: set V[x] to the value of V[y]
			uint8_t bit = V[x] & 0x01;
			V[x] >>= 1;
			if (bit == 1) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			break;
		}

		case 7:
		{
			bool not_borrow = V[y] >= V[x];
			V[x] = V[y] - V[x];

			if (not_borrow) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			//
			break;
		}


		case 0xE: {
			// optional: set V[x] to the value of V[y]
			uint8_t bit = (V[y] >> 7) & 0x01; // get the most significant bit
			V[x] = V[y] << 1;
			if (bit == 1) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			break;
		}
		}
		break;

	case 9:
		if (V[x] != V[y]) {
			program_counter += 2;
		}
		break;

	case 0xA:
		I = nnn;
		break;

	case 0xB:
		// ambiguous
		program_counter = nnn + V[0];
		break;

	case 0xC: {
		// TODO: check if random number is correct
		//uint8_t random_number = rand() % 256;
		V[x] = RNG() & kk;
		break;
	}

	case 0xD: {
		V[0xF] = 0;
		// TODO: directly write to a rectangle shape array for SFML, skip the display
		uint8_t coordinate_x = V[x];
		uint8_t coordinate_y = V[y];

		coordinate_x %= DISPLAY_WIDTH;
		coordinate_y %= DISPLAY_HEIGHT;

		for (unsigned int col = 0; col < n; ++col) {
			uint8_t sprite_byte = memory[I + col];

			if (coordinate_y + col > DISPLAY_HEIGHT) {
				break;
			}

			for (unsigned int row = 0; row < 8; ++row) {
				uint8_t sprite_bit = (sprite_byte >> (7 - row)) & 0x01;
				// gets the most significant bit, i.e. 1011 0111, will
				// get 1 first, then 0 etc.

				// formula for converting x and y coordinate to index:
				// (x + DISPLAY_WIDTH * y)
				// just need to keep incrementing x and y
				//
				if (coordinate_x + row > DISPLAY_WIDTH) {
					break;
				}

				int display_index = (coordinate_x + row) + (DISPLAY_WIDTH * (coordinate_y + col));
				uint8_t display_bit = display[display_index];
				

				if (display_bit == 1 && sprite_bit == 1) {
					display[display_index] = 0; // turn off pixel
					V[0xF] = 1;
				}
				else if (display_bit == 0 && sprite_bit == 1) {
					display[display_index] = 1; // turn on pixel
				}

			}
		}
		break;
	}

	case 0xE:
		switch (kk) {
		case 0x9E:
			// TODO: V[x] must be 0-F
			if (keypad[V[x]]) { // if key V[x] is pressed
				program_counter += 2;
			}
			break;
		case 0xA1:
			// TODO: V[x] must be 0-F
			if (!keypad[V[x]]) { //if key V[x] is not pressed
				program_counter += 2;
			}
			break;
		}
		break;

	case 0xF:
		switch (kk) {
		case 0x07:
			V[x] = delay_timer;
			break;
		case 0x15:
			delay_timer = V[x];
			break;
		case 0x18:
			sound_timer = V[x];
			break;
		case 0x1E:
			I += V[x];
			if (I > 0xFFF) {
				V[0xF] = 1;
			}
			break;
		case 0x0A:
			// loop through keypad, if any are true, set V[x] to it, else move program_counter backwards
			for (unsigned int i = 0; i < sizeof(keypad); ++i) {
				if (keypad[i]) {
					V[x] = i;
					break;
				}

				if (i == sizeof(keypad) - 1) {
					program_counter -= 2;
				}
			}
			break;

		case 0x29:
			I = 5 * (V[x] & 0x0F); // take last nibble as hexacdecimal
			// character, each character is 5 bytes long
			break;

		case 0x33:
			memory[I] = V[x] / 100;            // get first digit
			memory[I + 1] = (V[x] % 100) / 10; // get the second digit
			memory[I + 2] = V[x] % 10;
			break;

		case 0x55:
			for (unsigned int i = 0; i <= x; ++i) {
				memory[I + i] = V[i];
			}
			break;

		case 0x65:
			for (unsigned int i = 0; i <= x; ++i) {
				V[i] = memory[I + i];
			}
			break;
		}
		break;
		// end of switch case
	};
}

void Chip8::Cycle() {
	// get the current instruction from memory[program_counter] and combine it with the next byte for the full 16 bit opcode 
	uint16_t opcode = (memory[program_counter] << 8) | (memory[program_counter + 1]);
	// iterate the program counter
	program_counter += 2;
	// execute the full opcode/instruction
	ExecuteInstruction(opcode);
}

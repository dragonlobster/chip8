#pragma once

#include <cstdint>
#include <SFML/Graphics.hpp>

class Chip8 {
public:
	//constructor
	Chip8();

	// display
	const static uint8_t DISPLAY_WIDTH = 64;
	const static uint8_t DISPLAY_HEIGHT = 32;

	// display
	uint8_t display[Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT]{}; // defaulted to 0s

	void LoadROM(const char* filename); // load the rom into memory

	// TODO: delay / sound
	uint8_t delay_timer{};
	uint8_t sound_timer{};

	// keypad 16 keys: 0 - F -> 1234,qwer,asdf,zxcv, either pressed or not pressed (true, false)
	bool keypad[16]{}; // all initialized to false

	void Cycle();

	// for debugging opcode (store all opcodes in sequence)
	//std::vector<uint16_t> opcodes{};

private:
	// memory
	uint8_t memory[4096]{};
	const uint16_t ROM_START_ADDRESS{ 0x200 };

	// registers
	uint8_t V[16]{}; // 16 8-bit general purpose registers
	uint16_t I{};    // only right most 12 bits are usually used

	// TODO: sound
	uint16_t program_counter{};
	uint8_t stack_pointer{ 0 };
	uint16_t stack[16]{};

	// random number between 0 and 255
	uint8_t RNG();

	// execute a single instruction
	void ExecuteInstruction(uint16_t opcode);

};

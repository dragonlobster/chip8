#pragma once

#include "Chip8.h"

template <typename T>
void HandleInput(bool (&keypad)[16], const T& keys, const bool& pressed) {
	switch (keys.scancode) {
	case sf::Keyboard::Scancode::Num1:
		keypad[0] = pressed;
		break;
	case sf::Keyboard::Scancode::Num2:
		keypad[1] = pressed;
		break;
	case sf::Keyboard::Scancode::Num3:
		keypad[2] = pressed;
		break;
	case sf::Keyboard::Scancode::Num4:
		keypad[3] = pressed;
		break;
	case sf::Keyboard::Scancode::Q:
		keypad[4] = pressed;
		break;
	case sf::Keyboard::Scancode::W:
		keypad[5] = pressed;
		break;
	case sf::Keyboard::Scancode::E:
		keypad[6] = pressed;
		break;
	case sf::Keyboard::Scancode::R:
		keypad[7] = pressed;
		break;
	case sf::Keyboard::Scancode::A:
		keypad[8] = pressed;
		break;
	case sf::Keyboard::Scancode::S:
		keypad[9] = pressed;
		break;
	case sf::Keyboard::Scancode::D:
		keypad[10] = pressed; // 0xA
		break;
	case sf::Keyboard::Scancode::F:
		keypad[11] = pressed; // 0xB
		break;
	case sf::Keyboard::Scancode::Z:
		keypad[12] = pressed; // 0xC
		break;
	case sf::Keyboard::Scancode::X:
		keypad[13] = pressed; // 0xD
		break;
	case sf::Keyboard::Scancode::C:
		keypad[14] = pressed; // 0xE
		break;
	case sf::Keyboard::Scancode::V:
		keypad[15] = pressed; // 0xF
		break;
	}
}

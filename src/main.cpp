#include <SFML/Graphics.hpp>
#include "Chip8.h"
#include "DrawUtils.h"
#include <iostream>
#include "HandleInput.h"


int main() {
	// chip8
	Chip8 chip8{};

	sf::RenderWindow window(sf::VideoMode({ Chip8::DISPLAY_WIDTH * DrawUtils::SCALE, Chip8::DISPLAY_HEIGHT * DrawUtils::SCALE }), "SFML window!");
	//window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	// test brix rom
	chip8.LoadROM("roms/tests/6-keypad.ch8");

	double accumulator{ 0 };
	double last_frame_time{ 0 };

	// frame clock (avoiding setFrameRateLimit imprecision)
	sf::Clock frame_clock;

	while (window.isOpen()) {

		window.handleEvents(
			// on close
			[&](const sf::Event::Closed&) { window.close(); },

			// on key pressed
			[&](const sf::Event::KeyPressed& key) {
				// keypad 16 keys: 0 - F -> 1234,qwer,asdf,zxcv, either pressed or not pressed (true, false)
				HandleInput<sf::Event::KeyPressed>(chip8.keypad, key, true);
			},

			// on key released
			[&](const sf::Event::KeyReleased& key) {
				HandleInput<sf::Event::KeyReleased>(chip8.keypad, key, false);
			}
		);

		const double current_time = frame_clock.getElapsedTime().asMilliseconds(); // get current time
		const double delta_time = current_time - last_frame_time; // delta always calculates the difference between now within this frame and last frame
		accumulator += delta_time; // add the delta time to the accumulator, we need to wait until it reachs ~1.67 milliseconds (60 fps, ~1.67ms / frame)

		while (accumulator >= 1000.f / 60.f) { // why while and not if? in the case that we missed out a whole frame for some reason, double the instructions will be processed (22 instructions, since twice loop until accumulator resets again)
			// accumulator has reached 1.67 milliseconds or greater
			if (chip8.delay_timer > 0) {
				chip8.delay_timer--;
			}

			// 11 instructions / frame
			for (unsigned int i = 0; i < 11; ++i) {
				chip8.Cycle();
			}

			// subtract it down to 0 again (or offset by screen refresh rate), basically resetting accumulator
			accumulator -= 1000.f / 60.f; 
		}

		// clear/draw/display
		window.clear();

		const std::vector<sf::RectangleShape> pixels = DrawUtils::draw_display_buffer(chip8.display);
		for (const sf::RectangleShape& pixel : pixels) {
			window.draw(pixel);
		}
		window.display();

		// we are setting the "last frame time" as the current time before going to the next frame
		last_frame_time = current_time;
	}

	return 0;
}
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <SDL2/SDL.h>
#include "vm.h"
using namespace std;
using namespace std::chrono;

template <class T>
using delete_unique_ptr = unique_ptr<T, function<void(T*)>>;

Vm vm;
const int SCALE = 20;
const int WINDOW_HEIGHT = SCALE * Vm::HEIGHT;
const int WINDOW_WIDTH = SCALE * Vm::WIDTH;
delete_unique_ptr<SDL_Window> win;

void processKeys() {
	const auto keys = SDL_GetKeyboardState(NULL);
	auto vm_keyboard = vm.getKeyboard();
	const int keymap[] = {
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_4,
		SDL_SCANCODE_Q,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_R,
		SDL_SCANCODE_A,
		SDL_SCANCODE_S,
		SDL_SCANCODE_D,
		SDL_SCANCODE_F,
		SDL_SCANCODE_Z,
		SDL_SCANCODE_X,
		SDL_SCANCODE_C,
		SDL_SCANCODE_V,
	};
	fill_n(vm_keyboard, 0x10, false);
	for (int i = 0; i < 0x10; i++) {
		if (keys[keymap[i]]) {
			vm_keyboard[i] = true;
		}		
	}
}


void draw() {
	auto surface = SDL_GetWindowSurface(win.get());
	auto screen = vm.getScreen();
	SDL_LockSurface(surface);
	auto pixels = reinterpret_cast<Uint32*>(surface->pixels);

	for (int y = 0; y < WINDOW_HEIGHT; y++) {
		for (int x = 0; x < WINDOW_WIDTH; x++) {
			int indx_surface = y * surface->w + x;
			int indx_screen = y / SCALE * Vm::WIDTH + x / SCALE;
			if (screen[indx_screen] == 0) {
				pixels[indx_surface] = 0;
			} else {
				pixels[indx_surface] = 0xFFFFFFFF;
			}
		}
	}

	SDL_UnlockSurface(surface);
	SDL_UpdateWindowSurface(win.get());
}

int main() {
	vm.load("roms/CONNECT4");
	const Uint8 *keys;

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		cout << "SDL init error\n";
		return 1;
	}
	win = delete_unique_ptr<SDL_Window>(
		SDL_CreateWindow("CHIP8", 100, 100, WINDOW_WIDTH, 
			WINDOW_HEIGHT, SDL_WINDOW_SHOWN),
		SDL_DestroyWindow);

	if (win == nullptr) {
		cout << "SDL error creating window\n";
		SDL_Quit();
		return -1;
	}

	while (true) {
		vm.cycle();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			keys = SDL_GetKeyboardState(NULL);
			if (keys[SDL_SCANCODE_ESCAPE]) {
				return 0;
			}
			processKeys();
		}

		draw();
	}
	return 0;
}

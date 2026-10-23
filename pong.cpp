#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <vector>

//gameplay variables
const int screen_width{80};
const int screen_height{30};
const int frame_rate{8};
const std::chrono::milliseconds frame_duration(1000/ frame_rate);

void enableANSI() {
	//allows the use of ANSI codes
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

DWORD originalMode;
HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

void enableRawMode() {
        GetConsoleMode(hInput, &originalMode);

        DWORD rawMode = originalMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hInput, rawMode);
}

void disableRawMode() {
        SetConsoleMode(hInput, originalMode);
}

void setConsoleSize(int width, int height) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

    // 1. Set screen buffer size first to ensure it fits the desired window
	COORD bufferSize = { (SHORT)width, (SHORT)height };
	SetConsoleScreenBufferSize(hOut, bufferSize);

    // 2. Set the window rectangle (0-indexed coordinates)
	SMALL_RECT windowSize = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
	SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}

void drawScreen(int width, int height) {

	std::string frame_buffer = "";
	frame_buffer += "\n";
	frame_buffer += "\033[1;1H";

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (y == 0 || y == height-1) {
				frame_buffer += "-";
			} else if (x == 0 || x == width-1) {
				frame_buffer += "|";
			} else {
				frame_buffer += " ";
			}
		}

	frame_buffer += "\n";
		
	}

	std::cout << frame_buffer << std::flush;
}

int main() {

	setConsoleSize(screen_width, screen_height);

	enableANSI();
        std::cout << "\033[?25l" << std::flush;
	enableRawMode();

	bool running = true;
	while (running) {

		auto frame_start = std::chrono::steady_clock::now();

		if (_kbhit()) {
			char ch = _getch();

			if (ch == 'x' || ch == 'X') {
				running = false;
			}
		}

		drawScreen(screen_width, screen_height);

		auto frame_end = std::chrono::steady_clock::now();
		auto frame_length = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);
		if (frame_length < frame_duration) {
			std::this_thread::sleep_for(frame_duration - frame_length);
		}
		
	}

	disableRawMode();
	std::cout << "\033[?25h";
	return 0;
}

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <vector>

void enableANSI() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;

	dwMode != ENABLE_VIRTUAL_TERMINAL_PROCESSING;
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

int main() {
	enableRawMode();

	bool running = true;

	while (running) {

		std::cout << "Running" << "\n";

		if (_kbhit()) {
			char ch = _getch();

			if (ch == 'x' || ch == 'X') {
				running = false;
			}
		}
	}

	disableRawMode();
	return 0;
}

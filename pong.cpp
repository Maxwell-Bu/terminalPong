#include <string>	
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>

//DO TOMORROW:
//Make collision check/handle function
//Allow ball to bounce
//Think about 2nd paddle AI

class Paddle {
private:
	int center = 14;
	int column; 
	std::vector<int> normal;
public:
	Paddle(int c, std::vector<int> norm) : column(c), normal(norm) {
	
	}

	int returnCenter() {
		return center;
	}
	int returnColumn() {
		return column;
	}

	void moveUp() {
		center -= 1;
	}

	void moveDown() {
		center += 1;
	}
};

class Ball {
private:
	char symbol = 'O';	

	std::vector<int> pos = {40, 15};
	std::vector<int> vel = {1, 0};
public:
	void movement() {
		std::vector<int> result(pos.size());

		std::transform(pos.begin(), pos.end(), vel.begin(), result.begin(), std::plus<int>());
		pos = result;
	}

	void bouncedX(int rand) {
		vel = {-1 * vel.at(0), rand};
	}
	
	void bouncedTop() {
		pos = {pos.at(0), 1};
		vel = {vel.at(0), -1 * vel.at(1)};
	}

	void bouncedBottom() {
		pos = {pos.at(0), 28};
		vel = {vel.at(0), -1 * vel.at(1)};
	}
	
	int returnXpos() {
		return pos.front();
	}
	
	int returnYpos() {
		return pos.back();
	}
};

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

void drawScreen(int width, int height, auto& pad1, auto& pad2, auto& gameBall) {

	int pad1center = pad1.returnCenter();
	int pad1col = pad1.returnColumn();
	
	int pad2center = pad2.returnCenter();
	int pad2col = pad2.returnColumn();

	int ballX = gameBall.returnXpos();
	int ballY = gameBall.returnYpos();

	std::string frame_buffer = "";
	frame_buffer += "\n";
	frame_buffer += "\033[1;1H";

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (y == 0 || y == height-1) {
				frame_buffer += "-";
			} else if (x == 0 || x == width-1) {
				frame_buffer += "|";
			} else if (y >= pad1center -2 && y <= pad1center + 2 && x == pad1col) { 
				frame_buffer += "#";
			} else if (y >= pad2center -2 && y <= pad2center + 2 && x == pad2col) {
				frame_buffer += "#";
			} else if (x == ballX && y == ballY) {
				frame_buffer += "O";
			} else {
				frame_buffer += " ";
			}
		}
		frame_buffer += "\n";
		
	}

	std::cout << frame_buffer << std::flush;
}

void collisionHandle(int ballX, int ballY, int pad1col, int pad1center, int pad2col, int pad2center, auto& ball, int rand) {

	if (ballX == pad1col +1) {
		if (ballY >= pad1center -2 && ballY <= pad1center +2) {
			ball.bouncedX(rand);
		}
	}
	if (ballX == pad2col -1) {
		if (ballY >= pad2center -2 && ballY <= pad2center +2) {
			ball.bouncedX(rand);
		}
	}
	if (ballY <= 1) {
		ball.bouncedTop();
	} else if (ballY >= 28) {
		ball.bouncedBottom();
	}
}

int main() {

	setConsoleSize(screen_width, screen_height);

	enableANSI();
        std::cout << "\033[?25l" << std::flush;
	enableRawMode();

	std::vector<int> pad1norm = {1, 0};
	std::vector<int> pad2norm = {-1, 0};

	auto pad1 = Paddle(4, pad1norm);
	auto pad2 = Paddle(75, pad2norm);

	auto gameBall = Ball();

	//RNG setup
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> range(-2, 2);

	bool running = true;
	while (running) {

		auto frame_start = std::chrono::steady_clock::now();

		if (_kbhit()) {
			char ch = _getch();

			if (ch == 'x' || ch == 'X') {
				running = false;
			} else if (ch == 'w' || ch == 'W') {
				pad1.moveUp();
			} else if (ch == 's' || ch == 'S') {
				pad1.moveDown();
			}
		}

		gameBall.movement();
		collisionHandle(gameBall.returnXpos(), gameBall.returnYpos(), pad1.returnColumn(), pad1.returnCenter(), pad2.returnColumn(), pad2.returnCenter(), gameBall, range(gen));

		drawScreen(screen_width, screen_height, pad1, pad2, gameBall);

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

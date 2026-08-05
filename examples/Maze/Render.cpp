#include "Render.hpp"
#include "Ansi.hpp"
#include <iostream>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

std::mutex drawMutex;

void enableAnsiColors(){
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if(h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)){
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif
}

void resetCursor(){
	std::cout << "\x1B[H";
}

void render(const GraphT& g,
            const std::vector<char>& overlay,
            const std::vector<std::unique_ptr<LetterState>>& letters){
    const std::size_t numCols = g.getCol();
    const std::size_t numRows = g.getRow();
    if(numCols == 0 || numRows == 0){
		return;
	}

    std::vector<char> frame(numCols * numRows, '\0');
    std::vector<int>  owner(numCols * numRows, -1);
    for(std::size_t i = 0; i < overlay.size(); ++i){
		frame[i] = overlay[i];
	}
    for(std::size_t i = 0; i < letters.size(); ++i){
        std::size_t idx = letters[i]->currentIdx.load(std::memory_order_relaxed);
        if(idx < frame.size()){
			frame[idx] = letters[i]->ch;
			owner[idx] = static_cast<int>(i);
		}
    }

    std::lock_guard<std::mutex> lk(drawMutex);
    resetCursor();
    for(std::size_t row = 0; row + 1 < numRows; ++row){
        for(std::size_t col = 0; col < numCols; ++col){
            std::size_t idx = row * numCols + col;
            char c = frame[idx];
            if(c != '\0'){
                int o = owner[idx];
                std::cout << (o >= 0 ? algoColor(letters[o]->algo) : WHITE) << c << RESET;
            }else{
                std::cout << (g[idx].blocked ? '#' : ' ');
            }
        }
        std::cout << "\x1B[K\n";
    }
    std::cout << "\x1B[K\n";
    for(std::size_t col = 0; col < numCols; ++col){
        std::size_t idx = (numRows - 1) * numCols + col;
        char c = frame[idx];
        if(c != '\0'){
            int o = owner[idx];
            std::cout << (o >= 0 ? algoColor(letters[o]->algo) : WHITE) << c << RESET;
        }else{
            std::cout << (g[idx].blocked ? '#' : ' ');
        }
    }
    std::cout << "\x1B[K\n\n"
              << "Legend: "
              << CYAN << "DFS" << RESET << "  " << GREEN << "BFS" << RESET << "  "
              << YELLOW << "GBFS" << RESET << "  " << MAGENTA << "A*" << RESET
              << "\x1B[K\n\x1B[J";
    std::cout.flush();
}

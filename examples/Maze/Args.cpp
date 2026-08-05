#include "Args.hpp"
#include "Ansi.hpp"
#include <iostream>
#include <stdexcept>
#include <string_view>

void printHelp(const char* prog){
    std::cout
        << "Usage: " << prog
        << " [text] [--seed N] [--randomness F] [--binomial F] [-w N] [-h N] [--help]\n\n"
        << "  [text]            Text to spell out in the maze. (default: \"Hello, World\")\n"
        << "  --seed N          RNG seed for reproducible runs. (default: current clock time)\n"
        << "  --randomness F    GrowingTree bias towards newest cell [0.0-1.0]. (default: random)\n"
        << "  --binomial F      GrowingTree binomial selection weight [0.0-1.0]. (default: random)\n"
        << "  -w N              Maze width in columns. (default: max(25, len(text) + 6))\n"
        << "  -h N              Maze height in rows. (default: 25)\n"
        << "  --help            Print this help message and exit.\n\n"
        << "Legend:\n"
        << "  " << CYAN    << "DFS"  << RESET << "   Depth-First Search\n"
        << "  " << GREEN   << "BFS"  << RESET << "   Breadth-First Search\n"
        << "  " << YELLOW  << "GBFS" << RESET << "  Greedy Best-First Search\n"
        << "  " << MAGENTA << "A*"   << RESET << "    A-Star\n";
}

ParsedArgs parseArgs(int argc, char** argv){
    ParsedArgs args;
    try{
        for(int i = 1; i < argc; ++i){
            std::string_view a = argv[i];
            auto next = [&]() -> std::string{
                if(++i >= argc){
					throw std::runtime_error("missing value for " + std::string(a));
				}
                return argv[i];
            };
            if(a == "--help"){ 
				printHelp(argv[0]); std::exit(0);
			}else if(a == "--randomness"){
				args.randomness = std::stof(next());
			}else if(a == "--binomial"){
				args.binomial   = std::stof(next());
			}else if(a == "--seed"){
				args.seed = static_cast<unsigned>(std::stoul(next()));
			}else if(a == "-w"){
				args.width = std::stoull(next());
			}else if(a == "-h"){
				args.height = std::stoull(next());
			}else if(!a.empty() && a[0] != '-'){
				args.text = std::string(a);
            }else{
				std::cerr << "Unknown option: " << a << "\n"; 
				printHelp(argv[0]);
			}
        }
    }catch(const std::exception& e){
        std::cerr << "Argument error: " << e.what() << "\n";
        printHelp(argv[0]);
    }
    return args;
}

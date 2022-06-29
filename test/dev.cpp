#define DEBUG false
#define TRANSPOSITION_TABLE false
#define RECURSIVE_LINE_SEARCH true
// this macro does not work


#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <dirent.h>

#include "common.h"
#include "heuristic.h"
#include "board.h"
#include "PNS.h"
#include "play.h"

#include "logger.h"
#include "node.h"
#include "parallel.h"

#include "sides.h"
Heuristic PNS::heuristic(ROW, COL);
#if ISOM
#include "canonicalorder.h"
CanonicalOrder PNS::isom_machine(ROW, COL);
#endif
Logger* PNS::logger;
Licit PNS::licit;
board_int Board::base_forbidden = PNS::heuristic.forbidden_all;

std::vector<Sol> sols = {
    Sol({4,6},{0,2},{1,3},{5,7}),
    Sol({4,6},{1,0},{3,7},{2,5}),
    Sol({4,6},{2,0},{1,3},{5,7}),
    Sol({4,6},{3,2},{0,1},{5,7}),
    Sol({4,6},{5,2},{1,3},{0,7}),
    Sol({4,6},{7,0},{1,3},{2,5}),
    Sol({5,2},{0,4},{1,6},{3,7}),
    Sol({5,2},{1,6},{3,4},{0,7}),
    Sol({5,2},{3,7},{1,4,6},{0}),
    Sol({5,2},{4,6},{1,3},{0,7}),
    Sol({5,2},{6,4},{1,3},{0,7}),
    Sol({5,2},{7,4},{1,6},{0,3}),
    Sol({6,1},{0,4},{7},{2,3,5}),
    Sol({6,1},{2,5},{0,7},{3,4}),
    Sol({6,1},{3,7},{4},{0,2,5}),
    Sol({6,1},{4,7},{3},{0,2,5}),
    Sol({6,1},{5,7},{3},{0,2,4}),
    Sol({6,1},{7,5},{0,2},{3,4}),
    Sol({7,2},{0,6},{1,3},{4,5}),
    Sol({7,2},{1,6},{3,4},{0,5}),
    Sol({7,2},{3,6},{0,1},{4,5}),
    Sol({7,2},{4,6},{1,3},{0,5}),
    Sol({7,2},{5,4},{1,6},{0,3}),
    Sol({7,2},{6,4},{0,1},{3,5}),
};

Board restrict_to_side(Board& base, const board_int restrict){
    base.white &= restrict;
    base.black &= restrict;
    base.forbidden_all &= restrict;
    return base;
}

void get_all_side_strat(std::vector<Sol>& sols, std::set<Board>& strats){
    for(auto& sol: sols){
        for(char side: {'A','B'}){
            for(board_int neigh: {PNS::heuristic.forbidden_fields_right | (1ULL << (ROW*COL-12))}){
                std::pair<Board, Board> key_val = sol.get_second_key_value(side);
                Board key = restrict_to_side(key_val.second, neigh);
                strats.insert(key);
            }
        }
    }
}

void init_left_side(Board& base, const Board& side){
    base.white |= side.white;
    base.black |= side.black;
}

void log_boards(std::string filename, std::set<Board> strats){
    std::ofstream log_file(filename.c_str());

    log_file<<"white black score_left score_right type common depth intersection\n";
    for(auto& b: strats){
        log_file<<b.white<<" "<<b.black<<" "<<b.score_left<<" "<<b.score_right<<" "<<(b.node_type == OR?0:1);
        log_file<<" "<<b.forbidden_all<<" "<<-1<<" "<<-1<<std::endl;
    }
    log_file.close();
}

std::set<Board> get_AB_strats(Sol& s1, std::set<Board>& strats_base, bool first){
    std::set<Board> strats;
    for(char side: {'A','B'}){
        std::pair<Board, Board> key_val = first?s1.get_first_key_value(side):s1.get_second_key_value(side);
        Board b = restrict_to_side(key_val.second, PNS::heuristic.forbidden_fields_left | (1ULL << 8));
        display(b, true);
        display(b.forbidden_all, true);

        for(auto& s: strats_base){
            
            Board temp(b);
            init_left_side(temp, s);
            strats.insert(temp);
            
            //display(s, true);
            //display(temp.forbidden_all, true);
        }
    }
    return strats;
}

void search_moves(Sol& s1, bool first){
    // === Read base strats ===
    std::set<Board> strats_base;
    get_all_side_strat(sols, strats_base);
    std::cout<<"Base strategies size: "<<strats_base.size()<<std::endl;

    // === Join and  log ===
    std::set<Board> strats = get_AB_strats(s1, strats_base, first);
    log_boards("../strats.txt", strats);
    printf("End startegies size: %zu\n", strats.size());
}

int main(int argc, char* argv[]){
    if(argc == 1) printf("Please give the mode (-mode MODE)\n");
    else{
        if((std::string) argv[1] == "move"){
            int att = std::stoi(argv[2]);
            int def = std::stoi(argv[3]);
            printf("First move (%d,%d)\n", att, def);
            std::pair<int, int> second;
            if(argc > 4){
                int att2 = std::stoi(argv[4]);
                int def2 = std::stoi(argv[5]);
                second = {att2, def2};
            }
            Sol s1({att,def},second,{},{});
            search_moves(s1, argc <= 4);
        }
        else if((std::string) argv[1] == "split"){
            printf("Split\n");
            int att = std::stoi(argv[2]);
            int def = std::stoi(argv[3]);
            int att2 = std::stoi(argv[4]);
            int def2 = std::stoi(argv[5]);
            
            bool read_B = false;
            std::vector<int> A,B;
            for(int i=7;i<argc;i++){
                if((std::string) argv[i] == "B"){
                    read_B = true;
                    continue;
                }
                if(read_B) B.push_back(std::stoi(argv[i]));
                else A.push_back(std::stoi(argv[i]));
            }
            printf("Read ok\n");
            Sol s1({att,def},{att2,def2},A,B);
            search_moves(s1, false);
        }
        else printf("Mode not found\n");
    }
    return 0;
}

#include "sides.h"


void move_steps_symmetrically(Board& board, int& player,
                std::vector<int> &white_actions, std::vector<int> &black_actions){
    for(auto f: white_actions){
        int r = f%ROW;
        int c = f/ROW;
        int symm_field = ((COL-1-c)*ROW+r);
        board.white |= (1ULL << f) | (1ULL << symm_field);
    }

    for(auto f: black_actions){
        int r = f%ROW;
        int c = f/ROW;
        int symm_field = ((COL-1-c)*ROW+r);
        board.black |= (1ULL << f) | (1ULL << symm_field);
    }
}

void move_B_side_symmetrically(Board& board, int& player,
                std::vector<int> &white_actions, std::vector<int> &black_actions){
    std::vector<int> w1(white_actions);
    std::vector<int> b1(black_actions);

    for(auto& f: w1){
        int r = f%ROW;
        int c = f/ROW;
        f = (1-c)*ROW+r;
    }

    for(auto& f: b1){
        int r = f%ROW;
        int c = f/ROW;
        f = (1-c)*ROW+r;
    }

    move_steps_symmetrically(board, player, w1, b1);
}

void init_A(Board& board){
    board.white |= (1ULL << 1) | (1ULL << (ROW*COL-3));
    board.white |= (1ULL << 2) | (1ULL << (ROW*COL-2));
}

void choose_type(Board& board, int type, std::vector<int>& w, std::pair<int,int> p){
    if(type == 0){
        board.forbidden_all = 0ULL;
    }
    else if(type == 1){
        w.push_back(p.first);
        board.forbidden_all = 0ULL;
    }
    else if(type == 2){
        w.push_back(p.second);
        board.forbidden_all = 0ULL;
    }
    else if(type == 3){

    }
}
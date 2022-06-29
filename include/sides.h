#pragma once
#include "board.h"

void move_steps_symmetrically(Board& board, int& player,
                std::vector<int> &white_actions, std::vector<int> &black_actions);

void move_B_side_symmetrically(Board& board, int& player,
                std::vector<int> &white_actions, std::vector<int> &black_actions);

void init_A(Board& board);

void choose_type(Board& board, int type, std::vector<int>& w, std::pair<int,int> p);

class SideStrategy;
class Sol{
    friend SideStrategy;

    std::pair<int,int> first_move,second_move;
    std::vector<int> A_side_give_up, B_side_give_up;
public:
    Sol(std::pair<int,int> f, std::pair<int,int> s, std::vector<int> A, std::vector<int> B): first_move(f),second_move(s),
        A_side_give_up(A), B_side_give_up(B){
    }

    int get_symm(int f){
        int r = f%ROW;
        int c = f/ROW;
        return ((COL-1-c)*ROW+r);
    }

    int get_B(int f){
        int r = f%ROW;
        int c = f/ROW;
        return (1-c)*ROW+r;
    }

    int get_symm_and_B(int f, char type, bool symm){
        if(type == 'A') return symm ? get_symm(f):f;
        else if(type == 'B') return symm ? get_symm(get_B(f)):get_B(f);  
        else assert(0);
    }

    void move_symm(std::pair<Board, Board>& key_val, char type, int white_move, int black_move){
        for(bool symm: {true, false}){
            int act = get_symm_and_B(white_move, type, symm);
            if(act >= 0 && act < ROW*COL) key_val.first.white |= (1ULL << act);
        }

        key_val.second = key_val.first;
        for(bool symm: {true, false}){
            int act = get_symm_and_B(black_move, type, symm);
            if(act >= 0 && act < ROW*COL) key_val.second.black |= (1ULL << act);
        }
    }

    std::pair<Board, Board> get_first_key_value(char type){
        std::pair<Board, Board> f;
        move_symm(f, type, first_move.first, first_move.second);
        f.second.forbidden_all = 0ULL;

        return f;
    }

    std::pair<Board, Board> get_second_key_value(char type){
        std::pair<Board, Board> f;
        // === First and second steps ===
        move_symm(f, type, first_move.first, first_move.second);
        f.first = f.second;
        move_symm(f, type, second_move.first, second_move.second);

        // === Share fields ===
        for(bool symm: {true, false}){
            for(auto w: A_side_give_up){
                if(type == 'A') f.second.white |= (1ULL << get_symm_and_B(w, type, symm));
                else f.second.forbidden_all ^= (1ULL << get_symm_and_B(w, type, symm));
                //(type == 'A' ? f.second.white : f.second.forbidden_all) |= (1ULL << get_symm_and_B(w, type, symm));
            }
            for(auto w: B_side_give_up){
                if(type == 'B') f.second.white |= (1ULL << get_symm_and_B(w, type, symm));
                else f.second.forbidden_all ^= (1ULL << get_symm_and_B(w, type, symm));
            }
        }

        f.second.forbidden_all &= f.second.get_valids();
        
        return f;
    }

    void init_side_symm(Board& board, char type, bool symm){
        board.move(get_symm_and_B(first_move.first, type, symm), 1);
        board.move(get_symm_and_B(first_move.second, type, symm), -1);
        board.move(get_symm_and_B(second_move.first, type, symm), 1);
        board.move(get_symm_and_B(second_move.second, type, symm), -1);
        
        board.forbidden_all &= ~board.get_valids();
        for(auto w: A_side_give_up){
            (type == 'A' ? board.white : board.forbidden_all) |= (1ULL << get_symm_and_B(w, type, symm));
        }
        for(auto w: B_side_give_up){
            (type == 'B' ? board.white : board.forbidden_all) |= (1ULL << get_symm_and_B(w, type, symm));
        }

        //if(type == 'A') for(auto w: A_side_give_up) board.white |= (1ULL << get_symm_and_B(w, type, symm));
        //else if(type == 'B') for(auto w: B_side_give_up) board.white |= (1ULL << get_symm_and_B(w, type, symm));
        //else assert(0);
    }

    void init_side_AB(Board& board, char type){
        init_side_symm(board, type, false);
        init_side_symm(board, type, true);

        //board.forbidden_all = 0ULL;
    }
};

/*
namespace sol1{
Sol step1({6,1},{7,5},{2,0},{3,4});
Sol step2({6,1},{2,5},{0,7},{3,4});
Sol step3({6,1},{3,7},{4},{5,2,0});
Sol step4({6,1},{5,7},{},{0,2,4,3});
Sol step5({6,1},{0,5},{7,2},{3,4});
Sol step6({6,1},{4,7},{},{0,2,3,5});
};

namespace sol2{
Sol step1({4,1},{6,7},{3},{0,2,5});
Sol step2({4,1},{2,5},{0,7},{3,6});
Sol step3({4,1},{3,5},{0,2},{6,7});
Sol step4({4,1},{7,5},{0,2},{3,6});
Sol step5({4,1},{0,5},{2,3},{6,7});
Sol step6({4,1},{5,2},{},{}); // TODO: 'A' can move free
};

namespace sol3{
Sol step1({5,2},{4,6},{1,3},{0,7});
Sol step2({5,2},{0,4},{1,6},{3,7});
Sol step3({5,2},{1,6},{3,4},{0,7});
Sol step4({5,2},{6,4},{3,1},{7,0});
Sol step5({5,2},{3,6},{4,1},{0,7});
Sol step6({5,2},{7,4},{1,6},{0,3});
};

namespace sol4{
Sol step1({7,5},{0,2},{1,4,6},{3});
Sol step2({7,5},{4,2},{1,6,0},{3});
Sol step3({7,5},{2,3},{1,6,4},{0});
Sol step4({7,5},{6,1},{2,0},{3,4});
Sol step5({7,5},{3,1},{0,2},{4,6});
Sol step6({7,5},{1,6},{},{});
};
*/
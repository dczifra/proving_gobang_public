#pragma once

#include <array>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <cfloat>
// ============================================
//                BOARD INFOS
// ============================================
const unsigned int ROW=4;
const unsigned int COL=10;
const unsigned int ACTION_SIZE = ROW*COL;

#define LAYERNUM 7

#define BITSIZE 64
#define board_int std::uint64_t

#define LOG false

#define EVAL_TRESHOLD 5
#define ISOM false
#define HEURISTIC true
#define PNS2_START 250000000
#define LOG_CUT_DEPTH 2
#define SIDE_STRAT false

#if HEURISTIC
    #define var float
    #define var_MAX FLT_MAX
#else
    #define var unsigned int
    #define var_MAX UINT_MAX
#endif

unsigned long long set_full_board();

const unsigned long long FULL_BOARD = set_full_board();

// The number of bits need to be moved back after flip board:
const unsigned int FLIP_SIZE = BITSIZE-ACTION_SIZE;
const board_int flip_base = ((1ULL << 0) | (1ULL <<5) | (1ULL <<10) | (1ULL <<15) | (1ULL <<20) | (1ULL <<25) | (1ULL <<30) | (1ULL <<35) | (1ULL <<40) | (1ULL <<45));
// ============================================
//                  DISPLAY
// ============================================

//void print_v(std::vector<int>& v);
struct Board;
void display(const Board board, bool end, std::vector<int> show = std::vector<int>(), bool nocolor=false);
void display(const std::vector<Board> board, bool end, std::vector<int> show = std::vector<int>(), bool nocolor=false);
void display(const board_int board, bool end,  std::vector<int> show = std::vector<int>(), bool nocolor=false);

// ============================================
//                   ARGS
// ============================================
struct Args{
    bool log = false;
    bool talky = true;
    bool play = false;
    bool test = false;
    bool disproof = false;
    bool show_lines = false;
    bool PNS_square = false;
    bool parallel = false;
    bool generate_parallel = false;
    bool merge_solutions = false;
    float A = 200000000000.0;
    float B = 15000000000.0;
    int potencial_n = 128;
    int START=-1;
    std::string get_filename();
    std::string type = "Base";
    Args(int argc, char* argv[]);
    Args(){}
};

inline int clip(int n, int lower, int upper){
    return n <= lower ? lower : n >= upper ? upper : n;
}

// ============================================
//      Line infos (Shouldn't be in heuristic)
// ============================================
struct Line_info{
    unsigned int index;
    unsigned int size;
    board_int line_board;
    std::vector<int> points;
};
unsigned int selectBit(const board_int v, unsigned int r);
unsigned int flip_bit(board_int val);

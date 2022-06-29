#pragma once
#include<iostream>

#include "common.h"
#include "board.h"
#include "PNS.h"

#include <map>
#include <tuple>

struct Args;

class Play{
public:
    Play(std::string filename, bool disproof, bool talky, Args* args_);

// === Public functions ===
    void play_with_solution_split();
    std::tuple<Board, int, int> play_with_solution(Board base_board, int row, int col);

    void set_human_player(int p){ human_player = p;}
    static void read_solution(std::string filename, PNS& mytree);
    static NodeType choose_problem(Board& board, int& player, Args* args);



    
/// === Private Functions ===
private:
    int move_human();
    void build_tree();
    void build_node(Board b);
    void build_node2(PNSNode* node);
    void build_licit_node(const Board& b, int action);
    

    Board move_in_solution(int i, int& act, std::vector<int>& color);


// === Private Variables ===
    PNS tree;
    int human_player, player;
    std::map<std::vector<uint64_t>, PNSNode*> isom_map;

    bool talky;
    Args* args;
};

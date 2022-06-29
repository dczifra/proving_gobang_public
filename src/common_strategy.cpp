#include "PNS.h"
#include "node.h"
#include "common_strategy.h"
#include "common.h"

#define has(action, board) ((1ULL << action) & board)

GeneralCommonStrategy::GeneralCommonStrategy(PNS* tree_) : CommonStrategy(tree_),
                                    side_strategy({tree_->heuristic.forbidden_fields_left, tree_->heuristic.forbidden_fields_right }){
    //std::cout<<" >> GeneralCommonStrategy"<<std::endl;
}

Board SideStrategy::restrict_to_side(Board& base, const board_int restrict){
    base.white &= restrict;
    base.black &= restrict;
    return base;
}

SideStrategy::SideStrategy(std::vector<Sol> sol_v, std::pair<board_int, board_int> neighs){
    for(auto s: sol_v){
        for(char side: {'A','B'}){
            for(board_int neigh: {neighs.first, neighs.second}){
                std::pair<Board, Board> key_val = s.get_first_key_value(side);
                Board key = restrict_to_side(key_val.first, neigh);
                key.node_type = AND;
                key.forbidden_all = 0ULL;
                first.insert({key, restrict_to_side(key_val.second, neigh)});
                
                key_val = s.get_second_key_value(side);
                key = restrict_to_side(key_val.first, neigh);
                key.node_type = AND;
                key.forbidden_all = 0ULL;
                second.insert({key, restrict_to_side(key_val.second, neigh)});

                //display(key_val.second, true);
            }
        }
    }

    for(auto p: first){
        //display(p.first, true);
        //std::cout<<p.first.white<<" "<<p.first.black<<" "<<(p.first.node_type == AND)<<" "<<p.first.forbidden_all<<std::endl;
    }
}

std::vector<Sol> sols = {
};

SideStrategy::SideStrategy(std::pair<board_int, board_int> neighs):SideStrategy(sols, neighs){

}

Node* CommonStrategy::add_or_create(const Board& board){
    PNSNode* node = tree->get_states(board);
    //display(board, true);
    if (node == nullptr){
        node = new PNSNode(board, tree->args);
        tree->add_board(board, node);
    }
    else{
        node->parent_num += 1;
    }
    return node;
}

void deactivate_line(Board& board, int line){
    board.white &= ~(1ULL << line);
    board.black |= (1ULL << line);
}

void activate_line(Board& board, int line){
    board.black &= ~(1ULL << line);
    board.white |= (1ULL << line);
}

Node *GeneralCommonStrategy::choose_from(const Board &board, std::vector<int> actions, NodeType type){

}

void move_2_line(Board& act_board, int action){

}

void move_to_other(Board& act_board, int action){

}

void split_2_line(Board& act_board, int action){

}

Node* GeneralCommonStrategy::no_move_and_get_others(Board& act_board, int action, board_int side){

}

Node* GeneralCommonStrategy::move_free_and_give_up_others(Board& act_board, int action, board_int side){
}

Node* GeneralCommonStrategy::answer_to_neighbouring_fields(Board& act_board, int action, board_int side){

}

Node* GeneralCommonStrategy::sideA_startegy(Board &act_board, const int action, board_int& side){

}

void give_up(Board& board, std::vector<int> w){
    for(auto f: w){
        board.white |= (1ULL << f);
    }
}

Node* GeneralCommonStrategy::move_on_common(const Board &b, int action){
    assert(0);
}

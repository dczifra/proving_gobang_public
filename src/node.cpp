#include "node.h"
#include "heuristic.h"
#include "PNS.h"

// =======================
inline int get_child_num(const Board& b){
    return __builtin_popcountll(b.get_valids_without_ondegree(PNS::heuristic.all_linesinfo));
}

PNSNode::PNSNode(const Board& b, Args* args): Node(get_child_num(b)), board(b){
    type = b.node_type;

    heuristic_value = board.heuristic_value(PNS::heuristic.all_linesinfo);
    
    int valid_moves = board.get_valids_num();
    float log_odds = -4.63230495 - 9.67572108 * board.node_type - 0.87216265 * valid_moves + 17.23691808 *heuristic_value;
    float prob = 1 / (1 + exp(-log_odds));
    if(b.white_win(PNS::heuristic.all_linesinfo)){
        pn = 0;
        dn = var_MAX;
    }
    else if((b.node_type == AND) && b.heuristic_stop(PNS::heuristic.all_linesinfo)){
        pn = var_MAX;
        dn = 0;
    }
    // === Init proof and disploof number ===
    else if(child_num == 0){ // Game is over, black wins
        pn = var_MAX;
        dn = 0;
    }
    //else if(prob<0.1){
    //    pn = var_MAX;
    //    dn = 0;
    //}
    //else if((b.node_type == AND) && (b.heuristic_value(heuristic.all_linesinfo)*128 < args->potencial_n-0.000000001)){
    //    pn = var_MAX;
    //    dn = 0;
    //}
    else{
        init_pn_dn();
    }

    parent_num = 1;
}

void inline PNSNode::init_pn_dn(){
#if HEURISTIC
    int valid_moves = board.get_valids_num();
    float log_odds = -4.63230495 - 9.67572108 * board.node_type - 0.87216265 * valid_moves + 17.23691808 *heuristic_value;
    float prob = 1 / (1 + exp(-log_odds));
    float penalty = (1-prob) * 10;
    pn = (type==OR ? 1+penalty:child_num*(1+penalty));
    dn = std::pow(1000, heuristic_value);
#else
    pn = (type==OR ? 1:child_num);
    dn = (type==AND ? 1:child_num);
#endif
}

InnerNode::InnerNode(int childnum, NodeType t): Node(childnum){
    type = t;

    if(type == OR){
        pn = 1;
        dn = childnum;
    }
    else{
        pn = childnum;
        dn = 1;
    }

    extended = true;
    parent_num = 1;
}

void AttackerOnForbidden::extend_all(){
    assert(0);
}

AttackerOnForbidden::AttackerOnForbidden(PNS* tree, const Board& act_board, int action): InnerNode(2, OR){
    //children[0] = new LicitSwitchNode(tree, act_board, action, get_licit_limit(tree, act_board, action));
    children[0] = get_defender_side(tree, act_board, action);
    children[1] = add_neighbour_move(tree, act_board, action);
    tree->update_node(this);
}

LicitSwitchNode::LicitSwitchNode(PNS* tree, const Board& act_board, int action, int licit_limit): InnerNode(licit_limit+1, AND){
    for(int licit_value=0; licit_value<= licit_limit;licit_value++){
        children[licit_value] = new LicitNode(tree, act_board, action, licit_value);
    }
}

LicitNode::LicitNode(PNS* tree, const Board& act_board, int action, int licit_value): InnerNode(2, OR){
    // For all licitnode set Accept and Reject node
    bool is_left = (1ULL << action) & tree->heuristic.forbidden_fields_left;
    // 1. Reject
    Board reject(act_board);
    reject.node_type = OR;
    if(is_left) reject.score_left += (licit_value+tree->licit.licit_diff);
    else reject.score_right += (licit_value+tree->licit.licit_diff);

    PNSNode* rej = tree->get_states(reject);
    handle_collision(tree, rej, reject);
    children[0] = rej;

    // 2. accept
    Board accept(act_board);
    accept.node_type = AND;
    if(is_left) accept.score_left -= licit_value;
    else accept.score_right -= licit_value;

    PNSNode* acc = tree->get_states(accept);
    handle_collision(tree, acc, accept);
    children[1] = acc;
}

bool Node::is_empty_side(int action, const Board& b){
    bool is_left = (1ULL << action) & PNS::heuristic.forbidden_fields_left;
    if(is_left){
        if(((b.white) & PNS::heuristic.forbidden_fields_left) == 0){
        //if(((b.white | b.black) & PNS::heuristic.forbidden_fields_left) == 0){
            return true;
        }
        else return false;
    }
    else{
        if(((b.white | b.black) & PNS::heuristic.forbidden_fields_right) == 0){
            return true;
        }
        else return false;
    }
}

// === Helper Functions ===
PNSNode* Node::get_defender_side(PNS* tree, const Board& act_board, int action){
    bool is_left = (1ULL << action) & PNS::heuristic.forbidden_fields_left;
    bool inner = (1ULL << action) & PNS::heuristic.forbidden_fields_inner;
    int score = is_left ? act_board.score_left : act_board.score_right;
    
    Board next_state(act_board);
    // ======================================================
    //                 TAKE CARE HARD CODED !!!!
    // ======================================================
    // If attacker moves first to common, there is a common strategy:
    //    defender moves to the other forbidden field in the column,
    //    and the score will be 0
    // TODO: what if the previous attacker sign was deleted?
    int diff = PNS::licit.max_score;
    int base_score = 1;
    if(score > 0 or (score == 0 && is_left)){
        next_state.node_type = AND;
        is_left ? next_state.score_left = -base_score : next_state.score_right = -base_score;
    }
    else{
        next_state.node_type = OR;
        is_left ? next_state.score_left = base_score : next_state.score_right = base_score;
    }

    nullify_scores(next_state);
    PNS::simplify_board(next_state);

    PNSNode* neigh = tree->get_states(next_state);
    handle_collision(tree, neigh, next_state);
    return neigh;
}

PNSNode* Node::add_neighbour_move(PNS* tree, const Board& act_board, int action){
    int licit_max = tree->licit.max_score;
    bool is_left = (1ULL << action) & tree->heuristic.forbidden_fields_left;

    Board neighbour_move(act_board);
    // Change action field from white to black
    neighbour_move.white &= !(1ULL << action);
    int reward = is_empty_side(action, neighbour_move) ? 0 : PNS::licit.cover_forbiden_reward;
    //display(neighbour_move, true);
    //std::cout<<reward<<std::endl;
    //int reward = 0;
    neighbour_move.move(action, -1);
    neighbour_move.node_type = OR;//!!!
    if(is_left){ // reduce score, and clip (-MAXS_CORE, MAX_SCORE)
        neighbour_move.score_left -= reward;
        neighbour_move.score_left = clip(neighbour_move.score_left, -licit_max, licit_max);
    }
    else{
        neighbour_move.score_right -= reward;
        neighbour_move.score_right = clip(neighbour_move.score_right, -licit_max, licit_max);
    }
    nullify_scores(neighbour_move);
    PNS::simplify_board(neighbour_move);

    PNSNode* neigh = tree->get_states(neighbour_move);
    handle_collision(tree, neigh, neighbour_move);
    return neigh;
}

int Node::get_licit_limit(PNS* tree, const Board& act_board, int action){
    int licit_limit;
    bool is_left=false;
    if(tree->heuristic.forbidden_fields_left & (1ULL << action)){
        licit_limit = std::max(act_board.score_left+2, 0);
        is_left = true;
    }
    else{
        licit_limit = std::max(act_board.score_right+2, 0);
    }
    return licit_limit;
}

void Node::handle_collision(PNS* tree, PNSNode*& node, const Board& board){
    if(node == nullptr){
        node = new PNSNode(board, tree->args);
        tree->add_board(board, node);
    }
    else node->parent_num++;
}

void Node::nullify_scores(Board& board){
    // if this is the last forbidden square, set score to 0
    if(((board.black | board.white) & PNS::heuristic.forbidden_fields_left) == PNS::heuristic.forbidden_fields_left){
        board.score_left = 0;
        //display(board, true);
    }
    if(((board.black | board.white) & PNS::heuristic.forbidden_fields_right) == PNS::heuristic.forbidden_fields_right){
        board.score_right = 0;
        //display(next_state, true);
    }
}

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

Args::Args(int argc, char* argv[]){
    int i=0;
    while(i<argc){
        if((std::string) argv[i] == "--log") log = true;
        else if((std::string) argv[i] == "--play") play = true;
        else if((std::string) argv[i] == "--quiet") talky = false;
        else if((std::string) argv[i] == "--test") test = true;
        else if((std::string) argv[i] == "--disproof") disproof = true;
        else if((std::string) argv[i] == "--lines") show_lines = true;
        else if((std::string) argv[i] == "--parallel") parallel = true;
        else if((std::string) argv[i] == "--generate_parallel") generate_parallel = true;
        else if((std::string) argv[i] == "--merge_solutions") merge_solutions = true;
        else if((std::string) argv[i] == "-start"){
            START = std::stoi(argv[++i]);
        }
        else if((std::string) argv[i] == "-type"){
            type = std::string(argv[++i]);
        }
        else if((std::string) argv[i] == "-potencial_n"){
            potencial_n = std::stoi(argv[++i]);
        }
        else if((std::string) argv[i] == "--PNS2") PNS_square = true;
        else if((std::string )argv[i] == "--help"){
            std::cout<<"Help for AMOBA\nARGS:\n";
            std::cout<<"--play: Play with tree\n";
            std::cout<<"--log: log root PN and DN\n";
            std::cout<<"--test: Tets mode, you can play the solution\n";
        }
        i++;
    }
}

std::string Args::get_filename(){
    std::string folder = (disproof ? "data/disproof/" : "data/proof/");
    if(!SIDE_STRAT) folder+="old/";
    std::string filename =  std::to_string(ROW)+"x"+std::to_string(COL)+".csv";
    return folder + filename;
}

void add_proven_nodes(PNS& tree, std::string folder){
    //std::cout<<"";
    DIR *dir;
    struct dirent *ent;
    if((dir=opendir(folder.c_str())) != NULL){
        while((ent=readdir(dir)) != NULL){
            if(ent->d_name[0] == 'c'){
                std::string filename = folder + "/"+(std::string) ent->d_name;
                Play::read_solution(filename, tree);
                std::cout<<ent->d_name<<" processed\n";
            }
        }
    }
}

std::map<std::pair<board_int,board_int>,bool>  get_proven_boards(std::string folder){
    std::map<std::pair<board_int,board_int>, bool> solved;
    DIR *dir;
    struct dirent *ent;
    if((dir=opendir(folder.c_str())) != NULL){
        while((ent=readdir(dir)) != NULL){
            if(ent->d_name[0] != '.'){
                std::string s(ent->d_name);
                std::string delimiter = "_";
                std::string white = s.substr(0, s.find(delimiter));
                
                s = s.substr(s.find(delimiter)+1, s.size());
                std::string black = s.substr(0, s.find(delimiter));
                //std::cout<<white<<" "<<s<<" "<<black<<std::endl;
                board_int w = stoul(white), b = stoul(black);
                //std::cout<<w<<" "<<b<<std::endl;
                solved[{w,b}]=true;
            }
        }
    }
    return solved;
}



void eval_child(Node* node, PNS& tree, Args& args){
    if(!node->extended) tree.extend_all((PNSNode*) node, false);
    for(int i=0; i<node->children.size(); i++){
        std::cout<<"Child "<<i<<std::endl;
        if(!node->children[i]->is_inner()) display(node->children[i]->get_board(), true);
        tree.evaluate_node_with_PNS_square(node->children[i], args.log, false);
        tree.stats(node->children[i], true);
        PNS::logger->log_node(node->children[i],
                              "data/final/child_"+std::to_string(i)+".sol");
        tree.delete_all(node->children[i]);
    }
}

void eval_all_OR_descendents(Node* node, PNS& tree, Args& args, PNS& sol, int depth,
                             std::map<std::pair<board_int,board_int>, bool>& solved){
    if(!node->extended) tree.extend_all((PNSNode*) node, false);

    for(int i=0;i<node->children.size();i++){
        //std::cout<<"I "<<i<<std::endl;
        Node* child = node->children[i];
        if(child==nullptr) assert(0);

        Board act_board(child->get_board());
        if(solved.find({act_board.white, act_board.black})!=solved.end()) continue;
        else if(child->is_inner() || (child->type == OR && depth < 2)){
            eval_all_OR_descendents(child, tree, args, sol, depth+1, solved);
        }
        else{
            assert(!child->is_inner());
            const Board act_board(child->get_board());
            tree.evaluate_node_with_PNS(child, args.log, false);
            tree.stats(child, true);
            display(act_board, true);
            PNS::logger->log_node(child,
                                  "data/board_sol/"+act_board.to_string()+".sol");
        }
        tree.copy_states(sol);
        tree.delete_all(child);
        sol.copy_states(tree);
        std::cout<<"Sol size: "<<sol.get_states_size()<<std::endl;
        node->children[i]=nullptr;
    }
}

void PNS_test(Args& args){
    Board b;
    int player = 1;
    Play::choose_problem(b,player, &args);
    if(args.show_lines){
        display(b, true);
    }
    
    PNS tree(&args);
    //add_proven_nodes(tree, "../data/final");
    PNSNode* node = new PNSNode(b, &args);
    std::cout<<"Root node heuristic value: "<<node->heuristic_value<<std::endl;

    tree.init_PN_search(node);
    // === Eval all children first ===
    tree.extend_all(node, false);
    //node = (PNSNode*)node->children[1];
    // ============================================
    
    if(args.PNS_square){
        std::cout<<"PNS2"<<std::endl;
        tree.evaluate_node_with_PNS_square(node, args.log, false);
    }
    else{
        tree.evaluate_node_with_PNS(node, args.log, false);
    }
    
    tree.stats(node, true);
    PNS::logger->log_node(node, args.get_filename());

    tree.delete_all(node);
    tree.stats(nullptr, true);
    // tree.component_stats();
    std::cout<<"Nodes visited during search: "<<tree.total_state_size<<std::endl;
}

Heuristic PNS::heuristic(ROW, COL);
#if ISOM
#include "canonicalorder.h"
CanonicalOrder PNS::isom_machine(ROW, COL);
#endif
Logger* PNS::logger;
Licit PNS::licit;
board_int Board::base_forbidden = PNS::heuristic.forbidden_all;

int main(int argc, char* argv[]){
    Args args(argc, argv);
    std::string spam = (COL > 9 ? "#" : "");
    printf(" #%s%s\n",spam.c_str(), std::string(2*COL,'#').c_str());
    printf(" #%sBoard %dx%d%s#\n", std::string(COL-5,' ').c_str(), ROW, COL, std::string(COL-5,' ').c_str());
    printf(" #%s%s\n",spam.c_str(), std::string(2*COL,'#').c_str());
    if(args.show_lines) for(auto line : PNS::heuristic.all_linesinfo) display(line.line_board, true);
    
    PNS::logger = new Logger();
    PNS::logger->init(args.disproof);

    if(args.parallel){
        prove_node(args);
    }
    else if(args.generate_parallel){
        generate_roots_descendents(args, 3);
    }
    else if(args.merge_solutions){
        merge_solutions(args, args.get_filename());
    }
    else if(args.test){
        //Play game("data/board_sol/36318243455051_581091895297588_0_0.sol", args.disproof, args.talky, &args);
        Play game(args.get_filename(), args.disproof, args.talky, &args);
        game.play_with_solution_split();
    }
    else{
        //DFPNS_test(args);
        PNS_test(args);
    }
    return 0;
}

#include "PNS.h"
#include "logger.h"
#include <iostream>

#include "canonicalorder.h"
#include "parallel.h"
// === Compile ===
// g++ -o main nauty.cpp ../nauty27r1/nauty.c ../nauty27r1/nautil.c ../nauty27r1/naugraph.c ../nauty27r1/schreier.c ../nauty27r1/naurng.c 
// TODO: set putstring second argument to const in nauty (for no warnings)!!!

// Questions:
//   * Free after malloc?
//   * global graph (avoid malloc all the time)
void init_graph(){
    // === Basic variables ===
    set s[MAXM];  /* a set */
    int xy[MAXN];  /* an array */
    graph g1[MAXN*MAXM], cg1[MAXN*MAXM];  /* a graph */
    graph g2[MAXN*MAXM], cg2[MAXN*MAXM];  /* a graph */

    // === (2. option) Init arrays  dinamically ===
    //DYNALLSTAT(set,s,s_sz);
    //DYNALLSTAT(graph,g,g_sz);
    //DYNALLSTAT(int,xy,xy_sz);
    //DYNALLOC1(int,lab,lab_sz,n,"malloc");

    int lab1[MAXN],ptn1[MAXN],orbits1[MAXN];
    int lab2[MAXN],ptn2[MAXN],orbits2[MAXN];
    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;
    int n,m,v;

    //options.writeautoms = TRUE;
    options.getcanon = TRUE;
    options.defaultptn = TRUE;

    n = 6;
    m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    for(int i=0;i<n;i++){
        lab1[i] = i;
        lab2[i] = i;
        ptn1[i] = 1;
        ptn2[i] = 1;
    }
    ptn1[n-1] = 0;
    ptn2[n-1] = 0;

    // === G1 ===
    EMPTYGRAPH(g1,m,n);
    ADDONEEDGE(g1,0,3,m);
    ADDONEEDGE(g1,1,3,m);
    ADDONEEDGE(g1,1,4,m);
    ADDONEEDGE(g1,2,3,m);
    ADDONEEDGE(g1,2,4,m);

    // === G2 ===
    EMPTYGRAPH(g2,m,n);
    ADDONEEDGE(g2,0,3, m);
    ADDONEEDGE(g2,0,4,m);
    ADDONEEDGE(g2,1,4,m);
    ADDONEEDGE(g2,2,4,m);

    densenauty(g1,lab1,ptn1,orbits1,&options,&stats,m,n,cg1);
    densenauty(g2,lab2,ptn2,orbits2,&options,&stats,m,n,cg2);

    printf("Automorphism group size = ");
    writegroupsize(stdout,stats.grpsize1,stats.grpsize2);
    printf("\n");

    printf("Canonical labeling:\n");
    for(int i=0;i<m*n;i++){
        std::cout<<cg1[i]<<" ";
    }
    std::cout<<std::endl;

    for(int i=0;i<m*n;i++){
        std::cout<<cg2[i]<<" ";
    }
    std::cout<<std::endl;
}

void convert_board(const Board& b){
    PNS tree(nullptr);
    //std::cout<<(ROW*COL+tree.heuristic.all_linesinfo.size())<<" "<<MAXN<<std::endl;
    int nodes = b.get_valids_num();
    int n = nodes+b.get_active_line_num(tree.heuristic.all_linesinfo);
    int m = SETWORDSNEEDED(n);
    int index[ROW*COL];
    int sum = 0;
    for(int i=0;i<ROW*COL;i++){
        if(b.is_valid(i)) index[i] = (sum++);
        else index[i]=-1;
    }

    // === Declare variables ===
    //DYNALLSTAT(graph,g,g_sz);
    std::cout<<n<<" "<<nodes<<" "<<b.get_active_line_num(tree.heuristic.all_linesinfo)<<std::endl;
    graph* g = new graph[MAXM*MAXN];
    DYNALLSTAT(graph,cg,cg_sz);
    int lab[MAXN],ptn[MAXN],orbits[MAXN];
    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;
    options.getcanon = TRUE;
    
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    // === INIT nodes and edges ===
    //DYNALLOC2(graph,g,g_sz,m,n,"malloc");
    DYNALLOC2(graph,cg,cg_sz,m,n,"malloc");
    EMPTYGRAPH(g,m,n);
    printf("%d %d\n", n, m);

    int line_ind = 0;
    for(auto line: tree.heuristic.all_linesinfo){
        bool is_free = !(line.line_board & b.black);
        if(is_free){
            for(auto field : line.points){
                if(b.is_valid(field)){
                    //printf("%d %d\n", nodes+line_ind, index[field]);
                    ADDONEEDGE(g,nodes+line_ind,index[field],m);
                }
            }
            line_ind++;
        }
    }

    // === INIT colors ===
    for(int i=0;i<n;i++){
        ptn[i] = 1;
        lab[i] = i;
    }
    ptn[nodes-1] = 0; // 1. color nodes
    ptn[nodes-3] = 0; // 2. color edges
    ptn[n-1] = 0;     // 3. color OR/AND bit


    // === The labeling ===
    densenauty(g,lab,ptn,orbits,&options,&stats,m,n,cg);
    printf("Canonical labeling:\n");
    for(int i=0;i<n;i++){
        std::cout<<cg[i]<<" ";
    }
    std::cout<<std::endl;
    std::cout<<"End"<<std::endl;

    delete[] g;
}

Heuristic PNS::heuristic(ROW, COL);
#if ISOM
CanonicalOrder PNS::isom_machine(ROW, COL);
#endif
Logger* PNS::logger = new Logger();
Licit PNS::licit;
board_int Board::base_forbidden = PNS::heuristic.forbidden_all;

#define mymap std::unordered_map<std::vector<uint64_t>, PNSNode *, Vector_Hash>
//#define mymap std::unordered_map<Board, PNSNode*, Board_Hash>

void read_from_file(int n, std::string filename,
                    CanonicalOrder &isom_machine,
                    mymap &states){
    Board b;
    Args args;
    int depth, times;

    std::ifstream f(filename.c_str());

    std::string temp;
    for (int i=0; i<n; i++){
        if (i<1){
            std::getline(f, temp);
            std::cout << temp << std::endl;
            continue;
        }

        f >> b.white >> b.black >> b.score_left >> b.score_right >> b.node_type;
        f >> b.forbidden_all >> depth >> times;

        //display(b, true);

        std::vector<uint64_t> isom = isom_machine.get_canonical_graph(b, PNS::heuristic.all_linesinfo);
        if (states.find(isom) == states.end()){
            states[isom] = new PNSNode(b, &args);
        }
    }

    std::cout << "Map size: " << states.size() << std::endl;
}

void create_isom_states(){
    CanonicalOrder isom_machine(ROW, COL);
    mymap states;

    char *argv[0];
    Args args;
    generate_roots_descendents(args, 3);

    read_from_file(16976 + 2, "../ors.txt", isom_machine, states);
    read_from_file(729 + 2, "../ands.txt", isom_machine, states);

    std::string log_filename = "../isom.txt";
    std::ofstream log_file(log_filename);
    log_file<<"white black score_left score_right type common depth intersection\n";
    for(auto& p: states){
        const Board b(p.second->get_board());
        //display(b, true);
        log_file<<b.white<<" "<<b.black<<" "<<b.score_left<<" "<<b.score_right<<" "<<(b.node_type == OR?0:1);
        log_file<<" "<<b.forbidden_all<<" "<<-1<<" "<<-1<<std::endl;
    }
    log_file.close();
}



int main(){
    std::cout<<"###################\n";
    printf("# Nauty           #\n#  * Version: %d  #\n", 27);
    printf("#  * MAXN: %d     #\n#  * MAXM: %d      #\n", MAXN, MAXM);
    std::cout<<"###################\n\n";

    create_isom_states();


    return 0;
}

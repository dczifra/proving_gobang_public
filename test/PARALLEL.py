from subprocess import Popen, STDOUT, PIPE

import os
import sys
import time
import random
import multiprocessing
import subprocess

import resource
import argparse

def get_args():
     import argparse
     parser = argparse.ArgumentParser(description='Process input parameters')
     parser.add_argument('-mode', dest='mode', type=str, default="base",
                     help='Mode type')
     parser.add_argument('-procnum', dest='procnum', type=int, default=15,
                     help='Number of parallel cores')
     parser.add_argument('-memlimit', dest='memlimit', type=float, default=10,
                     help='Memory limit')
     parser.add_argument('-moves', dest='moves', type=int, nargs="+", default=10,
                     help='Initial moves')     
     args = parser.parse_args()
     print(args)
     return args

def get_lic_virt_mem(ulimit):
    def limit_virtual_memory():
        resource.setrlimit(resource.RLIMIT_AS, (ulimit*(1024**3), resource.RLIM_INFINITY))
    return limit_virtual_memory

def run_board(b, ulimit):
    p = Popen(["./AMOBA","--parallel"], preexec_fn=get_lic_virt_mem(ulimit),
              stdout=PIPE, stdin=PIPE, stderr=STDOUT, bufsize=1, universal_newlines=True)

    out,err = p.communicate(b)

    if(len(out.split('PN'))==1): return "fail",b
    if(out.split('PN: ')[1][0]=='0'): return "PN",b
    elif(out.split('DN: ')[1][0]=='0'): return "DN",b
    else: return "fail",b

def print_res(arg):
    result,b = arg
    log[result]+=1
    if(result == "PN"):
        print("Proof", b)
    elif(result == "fail"):
        #print("Fail", b)
        log["again"].append(b)
    print("\r{}/{} [failed: {}] [proof: {}]".format(log["DN"], log["all"], log["fail"], log["PN"]), flush=True, end=" ")

def save_result(arg):
    result,b = arg
    log[result]+=1
    if(result == "fail"):
        log["again"].append(b)

def run_all_board(boards, procnum, memory_limit, do_log=True):
    log["all"] = len(boards)
    pool = multiprocessing.Pool(processes=procnum)
    for b in boards:
        if(do_log): r = pool.apply_async(run_board, args =(b,memory_limit), callback=print_res)                                                                                  
        else: r = pool.apply_async(run_board, args =(b,memory_limit), callback=save_result)                                                                                  
    pool.close()
    pool.join()

global log
log={
    "PN":0,
    "DN":0,
    "fail":0,
    "all":None,
    "again":[],
}

def init_log():
    log["PN"]=0
    log["DN"]=0
    log["fail"]=0
    log["all"]=0
    log["again"]=[]

def runfile(filename, procnum, memlimit, do_log = True):
    with open(filename, "r") as file:
        boards = file.read().split('\n')
        boards = boards[1:-1]
        random.shuffle(boards)
        if do_log:
             print(filename, len(boards))
    

    run_all_board(boards, procnum, memlimit, do_log = do_log)
    if do_log:
         print("Summary: {}/{} [failed: {}] [proof: {}]".format(log["DN"], log["all"],
                                                           log["fail"], log["PN"]))
    with open(filename+".fail", "w") as f:
        for b in log["again"]:
            f.write(b+"\n")

def strat1(args, att1, do_log = True):
    res = {}
    for def1 in range(8):
        if(def1 != att1):
            print(f'\r {att1} {def1}', end=' ', flush=True)
            os.system(f"./DEV move {att1} {def1} > /dev/null")
            runfile("../strats.txt", args.procnum, args.memlimit, do_log=False)
            res[(att1, def1)] = (log["PN"], log["fail"], log["DN"])
            init_log()
    
    if(do_log): print("\nPN fail DN")
    sorted_strats = dict(sorted(res.items(), key = lambda l: l[1]))
    for k,v in sorted_strats.items():
        if(do_log): print(k, "==>", v)

    return next(iter(sorted_strats.items()))

def strat2(args, att1, def1):
     strat = {}
     for att2 in list(range(8))+[-4,8]:
          if(att2 in [att1, def1]): continue

          def_strat = {}
          for def2 in list(range(8))+[-4,8]:
               if (def2 not in [att1, def1, att2]):
                    p_log = f"Strat {att2} {def2}"
                    print(p_log, end=' ', flush=True)
                    k,v = try_strat_finetune(args, att1, def1, att2, def2,
                                             do_log=False, parent_log=p_log)
                    def_strat[(def2, k)]=v
                    print(f'\r{p_log} {k} ==> {v}')
          (def2,best_k),best_v = sorted(def_strat.items(), key = lambda l: l[1])[0]
          print(f"{att2}: {def2} {best_k} ==> {best_v}")
          strat[att2]=(def2,best_k,best_v)
     return strat

def try_strat_finetune(args, att1, def1, att2, def2, do_log=True, parent_log=""):
    from itertools import chain, combinations
    def powerset(iterable):
        s = list(iterable)
        return chain.from_iterable(combinations(s, r) for r in range(len(s)+1))
    
    res = {}
    if(do_log): print(att1, def1, att2, def2)
    free = [f for f in list(range(8)) if f not in [att1, def1, att2, def2]]

    for A in powerset(free):
        B = tuple([b for b in free if b not in A])
        print(f"\r{parent_log} {A} {B}", end=' ')
        A_str = " ".join([str(a) for a in A])
        B_str = " ".join([str(b) for b in B])
        
        os.system(f"./DEV split {att1} {def1} {att2} {def2} A {A_str} B {B_str} > /dev/null")
        runfile("../strats.txt", args.procnum, args.memlimit, do_log=False)
        res[(A, B)] = (log["PN"], log["fail"], log["DN"])
        init_log()

    if(do_log): print("\nPN fail DN")
    sorted_strats = dict(sorted(res.items(), key = lambda l: l[1]))
    for k,v in sorted_strats.items():
        if(do_log): print(k, "==>", v)

    return next(iter(sorted_strats.items()))

def print_sol(att1, def1, strats):
     for att2,v in strats.items():
          def2,(A,B),_ = v
          A_str = ','.join([str(a) for a in A])
          B_str = ','.join([str(b) for b in B])
          print(f"Sol({{{att1},{def1}}},{{{att2},{def2}}},{{{A_str}}},{{{B_str}}}),")

def print_mtx(res, max_i, max_j):
        b,e = max_j*11//2-1,max_j*11-(max_j*11//2)-1
        print("{:5}|{}{}{}|{}{}{}|".format(""," "*b,"A"," "*e," "*b,"B"," "*e))
        print(str("{:5}|"+"{:>5}     |"*max_j*2).format("", *(list(range(max_j))+list(range(max_j)))))
        print(str("{:5}+"+"----------+"*max_j*2).format("-"*5))
        #print("{:5}+{}+{}+{}+{}+{}+{}+".format("-"*5,"-"*10, "-"*10, "-"*10, "-"*10, "-"*10, "-"*10))
        for i in range(1,max_i+1):
            row = [res[side, i,j] for side in ['A', 'B'] for j in range(1,max_j+1)]
            print(str("{:<5}|"+("{:>10}|"*2*max_j)).format(i,*row))
        
        print(str("{:5}+"+"----------+"*max_j*2).format("-"*5))

if(__name__ == "__main__"):
    args = get_args()

    if(args.mode == "strat1"):
         att1, = args.moves
         strat1(args, att1)
    elif(args.mode == "strat2"):
         att1,def1 = args.moves
         strat = strat2(args, att1, def1)
         print_sol(att1,def1,strat)
    elif(args.mode == "strat_old2"):
        try_strat_second(args)
    elif(args.mode == "finetune"):
        att1, def1, att2, def2 = args.moves
        try_strat_finetune(args, att1, def1, att2, def2)
    elif(args.mode == "strats"):
        runfile("../strats.txt", args.procnum, args.memlimit)
    elif(args.mode == "base"):
        runfile("../ors.txt", args.procnum, args.memlimit)
        runfile("../ands.txt", args.procnum, args.memlimit)
        #run_all_board(log["again"], 2, 150)
        print("Summary: {}/{} [failed: {}] [proof: {}]".format(log["DN"], log["all"],
                                                            log["fail"], log["PN"]))
    else:
        print("Mode not found: {}".format(mode))

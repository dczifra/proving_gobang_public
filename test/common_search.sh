#!/bin/bash

exhaustive1_base_search () {
    memlim=10
    procnum=20
    python PARALLEL.py -memlimit $memlim -procnum $procnum -mode strat1 -moves 4
    python PARALLEL.py -memlimit $memlim -procnum $procnum -mode strat1 -moves 5
    python PARALLEL.py -memlimit $memlim -procnum $procnum -mode strat1 -moves 6
    python PARALLEL.py -memlimit $memlim -procnum $procnum -mode strat1 -moves 7
}

exhaustive1_second_move () {
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 4 1
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 4 5
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 4 6
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 5 2
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 5 6
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 6 2
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 6 5
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 6 7
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 7 2
    time python3 PARALLEL.py -mode strat2 -procnum 10 -memlimit 0.5 -moves 7 6
}

exhaustive2_second_move () {
    memlim=1
    procnum=40
    time python3 PARALLEL.py -mode strat2 -procnum $procnum -memlimit $memlim -moves 4 6
    time python3 PARALLEL.py -mode strat2 -procnum $procnum -memlimit $memlim -moves 5 2
    time python3 PARALLEL.py -mode strat2 -procnum $procnum -memlimit $memlim -moves 6 1
    time python3 PARALLEL.py -mode strat2 -procnum $procnum -memlimit $memlim -moves 7 2
    time python3 PARALLEL.py -mode strat2 -procnum $procnum -memlimit $memlim -moves 4 1
    time python3 PARALLEL.py -mode strat2 -procnum $procnum -memlimit $memlim -moves 4 5
}

fine_tune () {
    memlim=2
    procnum=40
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 4 6 0 2
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 4 6 1 0
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 4 6 3 2
    
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 5 2 0 4
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 5 2 0 4
    
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 6 1 0 4
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 6 1 5 7
    
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 7 2 1 6
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 7 2 4 6
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 7 2 5 4
    time python3 PARALLEL.py -mode finetune -procnum $procnum -memlimit $memlim -moves 7 2 6 4
}

#exhaustive1_base_search
#exhaustive2_second_move
fine_tune

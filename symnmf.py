import sys
import os
import numpy as np
import math
import symnmf

def initialize_H():
    pass

def main(K, goal, file_name): 
    np.random.seed(0)
    if(goal == "symnmf"):
        H = initialize_H()
    elif(goal == "sym"):
        N = 5
        d = 3
        A = symnmf.sym(goal, file_name, N, d)
    

if __name__ == "__main__":
    try:
        K = int(sys.argv[1])
        goal = str(sys.argv[2])
        file_name = str(sys.argv[3])
        main(K, goal, file_name)
    except:
         print("An Error Has Occurred")
import sys
import numpy as np
import pandas as pd
import math
import symnmf

def read_vectors(file_name):
    data = pd.read_csv(file_name, header=None)
    X = data.values.tolist()
    return X

def initialize_H(N, K, w_avg):
    mat = [[np.random.uniform(0, 2*math.sqrt(w_avg/K)) for _ in range(K)] for _ in range(N)]
    return mat

def print_result_matrix(matrix):
    print('\n'.join(['\t'.join([str(round(cell,4)).ljust(6,'0') for cell in row]) for row in matrix]))


def main(K, goal, file_name): 
    np.random.seed(0)
    X = read_vectors(file_name)
    N = len(X)
    d = len(X[0])
    if(goal == "symnmf"):
        W = symnmf.norm(X, N, d)
        w_avg = np.mean(W)
        H = initialize_H(N, K, w_avg)
        H = symnmf.symnmf(W, H, N, d, K)
        print_result_matrix(H)
    elif(goal == "sym"):
        A = symnmf.sym(X, N, d)
        print_result_matrix(A)
    elif(goal == "ddg"):
        D = symnmf.ddg(X, N, d)
        print_result_matrix(D)
    elif(goal == "norm"):
        W = symnmf.norm(X, N, d)
        print_result_matrix(W)

if __name__ == "__main__":
    try:
        K = int(sys.argv[1])
        goal = str(sys.argv[2])
        file_name = str(sys.argv[3])
        main(K, goal, file_name)
    except:
         print("An Error Has Occurred")
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

def get_mat_avg(mat, rows, cols):
    sum = 0
    cnt = 0
    for i in range(rows):
        for j in range(cols):
            sum += mat[i][j]
            cnt+=1
    return sum / cnt

def print_result_matrix(mat, rows, cols):
    for i in range(rows):
        for j in range(cols):
            print(mat[i][j])

def main(K, goal, file_name): 
    np.random.seed(0)
    X = read_vectors(file_name)
    N = len(X)
    d = len(X[0])
    if(goal == "symnmf"):
        W = symnmf.norm(X, N, d)
        w_avg = get_mat_avg(W, N, N)
        H = initialize_H(N, K, w_avg)
        H = symnmf.symnmf(H, W, N, d)
        print_result_matrix(H, N, K)
    elif(goal == "sym"):
        print("1")
        A = symnmf.sym(X, N, d)
        print_result_matrix(A, N, N)
    elif(goal == "ddg"):
        D = symnmf.ddg(X, N, d)
        print_result_matrix(D, N, N)
    elif(goal == "norm"):
        W = symnmf.norm(X, N, d)
        print_result_matrix(W, N, N)

if __name__ == "__main__":
    try:
        K = int(sys.argv[1])
        goal = str(sys.argv[2])
        file_name = str(sys.argv[3])
        main(K, goal, file_name)
    except:
         print("An Error Has Occurred")
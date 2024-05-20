import sys
import math
import numpy as np
import pandas as pd
import symnmf
import kmeans
from sklearn.metrics import silhouette_score

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

def read_vectors(file_name):
    data = pd.read_csv(file_name, header=None)
    X = data.values.tolist()
    return X

def create_lable_array_symnmf(H):
    max = -1
    max_idx = -1
    arr = []
    for i in range(len(H)):
        for j in range(len(H[i])):
            if H[i][j] > max:
                max = H[i][j]
                max_idx = j
        arr.append(max_idx)
        max = -1
        max_idx = -1
    return arr

def create_lable_array_kmeans(X, centroids, d):
    arr = []
    for x in X:
        arr.append(np.argmin([kmeans.distance(x, c, d) for c in centroids]))
    return arr

def main(K, file_name):
    np.random.seed(0)
    X = read_vectors(file_name)
    N = len(X)
    d = len(X[0])
    W = symnmf.norm(X, N, d)
    w_avg = get_mat_avg(W, N, N)
    H = initialize_H(N, K, w_avg)
    H_symnmf = symnmf.symnmf(W, H, N, d, K)
    L_symnmf = create_lable_array_symnmf(H_symnmf)
    H_kmeans = kmeans.kmeans_py(K, N, d, 300, file_name)
    L_kmeans = create_lable_array_kmeans(X, H_kmeans, d)
    print("nmf:", str(round(silhouette_score(X, L_symnmf),4)).ljust(6,'0'))
    print("kmeams:", str(round(silhouette_score(X, L_kmeans),4)).ljust(6,'0'))


if __name__ == "__main__":
    try:
        K = int(sys.argv[1])
        file_name = str(sys.argv[2])
        main(K, file_name)
    except:
         print("An Error Has Occurred")
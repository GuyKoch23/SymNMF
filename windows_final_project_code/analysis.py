import sys
import math
import numpy as np
import pandas as pd
import csymnmf
import symnmf
import kmeans
from sklearn.metrics import silhouette_score

def create_lable_array_symnmf(H):
    """
    Creates the lables for a given array in the symnmf module

    Args:
        H: Clustering matrix

    Returns:
        Lables array
    """
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
    """
    Creates the lables for a given array in the kmeans module

    Args:
        X: data points matrix
        centroids: chosen centroids in the algorithem
        d: number of dimentions in the vector

    Returns:
        Lables array
    """
    arr = []
    for x in X:
        arr.append(np.argmin([kmeans.distance(x, c, d) for c in centroids]))
    return arr

def analyse_lables(N, d, X, W, H):
    """
    Analyses the lables of the datapoints regarding the kmeans and symnmf algorithems. 

    Args:
        N: Number of datapoints
        d: number of dimentions in the vector
        X: data points matrix
        W: Similarity matrix
        H: Clustering matrix

    Returns:
        Lables arrays
    """
    H_symnmf = csymnmf.symnmf(W, H, N, d, K)
    Lables_arr_symnmf = create_lable_array_symnmf(H_symnmf)
    H_kmeans = kmeans.kmeans_py(K, N, d, 300, file_name)
    Lables_arr_kmeans = create_lable_array_kmeans(X, H_kmeans, d)
    return Lables_arr_symnmf, Lables_arr_kmeans

def print_silhouette_score(test_name, samples, Lables_arr):
    """
    Print the silhouette_score pretiffy.

    Args:
        test_name: type of the algorithems
        samples: samples for the algo
        Lables_arr: output lables regarding the samples
    """
    print(test_name + ":", "{:.4f}".format(silhouette_score(samples, Lables_arr)))

def main(K, file_name):
    np.random.seed(0)
    X = symnmf.read_vectors(file_name)
    N = len(X)
    d = len(X[0])
    W = csymnmf.norm(X, N, d)
    m = np.mean(W)
    H = symnmf.initialize_H(N, K, m)
    Lables_arr_symnmf, Lables_arr_kmeans = analyse_lables(N, d, X, W, H)
    print_silhouette_score("nmf", X, Lables_arr_symnmf)
    print_silhouette_score("kmeans", X, Lables_arr_kmeans)

if __name__ == "__main__":
    try:
        K = int(sys.argv[1])
        file_name = str(sys.argv[2])
        main(K, file_name)
    except:
         print("An Error Has Occurred")
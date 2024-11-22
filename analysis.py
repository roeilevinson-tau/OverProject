import sys
import numpy as np
import pandas as pd
from sklearn.metrics import silhouette_score
from symnmf import symnmf
from kmeans import kmeans 

def read_data(file_path):
    """Reads data points from a file."""
    data = pd.read_csv(file_path, header=None)
    return data.values.tolist()

def symnmf_clustering(k, matrix):
    """Performs clustering using SymNMF."""
    H_final = symnmf(k, matrix)
    cluster_assignments = np.argmax(H_final, axis=1)
    return cluster_assignments

def main():
    try:
        if len(sys.argv) != 3:
            raise ValueError("Invalid number of arguments")

        k = int(sys.argv[1])
        file_name = sys.argv[2]
        matrix = read_data(file_name)
        symnmf_labels = symnmf_clustering(k, matrix)
        symnmf_score = silhouette_score(matrix, symnmf_labels)
        print(f"nmf: {symnmf_score:.4f}")
        kmeans_labels = kmeans(k, file_name)
        kmeans_score = silhouette_score(matrix, kmeans_labels)
        print(f"kmeans: {kmeans_score:.4f}")
    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)

if __name__ == "__main__":
    main()
import math
import sys
import pandas as pd
import numpy as np
import random
import mysymnmf as sf

np.random.seed(1234)


def sym_matrix(matrix):
    return sf.sym(matrix)

def ddg_matrix(matrix):
    return sf.ddg(matrix)

def norm_matrix(matrix):
    return sf.norm(matrix)

def symnmf_matrix(k, matrix):
    W = norm_matrix(matrix)
    m = np.mean(W)
    H = np.random.uniform(0, 2 * math.sqrt(m / k), size=(len(matrix), k))
    H_list = H.tolist()
    result = sf.symnmf(H_list, W)
    return result

def main():
    try:
        if (len(sys.argv) < 3 or len(sys.argv) > 4):
            raise ValueError("Invalid number of arguments")

        k = int(sys.argv[1])
        goal = sys.argv[2]
        file_name = sys.argv[3]
        
        data = pd.read_csv(file_name, header=None)
        matrix = [x.tolist() for index, x in data.iterrows()]
        if k >= len(matrix) or len(matrix) == 0:
            raise ValueError("Invalid value of k or empty matrix")

        if goal == "sym":
            res = sym_matrix(matrix)
        elif goal == "ddg":
            res = ddg_matrix(matrix)
        elif goal == "norm":
            res = norm_matrix(matrix)
        elif goal == "symnmf":
            res = symnmf_matrix(k, matrix)
        else:
            raise ValueError("Invalid goal")

        for row in res:
            print(",".join(str("{:.4f}".format(round(x, 4))) for x in row))
    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)


if __name__ == "__main__":
    main()

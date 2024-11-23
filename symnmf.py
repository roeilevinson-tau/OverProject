import math
import sys
import pandas as pd
import numpy as np
import random
import mysymnmf as sf

np.random.seed(1234)


def sym(matrix):
    """Compute the symmetric matrix."""
    return sf.sym(matrix)

def ddg(matrix):
    """Compute the degree diagonal matrix."""
    return sf.ddg(matrix)

def norm(matrix):
    """Normalize the input matrix."""
    return sf.norm(matrix)

def symnmf(k, matrix):
    """Perform symmetric non-negative matrix factorization."""
    W = norm(matrix)
    m = np.mean(W)
    H = np.random.uniform(0, 2 * math.sqrt(m / k), size=(len(matrix), k))
    H_list = H.tolist()
    result = sf.symnmf(H_list, W)
    return result

def main():
    """Main function to execute the script."""
    try:
        if len(sys.argv) < 3 or len(sys.argv) > 4:
            raise ValueError("Invalid number of arguments")
        if len(sys.argv) == 3:
            goal = sys.argv[1]
            file_name = sys.argv[2]
            k = None
        elif len(sys.argv) == 4:
            k = int(sys.argv[1])
            goal = sys.argv[2]
            file_name = sys.argv[3]
        else:
            raise ValueError("Invalid number of arguments")
        data = pd.read_csv(file_name, header=None)
        matrix = [x.tolist() for index, x in data.iterrows()]
        if len(matrix) == 0:
            raise ValueError("Empty matrix")
        if goal == "sym":
            res = sym(matrix)
        elif goal == "ddg":
            res = ddg(matrix)
        elif goal == "norm":
            res = norm(matrix)
        elif goal == "symnmf":
            if k >= len(matrix):
                raise ValueError("Invalid value of k")
            res = symnmf(k, matrix)
        else:
            raise ValueError("Invalid goal")
        for row in res:
            print(",".join(str("{:.4f}".format(round(x, 4))) for x in row))
    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)


if __name__ == "__main__":
    main()

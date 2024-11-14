import math
import sys
import pandas as pd
import numpy as np
import random
import symnmf

def sym(matrix):
    return symnmf.sym(matrix)

def ddg(matrix):
    return symnmf.ddg(matrix)

def norm(matrix):
    return symnmf.norm(matrix)

def main():
    if (len(sys.argv) < 3 or len(sys.argv) > 4):
        print("An Error Has Occurred")
        sys.exit(1)

    k = sys.argv[1]
    goal = sys.argv[2]
    file_name = sys.argv[3]
    
    try: 
        data = pd.read_csv(file_name, header=None)
    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)

    matrix = [x.tolist() for index, x in data.iterrows()]
    if int(k) >= len(matrix) or len(matrix) == 0:
        print("An Error Has Occurred")
        sys.exit(1)

    try: 
        if goal == "sym":
            res = sym(matrix)
        elif goal == "ddg":
            res = ddg(matrix)
        elif goal == "norm":
            res = norm(matrix)
        elif goal == "symnmf":
            pass
        else:
            print("An Error Has Occurred")
            sys.exit(1)
    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)

    for row in res:
        print(",".join(str("{:.4f}".format(round(x, 4))) for x in row))


if __name__ == "__main__":
    main()

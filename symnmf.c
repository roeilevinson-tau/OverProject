#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

#define DELIMITER ','

/* Function to load a matrix from a file */
Matrix* load_matrix_from_file(const char *file_name) {
    int n = 0;
    int d = 0;
    int ch;
    int i, j;
    double **data;
    FILE *file;
    Matrix *matrix;
    file = fopen(file_name, "r");
    if (file == NULL) {
        return NULL;
    }
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            n++;
        } else if ((ch == ',') && (n == 0)) {
            d++;
        }
    }
    d++;
    rewind(file);
    data = (double **)malloc(n * sizeof(double *));
    for (i = 0; i < n; i++) {
        data[i] = (double *)malloc(d * sizeof(double));
        for (j = 0; j < d; j++) {
            if (fscanf(file, "%lf,", &data[i][j]) != 1) {
                return NULL;
            }
        }
    }
    fclose(file);
    matrix = (Matrix *)malloc(sizeof(Matrix));
    if (matrix == NULL) {
        return NULL;
    }
    matrix->rows = n;
    matrix->cols = d;
    matrix->data = data;
    return matrix;
}

/* Function to free the memory allocated for a matrix */
void free_matrix(Matrix *matrix) {
    int i;
    if (matrix == NULL) {
        return;
    }
    if (matrix->data != NULL) {
        for (i = 0; i < matrix->rows; i++) {
            if (matrix->data[i] != NULL) {
                free(matrix->data[i]);
            }
        }
        free(matrix->data);
    }
    free(matrix);
}

/* Function to initialize a matrix with zeros */
Matrix* initialize_matrix_with_zeros(int rows, int cols) {
    int i, j;
    Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
    if (matrix == NULL) {
        return NULL;
    }
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = (double **)malloc(rows * sizeof(double *));
    if (matrix->data == NULL) {
        free(matrix);
        return NULL;
    }
    for (i = 0; i < rows; i++) {
        matrix->data[i] = (double *)calloc(cols, sizeof(double));
        if (matrix->data[i] == NULL) {
            for (j = 0; j < i; j++) {
                free(matrix->data[j]);
            }
            free(matrix->data);
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

/* Function to calculate Euclidean distance between two vectors */
double euclidean_distance(double *vec1, double *vec2, int length) {
    double sum = 0.0;
    int i;
    for (i = 0; i < length; i++) {
        double diff = vec1[i] - vec2[i];
        sum += diff * diff;
    }
    return sum;
}

/* Function to compute the symmetric similarity matrix */
Matrix* sym(Matrix *matrix) {
    int n, i, j;
    double distance;
    Matrix *similarity_matrix;
    if (matrix == NULL) {
        return NULL;
    }
    n = matrix->rows;
    similarity_matrix = initialize_matrix_with_zeros(n, n);
    if (similarity_matrix == NULL) {
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < i; j++) {
            distance = euclidean_distance(matrix->data[i], matrix->data[j], matrix->cols);
            similarity_matrix->data[i][j] = exp(-0.5 * distance);
            similarity_matrix->data[j][i] = similarity_matrix->data[i][j];
        }
        similarity_matrix->data[i][i] = 0.0;
    }
    return similarity_matrix;
}

/* Function to compute the diagonal degree matrix */
Matrix* ddg(Matrix *matrix) {
    int n, i, j;
    Matrix *sym_matrix, *diagonal_matrix;
    if (matrix == NULL) {
        return NULL;
    }
    /* First, get the sym matrix */
    sym_matrix = sym(matrix);
    if (sym_matrix == NULL) {
        return NULL;
    }
    n = matrix->rows < matrix->cols ? matrix->cols : matrix->rows;
    /* Initialize a new matrix for the diagonal */
    diagonal_matrix = initialize_matrix_with_zeros(n, n);
    if (diagonal_matrix == NULL) {
        free_matrix(sym_matrix); /* Free sym_matrix before returning */
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            diagonal_matrix->data[i][i] += sym_matrix->data[i][j];
        }
    }
    free_matrix(sym_matrix); /* Free sym_matrix after use */
    return diagonal_matrix;
}

/* Function to multiply two matrices */
Matrix* multiply_matrices(Matrix *matrix1, Matrix *matrix2) {
    int rows, cols, common_dim, i, j, k;
    Matrix *result_matrix;
    if (matrix1 == NULL || matrix2 == NULL) {
        return NULL;
    }
    if (matrix1->cols != matrix2->rows) {
        return NULL;
    }
    rows = matrix1->rows;
    cols = matrix2->cols;
    common_dim = matrix1->cols;
    result_matrix = initialize_matrix_with_zeros(rows, cols);
    if (result_matrix == NULL) {
        return NULL;
    }
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            for (k = 0; k < common_dim; k++) {
                result_matrix->data[i][j] += matrix1->data[i][k] * matrix2->data[k][j];
            }
        }
    }
    return result_matrix;
}

/* Function to compute the inverse square root of a matrix */
Matrix* compute_inverse_sqrt(Matrix *matrix) {
    int rows, cols, i, j;
    Matrix *result_matrix;
    if (matrix == NULL) {
        return NULL;
    }
    rows = matrix->rows;
    cols = matrix->cols;
    result_matrix = initialize_matrix_with_zeros(rows, cols);
    if (result_matrix == NULL) {
        return NULL;
    }
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (matrix->data[i][j] != 0) {
                result_matrix->data[i][j] = 1.0 / sqrt(matrix->data[i][j]);
            } else {
                result_matrix->data[i][j] = 0;
            }
        }
    }
    return result_matrix;
}

/* Function to normalize a matrix */
Matrix* norm(Matrix *matrix) {
    int rows, i, j;
    Matrix *matrices[5]; /* Array to hold matrices: 0-normalized, 1-sym, 2-ddg, 3-inv_sqrt_ddg, 4-temp */
    Matrix *result_matrix;
    if (matrix == NULL) return NULL;
    rows = matrix->rows;
    matrices[0] = initialize_matrix_with_zeros(rows, rows);
    if (matrices[0] == NULL) return NULL;
    matrices[1] = sym(matrix);
    matrices[2] = ddg(matrix);
    matrices[3] = compute_inverse_sqrt(matrices[2]);
    matrices[4] = multiply_matrices(matrices[3], matrices[1]);
    result_matrix = multiply_matrices(matrices[4], matrices[3]);
    if (!matrices[1] || !matrices[2] || !matrices[3] || !matrices[4] || !result_matrix) {
        for (i = 0; i < 5; i++) free_matrix(matrices[i]);
        return NULL;
    }
    for (i = 0; i < rows; i++)
        for (j = 0; j < rows; j++)
            matrices[0]->data[i][j] = result_matrix->data[i][j];
    for (i = 1; i < 5; i++) free_matrix(matrices[i]);
    free_matrix(result_matrix);
    return matrices[0];
}

/* Function to update matrix H in the SYM-NMF algorithm */
Matrix* update(Matrix* H, Matrix* W) {
    int n, k, i, j;
    double b;
    Matrix* WH, *Ht, *HHt, *HHtH, *next_h;
    n = W->rows;
    k = H->cols;
    WH = multiply_matrices(W, H);
    Ht = transpose(H);
    HHt = multiply_matrices(H, Ht);
    HHtH = multiply_matrices(HHt, H);
    next_h = initialize_matrix_with_zeros(n, k);
    b = 0.5;
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            next_h->data[i][j] = H->data[i][j] * (b + b * (WH->data[i][j] / HHtH->data[i][j]));
        }
    }
    free_matrix(WH);
    free_matrix(Ht);
    free_matrix(HHt);
    free_matrix(HHtH);
    return next_h;
}

/* Function to perform the SYM-NMF algorithm */
Matrix* symnmf(Matrix *H, Matrix *W) {
    int iter, n, k, i, j;
    double eps;
    Matrix* next_H;
    iter = 0;
    eps = 0.0001;
    n = H->rows;
    k = H->cols;
    while (iter < 300) {
        next_H = update(H, W);
        if (pow(frobidean_distance(H, next_H), 2) < eps) {
            return next_H;
        }
        for (i = 0; i < n; i++) {
            for (j = 0; j < k; j++) {
                H->data[i][j] = next_H->data[i][j];
            }
        }
        free_matrix(next_H);
        iter++;
    }
    return next_H;
}

/* Function to print a matrix with specific formatting */
void print_matrix(Matrix *matrix) {
    int i, j;
    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->cols; j++) {
            printf("%.4f", matrix->data[i][j]);
            if (j < matrix->cols - 1) {
                printf(","); /* Add a comma between elements */
            }
        }
        printf("\n"); /* Newline at the end of each row */
    }
}

/* Function to calculate the Frobenius distance between two matrices */
double frobidean_distance(Matrix* mat1, Matrix* mat2) {
    double d = 0.0;
    int i, j;
    int rows = mat1->rows;
    int cols = mat1->cols;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            d += (mat1->data[i][j] - mat2->data[i][j]) * (mat1->data[i][j] - mat2->data[i][j]);
        }
    }
    return sqrt(d);
}

/* Function to transpose a matrix */
Matrix* transpose(Matrix* matrix) {
    int rows, cols, i, j;
    Matrix* transposed_matrix;
    if (matrix == NULL) {
        return NULL;
    }
    rows = matrix->rows;
    cols = matrix->cols;
    transposed_matrix = initialize_matrix_with_zeros(cols, rows);
    if (transposed_matrix == NULL) {
        return NULL;
    }
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            transposed_matrix->data[j][i] = matrix->data[i][j];
        }
    }
    return transposed_matrix;
}

/* Main function to execute the program based on command-line arguments */
int main(int argc, char *argv[]) {
    char *goal, *file_name;
    Matrix *matrix, *result;
    if (argc != 3) {
        fprintf(stderr, "An Error Has Occurred\n");
        return 1;
    }
    goal = argv[1];
    file_name = argv[2];
    matrix = load_matrix_from_file(file_name);
    if (matrix == NULL) {
        fprintf(stderr, "An Error Has Occurred\n");
        return 1;
    }
    result = NULL;
    if (strcmp(goal, "sym") == 0) {
        result = sym(matrix);
    } else if (strcmp(goal, "ddg") == 0) {
        result = ddg(matrix);
    } else if (strcmp(goal, "norm") == 0) {
        result = norm(matrix);
    } else {
        fprintf(stderr, "An Error Has Occurred\n");
        free_matrix(matrix);
        return 1;
    }
    if (result != NULL) {
        print_matrix(result);
        free_matrix(result);
    }
    free_matrix(matrix); /* Free the original matrix */
    return 0;
}
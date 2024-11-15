#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

#define DELIMITER ','




// Function to load a matrix from a file
Matrix* load_matrix_from_file(const char *file_name) {
    int n = 0;
    int d = 0;
    int ch;
    int i, j; 
    double **data;


    FILE *file = fopen(file_name, "r");
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

    Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
    if (matrix == NULL) {
        return NULL;
    }

    matrix->rows = n;
    matrix->cols = d;
    matrix->data = data;

    return matrix;
}

void free_matrix(Matrix *matrix) {
    if (matrix == NULL) {
        return;
    }
    
    if (matrix->data != NULL) {
        for (int i = 0; i < matrix->rows; i++) {
            if (matrix->data[i] != NULL) {
                free(matrix->data[i]);
            }
        }
        free(matrix->data);
    }
    
    free(matrix);
}


Matrix* initialize_matrix_with_zeros(int rows, int cols) {
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

    for (int i = 0; i < rows; i++) {
        matrix->data[i] = (double *)calloc(cols, sizeof(double));
        if (matrix->data[i] == NULL) {
            // If allocation fails, free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(matrix->data[j]);
            }
            free(matrix->data);
            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

// Function to calculate Euclidean distance between two vectors
double euclidean_distance(double *vec1, double *vec2, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        double diff = vec1[i] - vec2[i];
        sum += diff * diff;
    }
    return sum;
}

Matrix* sym(Matrix *matrix) {
    if (matrix == NULL) {
        return NULL;
    }

    int n = matrix->rows;
    Matrix *similarity_matrix = initialize_matrix_with_zeros(n, n);
    if (similarity_matrix == NULL) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            double distance = euclidean_distance(matrix->data[i], matrix->data[j], matrix->cols);
            similarity_matrix->data[i][j] = exp(-0.5 * distance);
            similarity_matrix->data[j][i] = similarity_matrix->data[i][j];
        }
        similarity_matrix->data[i][i] = 0.0;
    }

    return similarity_matrix;
}

Matrix* ddg(Matrix *matrix) {
    if (matrix == NULL) {
        return NULL;
    }
    // First, get the sym matrix
    Matrix *sym_matrix = sym(matrix);
    if (sym_matrix == NULL) {
        return NULL;
    }

    int n = matrix->rows < matrix->cols ? matrix->cols : matrix->rows;

    // Initialize a new matrix for the diagonal
    Matrix *diagonal_matrix = initialize_matrix_with_zeros(n, n);
    if (diagonal_matrix == NULL) {
        free_matrix(sym_matrix); // Free sym_matrix before returning
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            diagonal_matrix->data[i][i] += sym_matrix->data[i][j];
        }
    }

    free_matrix(sym_matrix); // Free sym_matrix after use
    return diagonal_matrix;
}
Matrix* multiply_matrices(Matrix *matrix1, Matrix *matrix2) {
    if (matrix1 == NULL || matrix2 == NULL) {
        return NULL;
    }

    if (matrix1->cols != matrix2->rows) {
        return NULL;
    }

    int rows = matrix1->rows;
    int cols = matrix2->cols;
    int common_dim = matrix1->cols;

    Matrix *result_matrix = initialize_matrix_with_zeros(rows, cols);
    if (result_matrix == NULL) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < common_dim; k++) {
                result_matrix->data[i][j] += matrix1->data[i][k] * matrix2->data[k][j];
            }
        }
    }

    return result_matrix;
}

Matrix* compute_inverse_sqrt(Matrix *matrix) {
    if (matrix == NULL) {
        return NULL;
    }

    int rows = matrix->rows;
    int cols = matrix->cols;

    Matrix *result_matrix = initialize_matrix_with_zeros(rows, cols);
    if (result_matrix == NULL) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix->data[i][j] != 0) {
                result_matrix->data[i][j] = 1.0 / sqrt(matrix->data[i][j]);
            } else {
                result_matrix->data[i][j] = 0; // Handle division by zero
            }
        }
    }

    return result_matrix;
}


Matrix* norm(Matrix *matrix) {
    if (matrix == NULL) {
        return NULL;
    }

    int rows = matrix->rows;
    Matrix *normalized_similarity_matrix = initialize_matrix_with_zeros(rows, rows);
    if (normalized_similarity_matrix == NULL) {
        return NULL;
    }

    // Create the symmetric matrix first
    Matrix *sym_matrix = sym(matrix);
    if (sym_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        return NULL;
    }

    // Create the diagonal degree matrix using the ddg function
    Matrix *ddg_matrix = ddg(matrix);
    if (ddg_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        return NULL;
    }

    // Compute the inverse square root of the diagonal degree matrix
    Matrix *inv_sqrt_ddg_matrix = compute_inverse_sqrt(ddg_matrix);
    if (inv_sqrt_ddg_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        free_matrix(ddg_matrix);
        return NULL;
    }

    // Multiply matrices to get the normalized similarity matrix
    Matrix *temp_matrix = multiply_matrices(inv_sqrt_ddg_matrix, sym_matrix);
    if (temp_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        free_matrix(ddg_matrix);
        free_matrix(inv_sqrt_ddg_matrix);
        return NULL;
    }

    Matrix *result_matrix = multiply_matrices(temp_matrix, inv_sqrt_ddg_matrix);
    if (result_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        free_matrix(ddg_matrix);
        free_matrix(inv_sqrt_ddg_matrix);
        free_matrix(temp_matrix);
        return NULL;
    }

    // Copy the result to normalized_similarity_matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            normalized_similarity_matrix->data[i][j] = result_matrix->data[i][j];
        }
    }

    free_matrix(temp_matrix);
    free_matrix(result_matrix);
    free_matrix(inv_sqrt_ddg_matrix);
    free_matrix(sym_matrix);
    free_matrix(ddg_matrix);

    return normalized_similarity_matrix;
}


Matrix* update(Matrix* H, Matrix* W) {
    int n = W->rows;
    int k = H->cols;
    Matrix* WH = multiply_matrices(W, H);
    Matrix* Ht = transpose(H);
    Matrix* HHt = multiply_matrices(H, Ht);
    Matrix* HHtH = multiply_matrices(HHt, H);
    Matrix* next_h = initialize_matrix_with_zeros(n, k);
    double b = 0.5;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            next_h->data[i][j] = H->data[i][j] * (b + b * (WH->data[i][j] / HHtH->data[i][j]));
        }
    }

    free_matrix(WH);
    free_matrix(Ht);
    free_matrix(HHt);
    free_matrix(HHtH);
    return next_h;
}   

Matrix* symnmf(Matrix *H, Matrix *W) {
    int iter = 0;
    double eps = 0.0001;
    Matrix* next_H;
    int n = H->rows;
    int k = H->cols;

    while (iter < 300) {
        next_H = update(H, W);
        if (pow(frobidean_distance(H, next_H), 2) < eps) {
            return next_H;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < k; j++) {
                H->data[i][j] = next_H->data[i][j];
            }
        }
        free_matrix(next_H);
        iter++;
    }
    return next_H;
}

// Function to print a matrix with specific formatting
void print_matrix(Matrix *matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%.4lf", matrix->data[i][j]);
            if (j < matrix->cols - 1) {
                printf(","); // Add a comma between elements
            }
        }
        printf("\n"); // Newline at the end of each row
    }
}

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

Matrix* transpose(Matrix* matrix) {
    if (matrix == NULL) {
        return NULL;
    }

    int rows = matrix->rows;
    int cols = matrix->cols;

    Matrix* transposed_matrix = initialize_matrix_with_zeros(cols, rows);
    if (transposed_matrix == NULL) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            transposed_matrix->data[j][i] = matrix->data[i][j];
        }
    }

    return transposed_matrix;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "An Error Has Occurred\n");
        return 1;
    }

    char *goal = argv[1];
    char *file_name = argv[2];

    // Load matrix from file
    Matrix *matrix = load_matrix_from_file(file_name);
    if (matrix == NULL) {
        fprintf(stderr, "An Error Has Occurred\n");
        return 1;
    }
    
    
    Matrix *result = NULL;
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
    free_matrix(matrix); // Free the original matrix
    return 0;
}




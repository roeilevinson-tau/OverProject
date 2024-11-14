#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

#define DELIMITER ','

// Function to allocate a 2D array
int allocate_2D_array(double ***array, int rows, int cols) {
    *array = (double **)malloc(rows * sizeof(double *));
    if (*array == NULL) {
        return 1;
    }
    for (int i = 0; i < rows; i++) {
        (*array)[i] = (double *)malloc(cols * sizeof(double));
        if ((*array)[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free((*array)[j]);
            }
            free(*array);
            return 1;
        }
    }
    return 0;
}

// Function to free a 2D array
void free_2D_array(double ***array, int rows) {
    for (int i = 0; i < rows; i++) {
        free((*array)[i]);
    }
    free(*array);
}

// Function to get the number of rows and columns in the file
int get_file_rows_cols(const char *file_name, int *rows, int *cols) {
    FILE *file;
    int c;
    double fake_num;
    file = fopen(file_name, "r");
    if (file == NULL)
        return 1;

    *rows = 0;
    *cols = 0;
    while (fscanf(file, "%lf", &fake_num) != EOF) {
        c = fgetc(file);

        if (*rows == 0)
            (*cols)++;

        if (c == '\n' || c == EOF)
            (*rows)++;
    }
    fclose(file);
    return 0;
}

// Function to read the file and populate the matrix
int read_file(double ***X, const char *file_name, int *rows, int *cols) {
    FILE *file;
    int c, row, col;
    if (get_file_rows_cols(file_name, rows, cols) != 0)
        return 1;

    if (allocate_2D_array(X, *rows, *cols) != 0)
        return 1;

    file = fopen(file_name, "r");
    if (file == NULL)
        return 1;

    row = 0;
    col = 0;
    while (fscanf(file, "%lf", &((*X)[row][col++])) != EOF) {
        c = fgetc(file);
        if (c == '\n') {
            row++;
            col = 0;
        } else if (c != DELIMITER && c != EOF) {
            free_2D_array(X, *rows);
            fclose(file);
            return 1;
        }
    }
    fclose(file);

    return 0;
}

// Function to load a matrix from a file
Matrix* load_matrix_from_file(const char *file_name) {
    int rows, cols;
    double **data;

    if (read_file(&data, file_name, &rows, &cols) != 0) {
        fprintf(stderr, "Failed to read matrix from file: %s\n", file_name);
        return NULL;
    }

    Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
    if (matrix == NULL) {
        free_2D_array(&data, rows);
        return NULL;
    }

    matrix->rows = rows;
    matrix->cols = cols;
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
        return NULL;
    }

    int i, j; // Variable declarations at the beginning of the function
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            diagonal_matrix->data[i][i] += sym_matrix->data[i][j];
        }
    }

    return diagonal_matrix;
}
Matrix* multiply_matrices(Matrix *matrix1, Matrix *matrix2) {
    if (matrix1 == NULL || matrix2 == NULL) {
        return NULL;
    }

    if (matrix1->cols != matrix2->rows) {
        fprintf(stderr, "Matrix dimensions do not match for multiplication.\n");
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

    // Create the diagonal degree matrix using the dgg function
    Matrix *ddg_matrix = ddg(matrix);
    if (ddg_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        return NULL;
    }

    // Create the inverse square root of the diagonal degree matrix
    Matrix *inv_sqrt_ddg_matrix = compute_inverse_sqrt(ddg_matrix);
    if (inv_sqrt_ddg_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        free_matrix(ddg_matrix);
        return NULL;
    }

    // First, compute inv_sqrt_ddg_matrix * sym_matrix
    Matrix *temp_matrix = multiply_matrices(inv_sqrt_ddg_matrix, sym_matrix);
    if (temp_matrix == NULL) {
        free_matrix(normalized_similarity_matrix);
        free_matrix(sym_matrix);
        free_matrix(ddg_matrix);
        free_matrix(inv_sqrt_ddg_matrix);
        return NULL;
    }

    // Then, compute (inv_sqrt_ddg_matrix * sym_matrix) * inv_sqrt_ddg_matrix
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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <goal> <file_name>\n", argv[0]);
        return 1;
    }

    char *goal = argv[1];
    char *file_name = argv[2];

    // Load matrix from file
    Matrix *matrix = load_matrix_from_file(file_name);
    if (matrix == NULL) {
        fprintf(stderr, "Failed to load matrix from file: %s\n", file_name);
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
        fprintf(stderr, "Invalid goal: %s\n", goal);
        free_matrix(matrix);
        return 1;
    }

    if (result != NULL) {
        print_matrix(result);
        free_matrix(result);
    }
    return 0;
}




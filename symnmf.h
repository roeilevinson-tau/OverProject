typedef struct Matrix {
    int rows;
    int cols;
    double **data;
} Matrix;

// Function prototypes
void free_matrix(Matrix *matrix);
Matrix* initialize_matrix_with_zeros(int rows, int cols);
Matrix* sym(Matrix *matrix);
Matrix* ddg(Matrix *matrix);
Matrix* norm(Matrix *matrix);

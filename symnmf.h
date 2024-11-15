typedef struct Matrix {
    int rows;
    int cols;
    double **data;
} Matrix;

Matrix* load_matrix_from_file(const char *file_name);
void free_matrix(Matrix *matrix);
Matrix* initialize_matrix_with_zeros(int rows, int cols);
double euclidean_distance(double *vec1, double *vec2, int length);
Matrix* sym(Matrix *matrix);
Matrix* ddg(Matrix *matrix);
Matrix* multiply_matrices(Matrix *matrix1, Matrix *matrix2);
Matrix* compute_inverse_sqrt(Matrix *matrix);
Matrix* norm(Matrix *matrix);
Matrix* update(Matrix* H, Matrix* W);
Matrix* symnmf(Matrix *H, Matrix *W);
void print_matrix(Matrix *matrix);
double frobidean_distance(Matrix* mat1, Matrix* mat2);
Matrix* transpose(Matrix* matrix);

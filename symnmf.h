typedef struct Matrix {
    int rows;
    int cols;
    double **data;
} Matrix;

void free_matrix(Matrix *matrix);
Matrix* initialize_matrix_with_zeros(int rows, int cols);
Matrix* sym(Matrix *matrix);
Matrix* ddg(Matrix *matrix);
Matrix* norm(Matrix *matrix);
Matrix* symnmf(Matrix *H, Matrix *W);
Matrix* update(Matrix *W, Matrix *H);
Matrix* transpose(Matrix* matrix);
double frobidean_distance(Matrix* mat1, Matrix* mat2);

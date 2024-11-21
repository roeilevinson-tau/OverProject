typedef struct Matrix {
    int rows;
    int cols;
    double **data;
} Matrix;

/* Loads a matrix from a file */
Matrix* load_matrix_from_file(const char *file_name);

/* Frees the memory allocated for a matrix */
void free_matrix(Matrix *matrix);

/* Initializes a matrix with zeros */
Matrix* initialize_matrix_with_zeros(int rows, int cols);

/* Calculates Euclidean distance between two vectors */
double euclidean_distance(double *vec1, double *vec2, int length);

/* Computes the symmetric similarity matrix */
Matrix* sym(Matrix *matrix);

/* Computes the diagonal degree matrix */
Matrix* ddg(Matrix *matrix);

/* Multiplies two matrices */
Matrix* multiply_matrices(Matrix *matrix1, Matrix *matrix2);

/* Computes the inverse square root of a matrix */
Matrix* compute_inverse_sqrt(Matrix *matrix);

/* Normalizes a matrix */
Matrix* norm(Matrix *matrix);

/* Updates matrix H in the SYM-NMF algorithm */
Matrix* update(Matrix* H, Matrix* W);

/* Performs the SYM-NMF algorithm */
Matrix* symnmf(Matrix *H, Matrix *W);

/* Prints a matrix with specific formatting */
void print_matrix(Matrix *matrix);

/* Calculates the Frobenius distance between two matrices */
double frobidean_distance(Matrix* mat1, Matrix* mat2);

/* Transposes a matrix */
Matrix* transpose(Matrix* matrix);

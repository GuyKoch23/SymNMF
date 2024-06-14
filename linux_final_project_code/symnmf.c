#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define eps 1e-4
#define max_iter 300
#define beta 0.5
#define ERROR_MESSAGE "An Error Has Occurred"

/*
 * Frees array of pointers 
 * Parameters:
 *   arr: The array of pointers.
 *   length: Length of the array.
 */
void free_array_of_pointers(double** arr, int length) {
  if(arr != NULL){
    int i;
    for (i=0; i<length; i++) {
        free(arr[i]);
    }
    free(arr);
    }
}

/*
 * Function to print a matrix pretiffy.
 * Parameters:
 *   mat: matrix to print.
 *   rows: number of rows of the matrix.
 *   cols: number of columns of the matrix.
 */
void print_matrix(double** mat, int rows, int cols) {
  int i, j;
  for (i=0; i<rows; i++) {
    for (j=0; j<cols; j++) {
      printf("%.4f",mat[i][j]);
      if (j != cols-1)
        printf(",");
      else
        printf("\n");
    }
  }
}

/*
 * Function which counts number of commas in string.
 * Parameters:
 *   str: String to count commas in.
 * Returns:
 *   The number of commas in the string.
 */
int count_commas_in_string(char* str){
    int count = 0;
    while(*str != '\0'){
        if(*str == ','){
            count++;
        }
        str++;
    }
    return count;
}

/*
 * Function which counts number of elements in file line.
 * Parameters:
 *   file_name: path to the file.
 * Returns:
 *   The nubmer of elements in the row.
 */
int get_number_of_elements_in_file_row(char* file_name){
    FILE *f;
    size_t len = 0;
    char* line = NULL;
    int num_of_commas = 0;
    f = fopen(file_name, "r");
    if(f == NULL){
        return -1;
    }
    if(getline(&line, &len, f) == -1 && !feof(f)){
        return -1;
    }
    num_of_commas = 1 + count_commas_in_string(line);
    fclose(f);
    free(line);
    return num_of_commas;
}

/*
 * Function which counts number of lines in file.
 * Parameters:
 *   file_name: path to the file.
 * Returns:
 *   The nubmer of lines in the file.
 */
int get_number_of_lines_in_file(char* file_name){
    FILE *f;
    size_t len = 0;
    char* line = NULL;
    int N = 0;
    f = fopen(file_name, "r");
    if(f == NULL){
        return -1;
    }
    while(!feof(f)){
        if(getline(&line, &len, f) == -1 && !feof(f)){
            return -1;
        }
        N++;
    }
    fclose(f);
    free(line);
    return N-1;
}

/*
 * Function which allocates matrix of rows rows and cols colunms.
 * Parameters:
 *   rows: number of rows in matrix.
 *   cols: number of columns in matrix.
 * Returns:
 *   The allocated matrix
 */
double** allocate_matrix(int rows, int cols){
    int i;
    double** mat = NULL;
    mat = (double**)calloc(rows, sizeof(double*));
    if(mat == NULL){
        return NULL;
    }
    for(i = 0 ; i < rows; i++){
        mat[i] = (double*)calloc(cols, sizeof(double));
        if(mat[i] == NULL){
            free_array_of_pointers(mat, i);
            return NULL;
        }   
    }
    return mat;
}

/*
 * Function which reads file of datapoints into matrix.
 * Parameters:
 *   file_name: path to the file.
 *   N: number of lines in the file.
 *   d: number of elements in file row.
 * Returns:
 *   The read matrix of lines which are datapoints.
 */
double** read_file_to_matrix_X(char* file_name, int N, int d){
    int d_counter, line_count = 0;
    size_t len = 0;
    char *start_iterator, *end_iterator, *line = NULL;
    double **X = NULL;
    FILE *f;

    X = allocate_matrix(N, d);
    if(X == NULL){
        return NULL;
    }
    f = fopen(file_name, "r");
    if(f == NULL){
        return NULL;
    }
    while ((int)getline(&line, &len, f) != -1 && line_count < N){
        start_iterator = line;
        end_iterator = line;
        d_counter = 0;
        while(*end_iterator != '\n' && *end_iterator != '\0') {
            while(*end_iterator != ',' && *end_iterator != '\n' && *end_iterator != '\0') {
                end_iterator++;
            }
            X[line_count][d_counter] = strtod(start_iterator, NULL);
            if (*end_iterator != '\n' && *end_iterator != '\0') {
                end_iterator++;
                start_iterator = end_iterator;
            }
            d_counter++;
        }
        line_count++;
    }
    free(line);
    fclose(f);
    return X;
}

/*
 * Function which calculates euclidean distance.
 * Parameters:
 *   mat: matrix to calculate its distance.
 *   row: row nubmer in the matrix.
 *   col: columm number in the matrix.
 *   d: number of elements in file row.
 * Returns:
 *   Distance calculated.
 */
double calculate_euclidean_distance(double** mat, int row, int col, int d){
    double distance = 0;
    int k;
    for(k = 0; k < d; k++){
        distance += pow((mat[row][k] - mat[col][k]),2);
    }
    return distance;
}

/*
 * Function which calculates symmatric matrix.
 * Parameters:
 *   X: datapoints matrix.
 *   N: number of datapoints.
 *   d: number of elements in datapoint.
 * Returns:
 *   Symmatric matrix.
 */
double** sym_c(double** X, int N, int d){
    int i, j;
    double** A = NULL;
    double current_sum = 0;
    double current_exp = 0;
    A = allocate_matrix(N, N);
    if(A == NULL){
        return NULL;
    }
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            if(i != j){
                current_sum = calculate_euclidean_distance(X, i, j, d);
                current_exp = exp((-0.5)*current_sum);
                A[i][j] = current_exp;
            }
        }
    }
    return A;
}

/*
 * Function which calculates vertice degree.
 * Parameters:
 *   mat: matrix of datapoints.
 *   row: row nubmer in the matrix.
 *   cols: columm number in the matrix.
 * Returns:
 *   Degree of the vertice.
 */
double calculate_vertice_degree(double** mat, int row, int cols){
        int j;
        double current_sum = 0;
        for(j = 0; j <cols; j++){
            current_sum += mat[row][j];
        }
        return current_sum;
}

/*
 * Function which calculates ddg matrix.
 * Parameters:
 *   X: datapoints matrix.
 *   N: number of datapoints.
 *   d: number of elements in datapoint.
 * Returns:
 *   ddg matrix.
 */
double** ddg_c(double** X, int N, int d){
    int i;
    double current_sum;
    double **A, **D;
    A = sym_c(X, N, d);
    if(A == NULL){
        return NULL;
    }
    D = allocate_matrix(N, N);
    if(D == NULL){
        free_array_of_pointers(A, N);
        return NULL;
    }
    for(i = 0; i < N; i++){
        current_sum = calculate_vertice_degree(A, i, N);
        D[i][i] = current_sum;
    }
    free_array_of_pointers(A, N);
    return D;
}

/*
 * Function which calculates mat to the power of minus half.
 * Parameters:
 *   mat: matrix of datapoints.
 *   N: number of datapoints.
 *   Calculated matrix.
 */
double** get_diag_pow_minus_half(double** mat, int N){
    int i;
    double** min_half_diag = allocate_matrix(N, N);
    if(min_half_diag == NULL){
        return NULL;
    }
    for(i = 0; i < N; i++){
        min_half_diag[i][i] = pow(mat[i][i], -0.5);
    }
    return min_half_diag;
}

/*
 * Function which calculates norm matrix.
 * Parameters:
 *   X: datapoints matrix.
 *   N: number of datapoints.
 *   d: number of elements in datapoint.
 * Returns:
 *   norm matrix.
 */
double** norm_c(double** X, int N, int d){
    int i, j;
    double **A, **D, **D_min_half, **W;
    A = sym_c(X, N, d);
    if(A == NULL){
        return NULL;
    }
    D = ddg_c(X, N, d);
    if(D == NULL){
        free_array_of_pointers(A, N);
        return NULL;
    }
    D_min_half = get_diag_pow_minus_half(D, N);
    if(D_min_half == NULL){
        free_array_of_pointers(A, N);
        free_array_of_pointers(D, N);
        return NULL;
    }
    W = allocate_matrix(N, N);
    if(W == NULL){
        free_array_of_pointers(A, N);
        free_array_of_pointers(D, N);
        free_array_of_pointers(D_min_half, N);
        return NULL;
    }
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            W[i][j] = D_min_half[i][i]*A[i][j];
        }
    }
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            W[i][j] = W[i][j]*D_min_half[j][j];
        }
    }
    free_array_of_pointers(D_min_half, N);
    free_array_of_pointers(D, N);
    free_array_of_pointers(A, N);
    return W;
}

/*
 * Function which Transposes matrix.
 * Parameters:
 *   A: matrix to transpose.
 *   rows: number of rows.
 *   cols: number of columns.
 * Returns:
 *   Transposed matrix.
 */
double** transpose_matrix_cust(double** A, int rows, int cols){
    double** At = NULL;
    int i, j;
    At = allocate_matrix(cols, rows);
    if(At == NULL){
        return NULL;
    }
    for(i=0; i < cols; i++){
        for(j=0; j<rows; j++){
            At[i][j] = A[j][i];
        }
    }
    return At;
}

/*
 * Function which multiply matrixes.
 * Parameters:
 *   A: left matrix.
 *   B: right matrix.
 *   rowA: number of rows in A.
 *   rowB: number of rows in B.
 *   colsB: number of columns in B.
 * Returns:
 *   Mult. matrixes.
 */
double** multiply_matrixes_cust(double** A, double** B, int rowA, int rowB, int colB){
    double** AB = NULL;
    int i, j, k;
    AB = allocate_matrix(rowA, colB);
    if(AB == NULL){
        return NULL;
    }
    for (i = 0; i < rowA; i++) {
        for (j = 0; j < colB; j++) {
            for (k = 0; k < rowB; k++) {
                AB[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return AB;
}

/*
 * Function which calculates symnmf matrix.
 * Parameters:
 *   W: similarity matrix.
 *   H: clustering matrix.
 *   N: number of datapoints.
 *   d: number of elements in datapoint.
 * Returns:
 *   symnmf matrix.
 */
double** symnmf_c(double** W, double** H, int N, int K){
    int iter_count = 0;
    double convergance_rate = 1;
    double trace = 0;
    double coef = 0;
    double** H_new = NULL;
    double** H_Diff = NULL;
    double** H_Diff_T = NULL;
    double** E = NULL;
    double** WHt = NULL;
    double** HT = NULL;
    double** C = NULL;
    int i,j;
    while(iter_count < max_iter && convergance_rate >= eps){
        WHt = multiply_matrixes_cust(W, H, N, N, K);
        HT = transpose_matrix_cust(H, N, K);
        C = multiply_matrixes_cust(H, HT, N, K, N);
        C = multiply_matrixes_cust(C, H, N, N, K);
        trace = 0;
        coef = 0;

        H_new = allocate_matrix(N, K);
        if(H_new == NULL){
            if(iter_count > 1){
                free_array_of_pointers(H, N);
            }
            free_array_of_pointers(WHt, N);
            free_array_of_pointers(HT, K);
            free_array_of_pointers(C, N);
            return NULL;
        }
        for(i = 0; i < N; i++){
            for(j = 0; j < K; j++){
                coef = (1-beta)+beta*(WHt[i][j]/C[i][j]);
                H_new[i][j] = H[i][j]*coef;
            }
        }

        H_Diff = allocate_matrix(N, K);
        if(H_Diff == NULL){
            if(iter_count > 1){
                free_array_of_pointers(H, N);
            }
            free_array_of_pointers(WHt, N);
            free_array_of_pointers(HT, K);
            free_array_of_pointers(C, N);
            free_array_of_pointers(H_new, N);
            return NULL;
        }
        for(i = 0; i < N; i++){
            for(j = 0; j < K; j++){
                H_Diff[i][j] = H_new[i][j] - H[i][j];
            }
        }

        H_Diff_T = transpose_matrix_cust(H_Diff, N, K);
        if(H_Diff_T == NULL){
            if(iter_count > 1){
                free_array_of_pointers(H, N);
            }
            free_array_of_pointers(WHt, N);
            free_array_of_pointers(HT, K);
            free_array_of_pointers(C, N);
            free_array_of_pointers(H_new, N);
            free_array_of_pointers(H_Diff, N);
            return NULL;
        }
        E = multiply_matrixes_cust(H_Diff_T, H_Diff, K, N, K);
        if(E == NULL){
            if(iter_count > 1){
                free_array_of_pointers(H, N);
            }
            free_array_of_pointers(WHt, N);
            free_array_of_pointers(HT, K);
            free_array_of_pointers(C, N);
            free_array_of_pointers(H_new, N);
            free_array_of_pointers(H_Diff, N);
            free_array_of_pointers(H_Diff_T, K);
            return NULL;
        }
        for(i = 0; i < K; i++){
            trace += E[i][i];
        }
        convergance_rate = trace;
        iter_count++;
        if(iter_count > 1){
            free_array_of_pointers(H, N);
        }
        free_array_of_pointers(WHt, N);
        free_array_of_pointers(C, N);
        free_array_of_pointers(H_Diff, N);
        free_array_of_pointers(HT, K);
        free_array_of_pointers(H_Diff_T, K);
        free_array_of_pointers(E, K);
        H = H_new;
    }
    return H;
}

/*
 * Function which gets a command and executes it.
 * Parameters:
 *   goal: the command to execute.
 *   file_name: path to file.
 *   N: number of datapoints.
 *   d: number of elements in datapoint.
 * Returns:
 *   result of execution.
 */
int run_command(char* goal, char* file_name, int N, int d){
    double** X = read_file_to_matrix_X(file_name, N, d);
    if(X == NULL){
        return 1;
    }
    if(strcmp(goal,"sym") == 0){
        double** A = sym_c(X, N, d);
        if(A == NULL){
            return 1;
        }
        print_matrix(A, N, N);
        free_array_of_pointers(A, N);
    }
    else if (strcmp(goal,"ddg") == 0)
    {
        double** D = ddg_c(X, N, d);
        if(D == NULL){
            return 1;
        }
        print_matrix(D, N, N);
        free_array_of_pointers(D, N);
    }
    else if (strcmp(goal,"norm") == 0)
    {
        double** W = norm_c(X, N, d);
        if(W == NULL){
            return 1;
        }
        print_matrix(W, N, N);
        free_array_of_pointers(W, N);
    }
    free_array_of_pointers(X, N);
    return 0;
}

int validate_file_exists(char* file_name){
    FILE *f;
    f = fopen(file_name, "r");
    if(f == NULL){
        return 0;
    }
    fclose(f);
    return 1;
}

int main(int argc, char* argv[]){
    int N, d, result, file_exists;
    char* goal = NULL;
    char* file_name = NULL;
    if(argc != 3){
        printf("%s\n", ERROR_MESSAGE);
        return 1;
    }
    goal = argv[1];
    file_name = argv[2];

    file_exists = validate_file_exists(file_name);
    if(file_exists == 0){
        printf("%s\n", ERROR_MESSAGE);
        return 1;
    }

    d = get_number_of_elements_in_file_row(file_name);
    if(d == -1){
        printf("%s\n", ERROR_MESSAGE);
        return 1;
    }
    N = get_number_of_lines_in_file(file_name);
    if(N == -1){
        printf("%s\n", ERROR_MESSAGE);
        return 1;
    }
    result = run_command(goal, file_name, N, d);
    if(result == 1){
        printf("%s\n", ERROR_MESSAGE);
    }
    return result;
}
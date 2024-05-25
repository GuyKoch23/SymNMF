#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define eps 1e-4
#define max_iter 300

size_t getline(char **lineptr, size_t *n, FILE *stream)
    {
        char *bufptr = NULL;
        char *p = bufptr;
        size_t size;
        int c;
    
        if (lineptr == NULL)
        {
            return -1;
        }
        if (stream == NULL)
        {
            return -1;
        }
        if (n == NULL)
        {
            return -1;
        }
        bufptr = *lineptr;
        size = *n;
    
        c = fgetc(stream);
        if (c == EOF)
        {
            return -1;
        }
        if (bufptr == NULL)
        {
            bufptr = malloc(128);
            if (bufptr == NULL)
            {
                return -1;
            }
            size = 128;
        }
        p = bufptr;
        while (c != EOF)
        {
            if ((size_t)(p - bufptr) > (size - 1))
            {
                size = size + 128;
                bufptr = realloc(bufptr, size);
                if (bufptr == NULL)
                {
                    return -1;
                }
                p = bufptr + (size - 128);
            }
            *p++ = c;
            if (c == '\n')
            {
                break;
            }
            c = fgetc(stream);
        }
    
        *p++ = '\0';
        *lineptr = bufptr;
        *n = size;
    
        return p - bufptr - 1;
    }

void free_array_of_pointers(double** arr, int length) {
  if(arr != NULL){
    int i;
    for (i=0; i<length; i++) {
        free(arr[i]);
    }
    free(arr);
    }
}

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

int get_number_of_elements_in_file_row(char* file_name){
    FILE *f;
    size_t len = 0;
    char* line = NULL;
    int num_of_commas = 0;
    f = fopen(file_name, "r");
    if(f == NULL){
        return -1;
    }
    getline(&line, &len, f);
    num_of_commas = 1 + count_commas_in_string(line);
    fclose(f);
    free(line);
    return num_of_commas;
}

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
        getline(&line, &len, f);
        N++;
    }
    fclose(f);
    free(line);
    return N-1;
}

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

double calculate_euclidean_distance(double** mat, int row, int col, int d){
    double sum = 0;
    int k;
    for(k = 0; k < d; k++){
        sum += pow((mat[row][k] - mat[col][k]),2);
    }
    return sum;
}

double** sym_c(double** X, int N, int d){
    int i, j, k;
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

double calculate_vertice_degree(double** mat, int row, int cols){
        int j;
        double current_sum = 0;
        for(j = 0; j <cols; j++){
            current_sum += mat[row][j];
        }
        return current_sum;
}

double** ddg_c(double** X, int N, int d){
    int i, j;
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
                coef = 0.5+0.5*(WHt[i][j]/C[i][j]);
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

int main(int argc, char* argv[]){
    char* goal = NULL;
    char* file_name = NULL;
    int N, d;
    if(argc != 3){
        return 1;
    }
    goal = (char*)malloc(strlen(argv[1]));
    if(goal == NULL){
        return 1;
    }
    file_name = (char*)malloc(strlen(argv[1]));
    if(file_name == NULL){
        return 1;
    }
    strcpy(goal, argv[1]);
    strcpy(file_name, argv[2]);

    d = get_number_of_elements_in_file_row(file_name);
    if(d == -1){
        return 0;
    }
    N = get_number_of_lines_in_file(file_name);
    if(N == -1){
        return 0;
    }
    run_command(goal, file_name, N, d);
    free(goal);
    free(file_name);
    return 0;
}

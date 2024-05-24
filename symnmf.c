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
  int i;
  for (i=0; i<length; i++) {
    free(arr[i]);
  }
  free(arr);
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

int get_N_size(char* file_name){
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

double** file_to_matrix_X(char* file_name, int N, int d){
    double **X = NULL;
    FILE *f;
    size_t len = 0;
    char* line = NULL;
    char *start_iterator = NULL;
    char *end_iterator = NULL;
    int d_counter = 0;
    int line_count = 0;
    X = (double**)malloc(N*sizeof(double *));

    if(X == NULL){
        return NULL;
    }
    f = fopen(file_name, "r");
    if(f == NULL){
        return NULL;
    }

    while ((int)getline(&line, &len, f) != -1 && line_count < N){
        X[line_count] = (double*)malloc(d*sizeof(double));
        
        if (X[line_count] == NULL) {
            free_array_of_pointers(X, line_count);
            free(line);
            return NULL;
        }

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
    if (X == NULL) {
      printf("An Error Has Occurred");
      return NULL;
    }
    free(line);
    fclose(f);
    return X;
}


double** sym_c(double** X, int N, int d){
    int i, j, k;
    double** A = NULL;
    double current_sum = 0;
    double current_exp = 0;
    A = (double**)malloc(N*sizeof(double*));
    if(A == NULL){
        return NULL;
    }
    for(i = 0 ; i < N; i++){
        A[i] = (double*)malloc(N*sizeof(double));
        if(A[i] == NULL){
            return NULL;
        }   
        for(j = 0; j < N; j++){
            if(i == j){
                A[i][j] = 0;
            }
            else{
                current_sum = 0;
                for(k = 0; k < d; k++){
                    current_sum += pow((X[i][k] - X[j][k]),2);
                }
                current_exp = exp((-0.5)*current_sum);
                A[i][j] = current_exp;
            }
        }
    }
    return A;
}

double** ddg_c(double** X, int N, int d){
    int i, j;
    double current_sum;
    double **A, **D;
    A = sym_c(X, N, d);
    if(A == NULL){
        return NULL;
    }
    D = (double**)calloc(N,sizeof(double*));
    if(D == NULL){
        return NULL;
    }
    for(i = 0; i < N; i++){
        D[i] = (double*)calloc(N,sizeof(double));
        if(D[i] == NULL){
            return NULL;
        }
        current_sum = 0;
        for(j = 0; j <N; j++){
            current_sum += A[i][j];
        }
        D[i][i] = current_sum;
    }
    free_array_of_pointers(A, N);
    return D;
}

double** diag_pow_minus_half(double** mat, int N){
    int i;
    double** min_half_diag = (double**)calloc(N,sizeof(double*));
    if(min_half_diag == NULL){
        return NULL;
    }
    for(i = 0; i < N; i++){
        min_half_diag[i] = (double*)calloc(N,sizeof(double));
        if(min_half_diag[i] == NULL){
            return NULL;
        }
        min_half_diag[i][i] = pow(mat[i][i], -0.5);
    }
    return min_half_diag;
}

double** norm_c(double** X, int N, int d){
    int i, j;
    double **A, **D, **D_min_half, **W;
    A = sym_c(X, N, d);
    D = ddg_c(X, N, d);
    if(A == NULL || D == NULL){
        return NULL;
    }
    D_min_half = diag_pow_minus_half(D, N);
    W = (double**)calloc(N,sizeof(double*));
    if(W == NULL){
        return NULL;
    }
    for(i = 0; i < N; i++){
        W[i] = (double*)calloc(N,sizeof(double));
        if(W[i] == NULL){
            return NULL;
        }
        for(j = 0; j < N; j++){
            W[i][j] = D_min_half[i][i]*A[i][j];
        }
    }
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            W[i][j] = W[i][j]*D_min_half[j][j];
        }
    }
    free_array_of_pointers(D, N);
    free_array_of_pointers(A, N);
    return W;
}

double** transpose_matrix_cust(double** A, int row, int col){
    double** At = (double**)calloc(col,sizeof(double*));
    int i, j;
    if(At == NULL){
        return NULL;
    }
    for (i = 0; i < col; i++) {
        At[i] = (double*)calloc(row,sizeof(double));
        if(At[i] == NULL){
            return NULL;
        }
        for (j = 0; j < row; j++){
            At[i][j] = A[j][i];
        }
    }
    return At;
}

double** multiply_matrixes_cust(double** A, double** B, int rowA, int rowB, int colB){
    double** AB = (double**)calloc(rowA,sizeof(double*));
    int i, j, k;
    if(AB == NULL){
        return NULL;
    }
    for (i = 0; i < rowA; i++) {
        AB[i] = (double*)calloc(colB,sizeof(double));
        if(AB[i] == NULL){
            return NULL;
        }
        for (j = 0; j < colB; j++) {
            AB[i][j] = 0;
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
        H_new = (double**)calloc(N, sizeof(double*));
        if(H_new == NULL){
            return NULL;
        }
        for(i = 0; i < N; i++){
            H_new[i] = (double*)calloc(K, sizeof(double));
            if(H_new[i] == NULL){
                return NULL;
            }
            for(j = 0; j < K; j++){
                coef = 0.5+0.5*(WHt[i][j]/C[i][j]);
                H_new[i][j] = H[i][j]*coef;
            }
        }
        H_Diff = (double**)calloc(N, sizeof(double*));
        if(H_Diff == NULL){
            return NULL;
        }
        for(i = 0; i < N; i++){
            H_Diff[i] = (double*)calloc(K, sizeof(double));
            if(H_Diff[i] == NULL){
                return NULL;
            }
            for(j = 0; j < K; j++){
                H_Diff[i][j] = H_new[i][j] - H[i][j];
            }
        }
        H_Diff_T = transpose_matrix_cust(H_Diff, N, K);
        E = multiply_matrixes_cust(H_Diff_T, H_Diff, K, N, K);
        for(i = 0; i < K; i++){
            trace += E[i][i];
        }
        convergance_rate = trace;
        iter_count++;
        if(iter_count > 1){
            free_array_of_pointers(H, K);
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
    double** X = file_to_matrix_X(file_name, N, d);
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
    N = get_N_size(file_name);

    run_command(goal, file_name, N, d);
    free(goal);
    free(file_name);
    return 0;
}

#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SYM "sym"
#define DDG "ddg"
#define NORM "norm"

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
            if ((p - bufptr) > (size - 1))
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

void free_matrix(double** mat, int rows, int cols){
    for(int i = 0; i < rows; i++){
        free_array_of_pointers(mat[i], cols);
    }
}

int commas_in_str(char* str){
    int count = 0;
    while(*str != '\0'){
        if(*str == ','){
            count++;
        }
        str++;
    }
    return count;
}

int get_d_size(char* file_name){
    FILE *f;
    size_t len = 0;
    ssize_t read;
    char* line = NULL;
    int d;
    f = fopen(file_name, "r");
    if(f == NULL){
        return -1;
    }
    read = getline(&line, &len, f);
    d = 1 + commas_in_str(line);
    fclose(f);
    free(line);
    return d;
}

int get_N_size(char* file_name){
    FILE *f;
    size_t len = 0;
    ssize_t read;
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
    return N;

}

double** file_to_matrix_X(char* file_name, int N, int d){
    double **X = NULL;
    FILE *f;
    size_t len = 0;
    ssize_t read;
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

    while (getline(&line, &len, f) != -1 && line_count < N){
        
        X[line_count] = (double*)malloc(d*sizeof(double)); // allocate memory for line numbers
        
        if (X[line_count] == NULL) { // memory allocation failed
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

void print_output(double** mat, int N, int d) {
  int i, j;

  for (i=0; i<N; i++) {
    for (j=0; j<d; j++) {
      printf("%.4f",mat[i][j]);
      if (j != d-1)
        printf(",");
      else
        printf("\n");
    }
  }
}


double** sym(char* goal, char* file_name, int N, int d){
    double** X = file_to_matrix_X(file_name, N, d);
    if(X == NULL){
        return NULL;
    }
    double** A = NULL;
    double current_sum = 0;
    double current_exp = 0;
    A = (double**)malloc(N*sizeof(double*));
    if(A == NULL){
        return NULL;
    }
    for(int i = 0 ; i < N; i++){
        A[i] = (double*)malloc(N*sizeof(double));
        if(A[i] == NULL){
            return NULL;
        }   
        for(int j = 0; j < N; j++){
            if(i == j){
                A[i][j] = 0;
            }
            else{
                current_sum = 0;
                for(int k = 0; k < d; k++){
                    current_sum += pow((X[i][k] - X[j][k]),2);
                }
                current_exp = exp((-0.5)*current_sum);
                A[i][j] = current_exp;
            }
        }
    }
    free_matrix(X, N, d);
    return A;
}

double** ddg(char* goal, char* file_name, int N, int d){
    double** A = sym(goal, file_name, N, d);
    if(A == NULL){
        return NULL;
    }
    double** D = NULL;
    double current_sum = 0;
    D = (double**)calloc(N,sizeof(double*));
    if(D == NULL){
        return NULL;
    }
    for(int i = 0; i < N; i++){
        D[i] = (double*)calloc(N,sizeof(double));
        if(D[i] == NULL){
            return NULL;
        }
        current_sum = 0;
        for(int j = 0; j <N; j++){
            current_sum += A[i][j];
        }
        D[i][i] = current_sum;
    }
    free_matrix(A, N, N);
    return D;
}

double** diag_pow_minus_half(double** mat, int N){
    double** min_half_diag = (double**)calloc(N,sizeof(double*));
    if(min_half_diag == NULL){
        return NULL;
    }
    for(int i = 0; i < N; i++){
        min_half_diag[i] = (double*)calloc(N,sizeof(double));
        if(min_half_diag[i] == NULL){
            return NULL;
        }
        min_half_diag[i][i] = pow(mat[i][i], -0.5);
    }
    return min_half_diag;
}

double** norm(char* goal, char* file_name, int N, int d){
    double** D = ddg(goal, file_name, N, d);
    if(D == NULL){
        return NULL;
    }
    double** D_min_half = diag_pow_minus_half(D, N);
    double** W = (double**)calloc(N,sizeof(double*));
    if(W == NULL){
        return NULL;
    }
    for(int i = 0; i < N; i++){
        W[i] = (double*)calloc(N,sizeof(double));
        if(W[i] == NULL){
            return NULL;
        }
        for(int j = 0; j < N; j++){
            W[i][j] = D_min_half[i][i]*A[i][j];
        }
    }
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            W[i][j] = W[i][j]*D_min_half[j][j];
        }
    }
    free_matrix(D, N, N);
    return W;
}

double** transpose_matrix_cust(double** A, int row, int col){
    double** At = (double**)calloc(row,sizeof(double*));
    if(At == NULL){
        return NULL;
    }
    for (int i = 0; i < row; i++) {
        At[i] = (double**)calloc(row,sizeof(double));
        if(At[i] == NULL){
            return NULL;
        }
        for (int j = 0; j < col; j++){
            At[i][j] = A[j][i];
        }
    }
    return At;
}

double** multiple_matrixes_cust(double** A, double** B, int rowA, int colA, int rowB, int colB){
    double** AB = (double**)calloc(rowA,sizeof(double*));
    if(AB == NULL){
        return NULL;
    }
    for (int i = 0; i < rowA; i++) {
        AB[i] = (double**)calloc(colB,sizeof(double));
        if(AB[i] == NULL){
            return NULL;
        }
        for (int j = 0; j < colB; j++) {
            AB[i][j] = 0;
            for (int k = 0; k < rowB; k++) {
                AB[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return AB;
}

double** symnmf(double** H, double** W, double eps, int max_iter, int N, int K){
    int iter_count = 0;
    int convergance_rate = eps+1;
    int coef = 0;
    int trace = 0;
    double** H_new = NULL;
    double** H_Diff = NULL;
    double** H_Diff_T = NULL;
    double** E = NULL;
    double** WHt = NULL;
    double** HT = NULL;
    double** C = NULL;
    while(iter_count < max_iter && convergance_rate >= eps){
        WHt = multiple_matrixes_cust(W, H, N, N, K, N);
        HT = transpose_matrix_cust(H, N, K);
        C = multiple_matrixes_cust(H, HT, N, K, K, N);
        trace = 0;
        H_new = (double**)calloc(N, sizeof(double*));
        if(H_new == NULL){
            return NULL;
        }
        C = multiple_matrixes_cust(C, H, N, N, N, K);
        //Updating H
        for(int i = 0; i < N; i++){
            H_new[i] = (double**)calloc(K, sizeof(double));
            if(H_new[i] == NULL){
                return NULL;
            }
            for(int j = 0; j < K; j++){
                H_new[i][j] = H[i][j]*((1-0.5)+0.5*(WHt[i][j]/C[i][j]));
            }
        }
        //Checking for convergance
        H_Diff = (double**)calloc(N, sizeof(double*));
        if(H_Diff == NULL){
            return NULL;
        }
        for(int i = 0; i < N; i++){
            H_Diff[i] = (double**)calloc(N, sizeof(double*));
            if(H_Diff[i] == NULL){
                return NULL;
            }
            for(int j = 0; j < K; j++){
                H_Diff[i][j] = H_new[i][j] - H[i][j];
            }
        }
        H_Diff_T = transpose_matrix_cust(H_Diff, N, K);
        E = multiple_matrixes_cust(H_Diff_T, H_Diff, K, N, N, K);
        for(int i = 0; i < K; i++){
            trace += E[i][i];
        }
        convergance_rate = trace;
        iter_count++;
        for(int i = 0; i < N; i++){
            if(iter_count > 1){ // we dont want to free the original H
                free_array_of_pointers(H[i], K);
            }
            free_array_of_pointers(WHt[i], N);
            free_array_of_pointers(C[i], N);
            free_array_of_pointers(H_Diff[i], K);
        }
        for(int i = 0; i < K; i++){
            free_array_of_pointers(HT[i], N);
            free_array_of_pointers(H_Diff_T[i], N);
            free_array_of_pointers(E[i], K);
        }
        H = H_new;
    }
    return H;
}

int run_command(char* goal, char* file_name, int N, int d){

    if(goal == "sym"){
        double** A = sym(goal, file_name, N, d);
        print_output(A, N, d);
        free_matrix(A, N, N);
    }
    else if (goal == "ddg")
    {
        double** D = ddg(goal, file_name, N, d);
        print_output(D, N, d);
        free_matrix(D, N, N);
    }
    else if (goal == "norm")
    {
        double** W = norm(goal, file_name, N, d);
        print_output(W, N, d);
        free_matrix(W, N, N);
    }
}

int main(int argc, char* argv[]){
    char *goal = (char*)NULL;
    char *file_name = (char*)NULL;
    double **X;
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

    int d = get_d_size(file_name);
    int N = get_N_size(file_name);

    run_command(goal, file_name, N, d);
    free(goal);
    free(file_name);
}

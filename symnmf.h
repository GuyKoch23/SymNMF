#include "symnmf.c"


double** sym_c(double** X, int N, int d);
double** ddg_c(double** X, int N, int d);
double** norm_c(double** X, int N, int d);
double** symnmf_c(double** H, double** W, double eps, int max_iter, int N, int K);
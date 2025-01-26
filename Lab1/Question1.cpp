#include <hls_stream.h>

#define lm 6
#define ln 6
#define lp 6

#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp)

void MATRIX_MUL(int A[n][m], int B[m][p], int C[n][p]) {

    #pragma HLS ARRAY_PARTITION variable=A complete dim=2
    #pragma HLS ARRAY_PARTITION variable=B complete dim=1
    #pragma HLS ARRAY_PARTITION variable=C complete dim=2


 for (int i = 0; i < n+1; i++) {
        for (int j = 0; j <p+1; j++) {
#pragma HLS PIPELINE  II=1
            for (int k = 0; k < m+1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }

        }
    }
}

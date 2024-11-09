#include <hls_stream.h>
//estw exoyme autes tis dunameis tou 2
#define lm 6
#define ln 6
#define lp 6
//shift aristera gia na paroume to dim twn pinakwn afou einai vasismenoi se 2^2 gt p.x to 2^2=4  ara to lm=2 =0010 ena <shift 0100=4
#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp)

void MATRIX_MUL(int A[m][n], int B[n][p], int C[m][p]) {
    // HLS PRAGMA directives gia optimization
    #pragma HLS ARRAY_PARTITION variable=A complete dim=2
    #pragma HLS ARRAY_PARTITION variable=B complete dim=1
    #pragma HLS ARRAY_PARTITION variable=C complete dim=2


#pragma HLS UNROLL
 for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < n; k++) {
            	//reduces loop overhead
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

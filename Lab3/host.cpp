#include <ap_int.h>
#include <stdio.h>
#include <string.h>
#include <hls_stream.h>
#define lm 6
#define ln 6
#define lp 6
#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp )

#define BUFFER_SIZE 64
#define DATAWIDTH 512
#define VECTOR_SIZE (DATAWIDTH / 32) // vector size is 16 (512/32 = 16)
typedef ap_uint<DATAWIDTH> uint512_dt;

//TRIPCOUNT identifier
const unsigned int c_chunk_sz = BUFFER_SIZE;
const unsigned int c_size     = VECTOR_SIZE;

/*
    Vector Addition Kernel Implementation using uint512_dt datatype
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */

extern "C" {
    void vadd(
        const uint512_dt *in1,  // Read-Only Matrix A (M x N)
        const uint512_dt *in2,  // Read-Only Matrix B (N x P)
        uint512_dt *out,        // Output Matrix C (M x P)
        int N,                  // Number of rows in Matrix A
        int M,                  // Number of columns in Matrix A (and rows in Matrix B)
        int P                   // Number of columns in Matrix B
    )
    {
        #pragma HLS INTERFACE m_axi port = in1 bundle = gmem
        #pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
        #pragma HLS INTERFACE m_axi port = out bundle = gmem2
        #pragma HLS INTERFACE s_axilite port = in1 bundle = control
        #pragma HLS INTERFACE s_axilite port = in2 bundle = control
        #pragma HLS INTERFACE s_axilite port = out bundle = control
        #pragma HLS INTERFACE s_axilite port = M bundle = control
        #pragma HLS INTERFACE s_axilite port = N bundle = control
        #pragma HLS INTERFACE s_axilite port = P bundle = control
        #pragma HLS INTERFACE s_axilite port = return bundle = control

        uint512_dt v1_local[BUFFER_SIZE];  // Local memory to store rows of Matrix A
        uint512_dt v2_local[BUFFER_SIZE];  // Local memory to store columns of Matrix B
        uint512_dt result_local[BUFFER_SIZE]; // Local memory for result

        // Loop over rows of Matrix A and columns of Matrix B
        for (int i = 0; i < N; i++) {
            #pragma HLS DATAFLOW
            #pragma HLS stream variable = v1_local depth = 64
            #pragma HLS stream variable = v2_local depth = 64

            // Load a row from Matrix A into v1_local
            for (int j = 0; j < M; j++) {
                #pragma HLS pipeline
                v1_local[j] = in1[i * N + j]; // Row i of Matrix A
            }

            // For each column of Matrix B
            for (int j = 0; j < P; j++) {
                #pragma HLS pipeline
                // Load the column of Matrix B into v2_local
                for (int k = 0; k < M; k++) {
                    v2_local[k] = in2[k * P + j]; // Column j of Matrix B
                }

                uint512_dt tmpOut = 0;
                // Compute the dot product of row i of A and column j of B
                for (int k = 0; k < M; k++) {
                    #pragma HLS UNROLL
                    ap_uint<32> tmp1 = v1_local[k].range(32 * (k + 1) - 1, k * 32);  // 32-bit chunk from row of A
                    ap_uint<32> tmp2 = v2_local[k].range(32 * (k + 1) - 1, k * 32);  // 32-bit chunk from column of B

                    tmpOut.range(32 * (k + 1) - 1, k * 32) = tmp1 * tmp2; // Multiply and store result
                }

                out[i * P + j] = tmpOut;  // Store the result in the output matrix
            }
        }
    }
}

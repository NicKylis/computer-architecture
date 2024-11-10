#include <stdio.h>
#include <stdlib.h>

#define lm 6
#define ln 6
#define lp 6

#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp)

void MATRIX_MUL(int A[m][n], int B[n][p], int C[m][p]);
void software_matrix_mul(int A[m][n], int B[n][p], int C[m][p]);

int main() {
 int comp=0;
    int A[m][n];
    int B[n][p];
    int C[m][p];
    int D[m][p];



    //Αrray initialization
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = (rand() % 256);
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            B[i][j] =  (rand() % 256);
        }
    }
    software_matrix_mul(A,B,D);

    MATRIX_MUL(A, B, C);

    //Check and flag any mistakes
    for (int i = 0; i < m; i++) {
           for (int j = 0; j < n; j++) {
        	    if(C[i][j]!=D[i][j]){
        	    	comp+=comp+1;
        	    }
        	    else{
        	    	break;
        	    }
           }
    }
     if(comp==0){
    	 printf("Test Passed! ");
     }
     else{
    	 printf("Test failed!");
     }
    return 0;
}

//Simple Matrix multiplication (A * B = C) without any accelaration, for testing
void software_matrix_mul(int A[m][n], int B[n][p], int C[m][p]) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

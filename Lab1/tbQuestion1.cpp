#include <stdio.h>
#include<stdlib.h>

#define lm 6
#define ln 6
#define lp 6

#define m (1 << lm)
#define n (1 << ln)
#define p (1 << lp)

void MATRIX_MUL(int A[n][m], int B[m][p], int C[n][p]);

void software_matrix_mul(int A[n][m], int B[m][p], int C[n][p]);


int main() {
 int comp=0;
    int A[n][m];
    int B[m][p];
    int C[n][p];
    int D[n][p];



  printf("Print A");
  printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            A[i][j]=rand() % 256;
            printf("%d  ",A[i][j]);
        }
    printf("\n");
    }

    printf("\n");
    printf("Print B\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            B[i][j]=rand() % 256;
            printf("%d  ",B[i][j]);

        }
        printf("\n");
    }
    printf("\n");


   software_matrix_mul(A,B,D);

    MATRIX_MUL(A,B,C);

  printf("Matrix C \n");
    for(int i=0;i<n;i++){
   		 for(int j=0;j<p;j++){
   			 printf("%d ",C[i][j]);
   		 }
   		 printf("\n");
   	 }
    printf("\n");

    printf("Matrix D \n");
    for(int i=0;i<n;i++){
     		 for(int j=0;j<p;j++){
     			 printf("%d ",D[i][j]);
     		 }
     		 printf("\n");
     	 }



    for (int i = 0; i < n; i++) {
           for (int j = 0; j <p ; j++) {
        	    if(C[i][j]!=D[i][j]){
        	    	comp=comp+1;
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
    	 printf("Test Failed!");
     }
    return 0;
}

void software_matrix_mul(int A[n][m], int B[m][p], int C[n][p]) {

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < m; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}


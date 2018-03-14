/*
OpenBLAS hello world, initially adapted from:
https://stackoverflow.com/questions/49227682/gem5-can-not-simulate-my-program-that-calls-openblas-functions-with-an-fatal-err
*/

#include <cblas.h>
#include <stdio.h>

int main() {
	size_t i = 0;
	double A[6] = {1.0,2.0,1.0,-3.0,4.0,-1.0};
	double B[6] = {1.0,2.0,1.0,-3.0,4.0,-1.0};
	double C[9] = {.5,.5,.5,.5,.5,.5,.5,.5,.5};
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,3,3,2,1,A, 3, B, 3,2,C,3);
	for(i = 0; i < 9; i++)
		printf("%f ", C[i]);
	printf("\n");
}

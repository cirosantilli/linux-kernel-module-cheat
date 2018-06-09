/* Adapted from: https://github.com/xianyi/OpenBLAS/wiki/User-Manual/59b62f98e7400270fb03ad1d85fba5b64ebbff2b#call-cblas-interface */

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

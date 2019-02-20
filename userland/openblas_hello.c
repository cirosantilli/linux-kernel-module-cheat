/* https://github.com/cirosantilli/linux-kernel-module-cheat#blas
 * Adapted from: https://github.com/xianyi/OpenBLAS/wiki/User-Manual/59b62f98e7400270fb03ad1d85fba5b64ebbff2b#call-cblas-interface */

#include "lkmc.h"

#include <assert.h>
#include <cblas.h>

int main(void) {
    double A[6] = {1.0, 2.0, 1.0, -3.0, 4.0, -1.0};
    double B[6] = {1.0, 2.0, 1.0, -3.0, 4.0, -1.0};
    double C[9] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans, 3, 3, 2, 1, A, 3, B, 3, 2, C, 3);
    assert(lkmc_vector_equal(9, C, (double[]){11.0, -9.0, 5.0, -9.0, 21.0, -1.0, 5.0, -1.0, 3.0}, 1e-6));
}

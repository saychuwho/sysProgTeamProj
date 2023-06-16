// DGIST, System Programming
// A deadly simple matmul program
#include <stdio.h>
#include <stdlib.h>

#define N 128

int main() {
    float* A = (float*)malloc(sizeof(float)*N*N);
    float* B = (float*)malloc(sizeof(float)*N*N);
    float* C = (float*)malloc(sizeof(float)*N*N);

    for (int tt = 0; tt < 100; ++tt) {
        for (int k = 0; k < N; ++k) {
            for (int i = 0; i < N; ++i) {
                float r = A[i * N + k];
                for (int j = 0; j < N; ++j) {
                    C[i*N + j] += r * B[k * N + j];
                }
            }
        }
    }

    free(A);
    free(B);
    free(C);

    return 0;
}

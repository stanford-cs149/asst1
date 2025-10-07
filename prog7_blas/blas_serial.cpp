#include <thread>
#include <algorithm>
#include <stdio.h>

#include "blas_ispc.h"

using namespace ispc;

void axpySerial(
    int n,
    float a,
    float x[],
    float y[],
    float result[]
) {
    for (int i = 0; i < n; i++) {
        result[i] = a * x[i] + y[i];
    }
}

void AxSerial(
    int m,
    int n,
    float A[],
    float x[],
    float result[]
) {
    for (int i = 0; i < m; i++) {
        float temp = 0.0f;
        for (int j = 0; j < n; j++) {
            temp += A[i*n + j] * x[j];
        }
        result[i] = temp;
    }
}

typedef struct {
    int threadIndex, threadCount;
    int m, n;
    float* buffer;
    float* result;
} AxIspc2Arg;

void AxIspc2Thread(AxIspc2Arg* const args) {
    int span = args->m / args->threadCount;
    int rowStart = args->threadIndex * span;
    int rowStop = std::min(rowStart + span, args->m);
    for (int i = rowStart; i < rowStop; i++) {
        float temp = 0.0f;
        for (int j = 0; j < args->n; j++) {
            temp += args->buffer[i*args->n + j];
        }
        args->result[i] = temp;
    }
}

void AxIspc(
    int tasks,
    int m,
    int n,
    float A[],
    float x[],
    float result[]
) {
    // the multiplication of A[i] and x can be SIMD-parallelized, because
    // the elements have adjacent addresses in memory
    float * buffer = new float[m*n];
    AxIspc1(tasks, m, n, A, x, buffer);
    
    // the summation of the multiplication, however, can't be SIMD-parallelized,
    // because different lanes should work on different rows, whose elements
    // don't have adjacent addresses in memory. so we use thread-parallelism instead
    AxIspc2Arg* args = new AxIspc2Arg[tasks];
    for (int i = 0; i < tasks; i++) {
        args[i].threadIndex = i;
        args[i].threadCount = tasks;
        args[i].m = m;
        args[i].n = n;
        args[i].buffer = buffer;
        args[i].result = result;
    }
    std::thread* threads = new std::thread[tasks];
    for (int i = 0; i < tasks; i++) {
        threads[i] = std::thread(AxIspc2Thread, &args[i]);
    }
    for (int i = 0; i < tasks; i++) {
        threads[i].join();
    }
    
    delete[] threads;
    delete[] args;
    delete[] buffer;
}

void aAxpySerial(
    int m,
    int n,
    float a,
    float A[],
    float x[],
    float y[],
    float result[]
) {
    float* buffer = new float[m];
    AxSerial(m, n, A, x, buffer);

    for (int i = 0; i < m; i++) {
        result[i] = a * buffer[i] +  y[i];
    }

    delete[] buffer;
}

void aAxpyIspc(
    int tasks,
    int m,
    int n,
    float a,
    float A[],
    float x[],
    float y[],
    float result[]
) {
    float* buffer = new float[m];
    AxIspc(tasks, m, n, A, x, buffer);

    axpyIspc(tasks, m, a, buffer, y, result);

    delete[] buffer;
}

void ABSerial(
    int m,
    int n,
    int l,
    float A[], // mxn
    float B[], // nxl
    float result[] // mxl; NOTE: assumes result has been zeroed
) {
    /**
     * While looping through the third dimension, the first two dimensions are
     * locked, which pins an element in the matrix A/B/C:
     * - if i & j are locked:
     *   - then A[i,j] is pinned, and the third dimension k traverses B & C
     *     horizontally
     * - if j & k are locked:
     *   - then B[j,k] is pinned, and the third dimension i traverses A & C
     *     vertically
     * - if i & k are locked:
     *   - then C[i,k] is pinned, and the thid dimension j traverses A
     *     horizontally and B vertically
     * Since we represent matrix as a 1D array, based on the memory access pattern,
     * the performance should rank as follows:
     * - locking A[i,j]
     * - locking C[i,k]
     * - locking B[j,k]
     * Between the two implementations of locking i & j (i => j, or j => i),
     * since traversal of i is vertical and traversal of j is horizontal,
     * it's better to traverse j in the second loop, and i in the first loop
     */
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float a = A[i*n + j];
            for (int k = 0; k < l; k++) {
                result[i*l + k] += a * B[j*l + k];
            }
        }
    }
}

void aABpbCSerial(
    int m,
    int n,
    int l,
    float a,
    float A[],
    float B[],
    float b,
    float C[],
    float result[]
) {
    ABSerial(m, n, l, A, B, result);
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < l; k++) {
            int loc = i*l + k;
            result[loc] = a * result[loc] + b * C[loc];
        }
    }
}

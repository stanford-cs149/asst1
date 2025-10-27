#include <stdio.h>
#include <algorithm>

#include "CycleTimer.h"
#include "blas_ispc.h"

using namespace ispc;

const int TASK_COUNT = 50;

static float toBW(int bytes, float sec) {
    return static_cast<float>(bytes) / (1024. * 1024. * 1024.) / sec;
}

static float toGFLOPS(int ops, float sec) {
    return static_cast<float>(ops) / 1e9 / sec;
}

static void verifyResult(int N, float* result, float* gold) {
    for (int i=0; i<N; i++) {
        if (result[i] != gold[i]) {
            printf("Error: [%d] Got %f expected %f\n", i, result[i], gold[i]);
        }
    }
}

void axpySerial(int n, float a, float x[], float y[], float result[]);

void axpy() {
    double startTime;
    double endTime;
    double minTime;

    /**
     * Experiment: ax+y
     */
    const int n = 100 * 1000 * 1000;
    int axpyBytes = 4 * n * sizeof(float);
    int axpyFlops = 2 * n;
    float a = 1.0f;
    float* x = new float[n];
    float* y = new float[n];
    for (int i = 0; i < n; i++) {
        x[i] = i;
        y[i] = n-i;
    }

    // Run serial implementation
    float* resultSerial = new float[n];
    for (int i = 0; i < n; i++) {
        resultSerial[i] = 0.f;
    }
    minTime = 1e30;
    for (int i = 0; i < 3; i++) {
        startTime = CycleTimer::currentSeconds();
        axpySerial(n, a, x, y, resultSerial);
        endTime = CycleTimer::currentSeconds();
        minTime = std::min(minTime, endTime - startTime);
    }
    printf(
        "[axpy serial]:\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
        minTime * 1000,
        toBW(axpyBytes, minTime),
        toGFLOPS(axpyFlops, minTime)
    );

    // Run ispc implementation
    float* resultIspc = new float[n];
    for (int i = 0; i < n; i++) {
        resultIspc[i] = 0.f;
    }
    minTime = 1e30;
    for (int i = 0; i < 3; i++) {
        startTime = CycleTimer::currentSeconds();
        axpyIspc(TASK_COUNT, n, a, x, y, resultIspc);
        endTime = CycleTimer::currentSeconds();
        minTime = std::min(minTime, endTime - startTime);
    }
    printf(
        "[axpy ispc]:\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
        minTime * 1000,
        toBW(axpyBytes, minTime),
        toGFLOPS(axpyFlops, minTime)
    );
    verifyResult(n, resultIspc, resultSerial);

    delete[] x;
    delete[] y;
    delete[] resultSerial;
    delete[] resultIspc;
}

extern void aAxpySerial(int m, int n, float a, float A[], float x[], float y[], float result[]);

extern void aAxpyIspc(int tasks, int m, int n, float a, float A[], float x[], float y[], float result[]);

void aAxpy() {
    double startTime;
    double endTime;
    double minTime;

    /**
     * Experiment: aAx + y
     */
    const int m = 10 * 1000;
    const int n = 10 * 1000;

    float a = 0.5f;
    float* A = new float[m*n];
    float* x = new float[n];
    float* y = new float[m];
    for (int j = 0; j < n; j++) {
        x[j] = j+1;
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            A[i*n + j] = 1.0f/(j+1);
        }
        y[i] = m/2.0f;
    }

    // Run serial implementation
    float* resultSerial = new float[m];
    for (int i = 0; i < m; i++) {
        resultSerial[i] = 0.f;
    }
    minTime = 1e30;
    for (int i = 0; i < 3; i++) {
        startTime = CycleTimer::currentSeconds();
        aAxpySerial(m, n, a, A, x, y, resultSerial);
        endTime = CycleTimer::currentSeconds();
        minTime = std::min(minTime, endTime - startTime);
    }
    printf("[aAxpy serial]:\t[%.3f] ms\n", minTime * 1000);

    // Run ispc implementation
    float* resultIspc = new float[m];
    for (int i = 0; i < m; i++) {
        resultIspc[i] = 0.f;
    }
    minTime = 1e30;
    for (int i = 0; i < 3; i++) {
        startTime = CycleTimer::currentSeconds();
        aAxpyIspc(TASK_COUNT, m, n, a, A, x, y, resultIspc);
        endTime = CycleTimer::currentSeconds();
        minTime = std::min(minTime, endTime - startTime);
    }
    printf("[aAxpy ispc]:\t[%.3f] ms\n", minTime * 1000);
    verifyResult(n, resultIspc, resultSerial);

    delete[] A;
    delete[] x;
    delete[] y;
    delete[] resultSerial;
    delete[] resultIspc;
}

extern void aABpbCSerial(int m, int n, int l, float a, float A[], float B[], float b, float C[], float result[]);

void aABpbC() {
    double startTime;
    double endTime;
    double minTime;

    /**
     * Experiment: aAB + bC
     */
    const int m = 2 * 1000;
    const int n = 2 * 1000;
    const int l = 2 * 1000;

    float a = 1.0f;
    float b = 1.0f;
    float* A = new float[m*n];
    float* B = new float[n*l];
    float* C = new float[m*l];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            A[i*n + j] = i+1;
        }
    }
    for (int i = 0; i < n*l; i++) {
        B[i] = 1.0f;
    }
    for (int i = 0; i < m*l; i++) {
        C[i] = 1.0f;
    }
    
    // Run serial implementation
    float* resultSerial = new float[m*l];
    minTime = 1e30;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < m*l; j++) {
            resultSerial[j] = 0.f;
        }
        startTime = CycleTimer::currentSeconds();
        aABpbCSerial(m, n, l, a, A, B, b, C, resultSerial);
        endTime = CycleTimer::currentSeconds();
        minTime = std::min(minTime, endTime - startTime);
    }
    printf("[aABpbC serial]:[%.3f] ms\n", minTime * 1000);

    // Run ispc implementation
    float* resultIspc = new float[m*l];
    minTime = 1e30;    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < m*l; j++) {
            resultIspc[j] = 0.f;
        }
        startTime = CycleTimer::currentSeconds();
        aABpbCIspc(TASK_COUNT, m, n, l, a, A, B, b, C, resultIspc);
        endTime = CycleTimer::currentSeconds();
        minTime = std::min(minTime, endTime - startTime);
    }
    printf("[aABpbC ispc]:\t[%.3f] ms\n", minTime * 1000);
    verifyResult(n, resultIspc, resultSerial);

    delete[] resultIspc;
    delete[] resultSerial;
    delete[] C;
    delete[] B;
    delete[] A;
}

int main() {
    // source: https://en.wikipedia.org/wiki/Basic_Linear_Algebra_Subprograms#Functionality
    // implement storage and cache-optimized blas algorithms
    axpy();
    aAxpy();
    aABpbC();
}
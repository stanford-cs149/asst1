#include <thread>

void saxpySerial(int N,
                       float scale,
                       float X[],
                       float Y[],
                       float result[])
{

    for (int i=0; i<N; i++) {
        result[i] = scale * X[i] + Y[i];
    }
}

typedef struct {
    int start, span;
    float scale;
    float* X;
    float* Y;
    float* result;
} WorkerArgs;

void workerThreadStart(WorkerArgs* const args) {
    int stop = args->start + args->span;
    float scale = args->scale;
    float* x = args->X;
    float* y = args->Y;
    float* result = args->result;
    for (int i = args->start; i < stop; i++) {
        result[i] = scale * x[i] + y[i];
    }
}

void saxpyThread(int N,
                        float scale,
                        float X[],
                        float Y[],
                        float result[])
{

    static constexpr int threads = 64;
    int span = N/threads;
    std::thread workers[threads];
    WorkerArgs args[threads];

    for (int i = 0; i < threads; i++) {
        args[i].start = i * span;
        args[i].span = span;
        args[i].scale = scale;
        args[i].X = X;
        args[i].Y = Y;
        args[i].result = result;
    }

    for (int i = 1; i < threads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    workerThreadStart(&args[0]);

    for (int i = 1; i < threads; i++) {
        workers[i].join();
    }
}


#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    /* from Approach 1 where we compute numThreads horizontal chunks
    unsigned int delta_height;
    */
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

extern void mandelbrotSlices(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int slice,
    int maxIterations,
    int output[]);


//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs * const args) {

    double start_time = CycleTimer::currentSeconds();

    /* Approach 1: Splitting up the image into numThread horizontal chunks, 
    computed by mandelbrotSerial.

    // capture the case where we process the very last thread which may be slightly more 
    // due to non perfect division of height by numThreads
    int startRow;
    if (args->threadId < args->numThreads - 1) {
        startRow = (args->threadId) * (args->delta_height);
    }
    else {
        startRow = args->height - args->delta_height;
    }

    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
                     args->width, 
                     args->height,
                     startRow,  // startRow
                     args->delta_height, // delta amount of rows to process
                     args->maxIterations,
                     args->output);
    */
    /* Approach 2: We split up the image to be more balanced. Each thread computes every 
    numThreads-th row.

    ie for a given row r, if (r % numThreads == threadId), then this thread computes row 
    r.    
    */
    int slice = args->numThreads;
    mandelbrotSlices(args->x0, args->y0, args->x1, args->y1,
                     args->width, 
                     args->height,
                     args->threadId,  // startRow
                     slice, // slice
                     args->maxIterations,
                     args->output);

    double end_time = CycleTimer::currentSeconds();
    printf("Thread %d finished in %f ms\n", args->threadId,
           (end_time - start_time) * 1000);
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    /* from Approach 1 where we compute numThreads horizontal chunks
    // we will parallelize into numThreads horizontal blocks
    // such that the width is always the same (x0, x1 do not change)
    // but y0 and y1 change for each thread

    // since height may not be evenly divisible by numThreads,
    // we will give the last thread any remaining rows
    // but the other heights will be standardized under int division
    int block_height = height / numThreads; 
    */

    for (int i=0; i<numThreads; i++) {
        /* from Approach 1 where we compute numThreads horizontal chunks
        // height changes for each thread
        if (i < (numThreads - 1)){
            args[i].delta_height = block_height; 
        } else {
            args[i].delta_height = height - i * block_height;
        }
        */

        // shared parameters
        args[i].x0 = x0;  
        args[i].x1 = x1; 
        args[i].y0 = y0;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height; 
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
        args[i].threadId = i;
    }
    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    printf("Number of threads: %d\n", numThreads);
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}


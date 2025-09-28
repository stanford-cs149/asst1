#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    unsigned int delta_height;
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


//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs * const args) {

    // Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.
    int startRow = (args->threadId) * (args->delta_height); 
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
                     args->width, 
                     args->height,
                     0,  // startRow
                     args->delta_height, // totalRows
                     args->maxIterations,
                     args->output);
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
    float dy = (y1 - y0) / height; 

    // we will parallelize into numThreads horizontal blocks
    // such that the width is always the same (x0, x1 do not change)
    // but y0 and y1 change for each thread

    // since height may not be evenly divisible by numThreads,
    // we will give the last thread any remaining rows
    // but the other heights will be standardized under int division
    int block_height = height / numThreads; 

    for (int i=0; i<numThreads; i++) {
        // width is always the same
        args[i].x0 = x0;  
        args[i].x1 = x1; 
        args[i].width = width;

        // height changes for each thread
        if (i < (numThreads - 1)){
            args[i].y0 = y0 + i * block_height * dy; 
            args[i].y1 = y0 + (i+1) * block_height * dy;
            args[i].delta_height = block_height; 
        } else {
            args[i].y0 = y0 + i * block_height * dy; 
            args[i].y1 = y1; // make sure last thread gets all remaining rows
            args[i].delta_height = height - i * block_height;
        }

        // shared parameters
        args[i].height = height; // height of the full image
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output + i*block_height*width;;
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


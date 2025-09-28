#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
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

    // TODO FOR CS149 STUDENTS: Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.
    int startRow = (args->threadId) * (args->height); // xg: this will break if numThreads=7
    printf("Hello world from thread %d\n", args->threadId);
    printf("the start row is %d\n", startRow);
    printf("the height is %d\n", args->height);
    printf("the output pointer is %p\n", args->output);
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
                     args->width, args->height,
                     startRow, // startRow. xg: this will break if numThreads=7
                     args->height, // totalRows
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

    // xg additions 
    float dy = (y1 - y0) / height;  // xg: equivalent to dy for mandelbrotSerial
    int block_height = height / numThreads;

    for (int i=0; i<numThreads; i++) {
      
        // TODO FOR CS149 STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        // xg: right now, assume we are breaking into bands
        args[i].x0 = x0;  // xg: stays the same
        args[i].y0 = y0 + i * block_height * dy;  //xg: shift y0 up
        args[i].x1 = x1;  // xg: stays the same
        args[i].y1 = y1 + (i + 1) * block_height * dy;  // xg: shift y1 up
        args[i].width = width;  // xg: stays the same
        args[i].height = block_height;  // xg: need to rescale bc we are changing y0 and y1
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output + i * block_height * width; // xg: offset output pointer to start of thread's block
      
        args[i].threadId = i;
    }
    args[numThreads - 1].height = (y1 - args[numThreads - 1].y0) / dy; // xg: make sure last thread gets all remaining rows
    args[numThreads - 1].y1 = y1; // xg: make sure last thread gets all remaining rows

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}


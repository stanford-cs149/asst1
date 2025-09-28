#include <stdio.h>
#include <thread>
#include <algorithm>
#include <cmath>
#include <vector>

#include "CycleTimer.h"
#include <numeric>

bool DEBUG = false;



typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;

    std::vector<int> startRows;
    int numRows;

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

    double startTime = CycleTimer::currentSeconds();

    for (int startRow: args->startRows){
        if (DEBUG){
            printf("[thread-%d] start row: %d numRows: %d \n",
                 args->threadId, startRow, args->numRows);
        }

        mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, 
            /*startRow*/ startRow, 
            /*numRows*/  args->numRows ,
            args->maxIterations, args->output);
    }
    
    double endTime = CycleTimer::currentSeconds();

    if (DEBUG) {
        double minThread = 1e30;
        printf("[thread-%d][%fs] Done! \n",
             args->threadId,  std::min(minThread, endTime - startTime));
    }

}

void taskAssignmentEqualInterval(WorkerArgs *args, int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[]) 
{
    int rowGroupSize = height / numThreads;
    int startRow = 0;

    for (int i=0; i< numThreads; i++) {    
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;

        args[i].startRows.push_back(startRow);
        args[i].numRows = (i == numThreads - 1) ? height - i * rowGroupSize: rowGroupSize;
        startRow += args[i].numRows;
        args[i].threadId = i;
        }
}

void taskAssignmentNonOverlappingPartition(WorkerArgs *args, int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[]) 
{
    int rowGroupSize = 2;
    int numPartitions = height / rowGroupSize;

    for (int i=0; i<numThreads; i++) {    
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
        args[i].threadId = i;

        for (int j = i; j < numPartitions; j += numThreads){
            args[i].startRows.push_back(j * rowGroupSize);
        }
        args[i].numRows = rowGroupSize;
    }
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

    if (DEBUG){
        printf("\n==mandelbrotThread\n");
    } 

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    taskAssignmentNonOverlappingPartition(args, numThreads,
                        x0,  y0,  x1,  y1,
                        width,  height,
                        maxIterations, output);


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


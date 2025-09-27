# Changelog

Log to indicate changes so we know what is going on

## 9/27 at 4:45 pm

XG made changes to`mandelbrotThread.cpp`in lines 80-93 to set up the `WorkerArgs` for each thread and addded a call to `mandelbrotSerial` in `workerThreadStart` (lines 39-44). The code doesn't work for `numThreads=7` because of divisibility problems of `height=1600`. The output is buggy (the mandelbrot set looks squished into the top band of the image), but XG had to leave for now to go get a haircut.
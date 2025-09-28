# Writeup for Assignment 1

Xavier Gonzalez: xavier18@stanford.edu
JS Paul: jspaul@stanford.edu

## Program 1

## 1.2

The speed-up is not linear n the number of threads used because of a thread-imbalance (just looking at the mandelbrot image, you can see that more work is being done by the threads in the middle).

## 1.3

The measurements show that because of worker imbalance, we are not getting linear speed ups, but they are instead plateauing.

## 1.4

We chunk the image into smaller pieces, assign each thread to the smaller piece, in this way avoiding worker imbalance. 7.19 thread-up with 8 threads.

## 1.5

Only 7.08 speed up with 16 threads. This is because the specs only support 8 hardware threads, so the OS is context switching between threads.

## Program 2

## Program 3

## Program 4

## Program 5

## Program 6
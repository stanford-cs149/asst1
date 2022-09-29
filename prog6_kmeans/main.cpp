#include <algorithm>
#include <iostream>
#include <math.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "CycleTimer.h"

#define SEED 7
#define SAMPLE_RATE 1e-2

using namespace std;

// Main compute functions
extern void kMeansThread(double *data, double *clusterCentroids,
                      int *clusterAssignments, int M, int N, int K,
                      double epsilon);
extern double dist(double *x, double *y, int nDim);

// Utilities
extern void logToFile(string filename, double sampleRate, double *data,
                      int *clusterAssignments, double *clusterCentroids, int M,
                      int N, int K);
extern void writeData(string filename, double *data, double *clusterCentroids,
                      int *clusterAssignments, int *M_p, int *N_p, int *K_p,
                      double *epsilon_p);
extern void readData(string filename, double **data, double **clusterCentroids,
                     int **clusterAssignments, int *M_p, int *N_p, int *K_p,
                     double *epsilon_p);

// Functions for generating data
double randDouble() {
  return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

void initData(double *data, int M, int N) {
  int K = 10;
  double *centers = new double[K * N];

  // Gaussian noise
  double mean = 0.0;
  double stddev = 0.5;
  std::default_random_engine generator;
  std::normal_distribution<double> normal_dist(mean, stddev);

  // Randomly create points to center data around
  for (int k = 0; k < K; k++) {
    for (int n = 0; n < N; n++) {
      centers[k * N + n] = randDouble();
    }
  }

  // Even clustering
  for (int m = 0; m < M; m++) {
    int startingPoint = rand() % K; // Which center to start from
    for (int n = 0; n < N; n++) {
      double noise = normal_dist(generator);
      data[m * N + n] = centers[startingPoint * N + n] + noise;
    }
  }

  free(centers);
}

void initCentroids(double *clusterCentroids, int K, int N) {
  // Initialize centroids (close together - makes it a bit more interesting)
  for (int n = 0; n < N; n++) {
    clusterCentroids[n] = randDouble();
  }
  for (int k = 1; k < K; k++) {
    for (int n = 0; n < N; n++) {
      clusterCentroids[k * N + n] =
          clusterCentroids[n] + (randDouble() - 0.5) * 0.1;
    }
  }
}

int main() {
  srand(SEED);

  int M, N, K;
  double epsilon;

  double *data;
  double *clusterCentroids;
  int *clusterAssignments;

  // NOTE: we will grade your submission using the data in data.dat
  // which is read by this function
  readData("./data.dat", &data, &clusterCentroids, &clusterAssignments, &M, &N,
           &K, &epsilon);

  // NOTE: if you want to generate your own data (for fun), you can use the
  // below code
  /*
  M = 1e6;
  N = 100;
  K = 3;
  epsilon = 0.1;

  data = new double[M * N];
  clusterCentroids = new double[K * N];
  clusterAssignments = new int[M];

  // Initialize data
  initData(data, M, N);
  initCentroids(clusterCentroids, K, N);

  // Initialize cluster assignments
  for (int m = 0; m < M; m++) {
    double minDist = 1e30;
    int bestAssignment = -1;
    for (int k = 0; k < K; k++) {
      double d = dist(&data[m * N], &clusterCentroids[k * N], N);
      if (d < minDist) {
        minDist = d;
        bestAssignment = k;
      }
    }
    clusterAssignments[m] = bestAssignment;
  }

  // Uncomment to generate data file
  // writeData("./data.dat", data, clusterCentroids, clusterAssignments, &M, &N,
  //           &K, &epsilon);
  */

  printf("Running K-means with: M=%d, N=%d, K=%d, epsilon=%f\n", M, N,
         K, epsilon);

  // Log the starting state of the algorithm
  logToFile("./start.log", SAMPLE_RATE, data, clusterAssignments,
            clusterCentroids, M, N, K);

  double startTime = CycleTimer::currentSeconds();
  kMeansThread(data, clusterCentroids, clusterAssignments, M, N, K, epsilon);
  double endTime = CycleTimer::currentSeconds();
  printf("[Total Time]: %.3f ms\n", (endTime - startTime) * 1000);

  // Log the end state of the algorithm
  logToFile("./end.log", SAMPLE_RATE, data, clusterAssignments,
            clusterCentroids, M, N, K);

  free(data);
  free(clusterCentroids);
  free(clusterAssignments);
  return 0;
}
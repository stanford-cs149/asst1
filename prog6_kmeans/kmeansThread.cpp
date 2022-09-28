#include <algorithm>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "CycleTimer.h"

using namespace std;

typedef struct {
  // Control work assignments
  int start, end;

  // Shared by all functions
  double *data;
  double *clusterCentroids;
  int *clusterAssignments;
  double *currCost;
  int M, N, K, threadIdx;
} WorkerArgs;

/* DO NOT MODIFY THIS FUNCTION!!! */
static bool stoppingConditionMet(double *prevCost, double *currCost,
                                 double epsilon, int K) {
  for (int k = 0; k < K; k++) {
    if (abs(prevCost[k] - currCost[k]) > epsilon)
      return false;
  }
  return true;
}

double dist(double *x, double *y, int nDim) {
  double accum = 0.0;
  for (int i = 0; i < nDim; i++) {
    accum += pow((x[i] - y[i]), 2);
  }
  return sqrt(accum);
}

void computeAssignments(WorkerArgs *const args) {
  for (int m = args->start; m < args->end; m++) {
    double minDist = 1e30;
    int bestAssignment = -1;

    // Assign datapoints to closest centroids
    for (int k = 0; k < args->K; k++) {
      double d = dist(&args->data[m * args->N],
                      &args->clusterCentroids[k * args->N], args->N);
      if (d < minDist) {
        minDist = d;
        bestAssignment = k;
      }
    }
    args->clusterAssignments[m] = bestAssignment;
  }
}

void computeCentroids(WorkerArgs *const args) {
  for (int k = args->start; k < args->end; k++) {
    int count = 0;

    // Zero things out
    for (int n = 0; n < args->N; n++) {
      args->clusterCentroids[k * args->N + n] = 0.0;
    }

    // Sum up contributions from assigned examples
    for (int m = 0; m < args->M; m++) {
      if (args->clusterAssignments[m] == k) {
        for (int n = 0; n < args->N; n++) {
          args->clusterCentroids[k * args->N + n] +=
              args->data[m * args->N + n];
        }
        count++;
      }
    }

    // Compute mean
    count = max(count, 1); // prevent divide by 0
    for (int n = 0; n < args->N; n++) {
      args->clusterCentroids[k * args->N + n] /= count;
    }
  }
}

void computeCost(WorkerArgs *const args) {
  for (int k = args->start; k < args->end; k++) {
    double accum = 0.0;

    // Sum cost for all data points assigned to centroid
    for (int m = 0; m < args->M; m++) {
      if (args->clusterAssignments[m] == k) {
        accum += dist(&args->data[m * args->N],
                      &args->clusterCentroids[k * args->N], args->N);
      }
    }
    args->currCost[k] = accum;
  }
}

/**
 * Computes the K-Means algorithm, using std::thread to parallelize the work.
 *
 * @param data Pointer to an array of length M*N representing the M different N 
 *     dimensional data points clustered. E.g. data[:N] will correspond to the 
 *     first data point, data[N:2N] will correspond to the second, etc.
 * @param clusterCentroids Pointer to an array of length K*N representing the K 
 *     different N dimensional cluster centroids. E.g. clusterCentroids[:N] will
 *     correspond to the first cluster centroid, etc.
 * @param clusterAssignments Pointer to an array of length M representing the
 *     cluster assignments of each data point, where clusterAssignments[i] = j
 *     indicates that data point i is closest to cluster centroid j.
 * @param M The number of data points to cluster.
 * @param N The dimensionality of the data points.
 * @param K The number of cluster centroids.
 * @param epsilon The algorithm is said to have converged when
 *     |currCost[i] - prevCost[i]| < epsilon for all i where i = 0, 1, ..., K-1
 */
void kMeansThread(double *data, double *clusterCentroids, int *clusterAssignments,
               int M, int N, int K, double epsilon) {

  // Setup for threads
  static constexpr int NUM_THREADS = 32;
  thread workers[NUM_THREADS];
  WorkerArgs args[NUM_THREADS];

  // Used to track convergence
  double *prevCost = new double[K];
  double *currCost = new double[K];

  // Initialize arrays to track cost
  for (int k = 0; k < K; k++) {
    prevCost[k] = 1e30;
    currCost[k] = 0.0;
  }

  // The WorkerArgs array is used to passing inputs to and returning outputs 
  // from the workers for each parallel function invocation. The below args are 
  // the same for each function invocation.   
  for (int i = 0; i < NUM_THREADS; i++) {
    args[i].data = data;
    args[i].clusterCentroids = clusterCentroids;
    args[i].clusterAssignments = clusterAssignments;
    args[i].currCost = currCost;
    args[i].M = M;
    args[i].N = N;
    args[i].K = K;
    args[i].threadIdx = i;
  }

  int iter = 0;
  while (!stoppingConditionMet(prevCost, currCost, epsilon, K)) {
    // Update cost arrays (for checking convergence criteria)
    for (int k = 0; k < K; k++) {
      prevCost[k] = currCost[k];
    }

    // Staticically assigning work to threads
    int chunkSizeM =
        (M % NUM_THREADS == 0) ? (M / NUM_THREADS) : (M / NUM_THREADS) + 1;
    for (int i = 0; i < NUM_THREADS; i++) {
      args[i].start = min(chunkSizeM * i, M);
      args[i].end = min(args[i].start + chunkSizeM, M);
    }

    // Compute cluster assignments in parallel
    for (int i = 0; i < NUM_THREADS; i++)
      workers[i] = std::thread(computeAssignments, &args[i]);
    for (int i = 0; i < NUM_THREADS; i++)
      workers[i].join();

    // Staticically assigning work to threads
    int chunkSizeK =
        (K % NUM_THREADS == 0) ? (K / NUM_THREADS) : (K / NUM_THREADS) + 1;
    for (int i = 0; i < NUM_THREADS; i++) {
      args[i].start = min(chunkSizeK * i, K);
      args[i].end = min(args[i].start + chunkSizeK, K);
    }

    // Compute centroids in parallel
    for (int i = 0; i < NUM_THREADS; i++)
      workers[i] = std::thread(computeCentroids, &args[i]);
    for (int i = 0; i < NUM_THREADS; i++)
      workers[i].join();

    // Compute cost in parallel
    for (int i = 0; i < NUM_THREADS; i++)
      workers[i] = std::thread(computeCost, &args[i]);
    for (int i = 0; i < NUM_THREADS; i++)
      workers[i].join();

    iter++;
  }

  free(currCost);
  free(prevCost);
}

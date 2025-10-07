#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h> // For AVX2 intrinsics


void sqrtSerial(int N,
                float initialGuess,
                float values[],
                float output[])
{

    static const float kThreshold = 0.00001f;

    for (int i=0; i<N; i++) {

        float x = values[i];
        float guess = initialGuess;

        float error = fabs(guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            error = fabs(guess * guess * x - 1.f);
        }

        output[i] = x * guess;
    }
}

void sqrtAvx2(
    int N,
    float initialGuess,
    float values[],
    float output[]
) {
    static const int kMaxIters = 25;

    int vectorWidth = 8; // AVX2 processes 8 floats at a time
    int i = 0;
    for (; i < N; i += vectorWidth) {
        __m256 value = _mm256_loadu_ps(&values[i]);
        __m256 guess = _mm256_set1_ps(initialGuess);
        
        for (int j = 0; j < kMaxIters; j++) {
            __m256 guessTimesThree = _mm256_mul_ps(_mm256_set1_ps(3.f), guess);
            __m256 guessTriple = _mm256_mul_ps(guess, _mm256_mul_ps(guess, guess));
            __m256 temp = _mm256_sub_ps(guessTimesThree, _mm256_mul_ps(value, guessTriple));
            guess = _mm256_mul_ps(temp, _mm256_set1_ps(0.5f));
        }

        _mm256_storeu_ps(&output[i], _mm256_mul_ps(value, guess));
    }
}

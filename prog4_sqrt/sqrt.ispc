
static const float kThreshold = 0.00001f; 

export void sqrt_ispc(uniform int N,
                      uniform float initialGuess,
                      uniform float values[],
                      uniform float output[])
{
    foreach (i = 0 ... N) {

        float x = values[i];
        float guess = initialGuess;

        float pred = abs(guess * guess * x - 1.f);

        while (pred > kThreshold) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            pred = abs(guess * guess * x - 1.f);
        }

        output[i] = x * guess;
        
    }
}

task void sqrt_ispc_task(uniform int N,
                         uniform int span,
                         uniform float initialGuess,
                         uniform float values[],
                         uniform float output[])
{

    uniform int indexStart = taskIndex * span;
    uniform int indexEnd = min(N, indexStart + span);
    
    foreach (i = indexStart ... indexEnd) {

        float x = values[i];
        float guess = initialGuess;

        float pred = abs(guess * guess * x - 1.f);

        while (pred > kThreshold) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            pred = abs(guess * guess * x - 1.f);
        }

        output[i] = x * guess;
    
    }
}

export void sqrt_ispc_withtasks(uniform int N,
                                uniform float initialGuess,
                                uniform float values[],
                                uniform float output[])
{

    uniform int span = N / 64;  // 64 tasks

    launch[N/span] sqrt_ispc_task(N, span, initialGuess, values, output);
}

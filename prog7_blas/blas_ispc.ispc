task void axpyIspcTask(
    uniform int n,
    uniform float a,
    uniform float x[],
    uniform float y[],
    uniform float result[]
) {
    uniform int span = n / taskCount;
    uniform int indexStart = taskIndex * span;
    uniform int indexStop = min(indexStart + span, n);
    foreach (i = indexStart ... indexStop) {
        result[i] = a * x[i] + y[i];
    }
}

export void axpyIspc(
    uniform int tasks,
    uniform int n,
    uniform float a,
    uniform float x[],
    uniform float y[],
    uniform float result[]
) {
    launch[tasks] axpyIspcTask(n, a, x, y, result);
}

task void AxIspc1Task(
    uniform int m,
    uniform int n,
    uniform float A[],
    uniform float x[],
    uniform float result[]
) {
    uniform int span = m / taskCount;
    uniform int rowStart = taskIndex * span;
    uniform int rowStop = min(rowStart + span, m);
    for (uniform int i = rowStart; i < rowStop; i++) {
        foreach (j = 0 ... n) {
            int k = i*n + j;
            result[k] = A[k] * x[j];
        }
    }
}

export void AxIspc1(
    uniform int tasks,
    uniform int m,
    uniform int n,
    uniform float A[],
    uniform float x[],
    uniform float result[]
) {
    launch[tasks] AxIspc1Task(m, n, A, x, result);
}

task void ABIspcTask(
    uniform int m,
    uniform int n,
    uniform int l,
    uniform float A[],
    uniform float B[],
    uniform float result[]
){
    uniform int span = m / taskCount;
    uniform int rowStart = taskIndex * span;
    uniform int rowStop = min(rowStart + span, m);
    for (uniform int i = rowStart; i < rowStop; i++) {
        for (uniform int j = 0; j < n; j++) {
            uniform float a = A[i*n + j];
            foreach (k = 0 ... l){
                result[i*l + k] += a * B[j*l + k];
            }
        }
    }
}

export void ABIspc(
    uniform int tasks,
    uniform int m,
    uniform int n,
    uniform int l,
    uniform float A[],
    uniform float B[],
    uniform float result[]
) {
    /**
     * Note: how to parallelize matrix multiplication
     * - iterations of the third loop should be executed on lanes, because they
     *   access adjacent addresses of result
     * - iterations of the second loop must be executed in the same task, because
     *   latter iterations depend on previous interations (+=)
     * - iterations of the first loop can be executed in different tasks, because
     *   they access different rows of result
     */
    launch[tasks] ABIspcTask(m, n, l, A, B, result);
}

task void aABpbCIspcTask(
    uniform int m,
    uniform int n,
    uniform int l,
    uniform float a,
    uniform float result[],
    uniform float b,
    uniform float C[]
) {
    uniform int span = m / taskCount;
    uniform int rowStart = taskIndex * span;
    uniform int rowStop = min(rowStart + span, m);
    for (uniform int i = rowStart; i < rowStop; i++) {
        foreach (k = 0 ... l){
            int loc = i*l + k;
            result[loc] = a * result[loc] + b * C[loc];
        }
    }
}

export void aABpbCIspc(
    uniform int tasks,
    uniform int m,
    uniform int n,
    uniform int l,
    uniform float a,
    uniform float A[],
    uniform float B[],
    uniform float b,
    uniform float C[],
    uniform float result[]
) {
    launch[tasks] ABIspcTask(m, n, l, A, B, result);
    sync;
    launch[tasks] aABpbCIspcTask(m, n, l, a, result, b, C);
}

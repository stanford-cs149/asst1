
void saxpySerial(int N,
                       float scale,
                       float X[],
                       float Y[],
                       float result[])
{

    for (int i=0; i<N; i++) {
        result[i] = scale * X[i] + Y[i];
    }
}


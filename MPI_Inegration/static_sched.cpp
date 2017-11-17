#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

    float f1(float x, int intensity);
    float f2(float x, int intensity);
    float f3(float x, int intensity);
    float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float (*funcPtr) (float,int);

int main (int argc, char* argv[])
{
    if (argc < 6) 
    {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
        return -1;
    }

    int a = atoi(argv[2]);
    int n = atoi(argv[4]);
    int intensity = atoi(argv[5]);
    int rank, size;
    int chunkSize = n / size;
    int tag = MPI_ANY_TAG;
    double globalResult = 0.0;
    float multiplier = (atoi(argv[3]) - a) / (float)n;
    MPI_Comm comm;

    switch(atoi(argv[1]))
    {
        case 1:
            funcPtr = &f1;
            break;
        case 2:
            funcPtr = &f2;
            break;
        case 3:
            funcPtr = &f3;
            break;
        case 4:
            funcPtr = &f4;
            break;
        default:
            std::cerr<<"Invalid function number provided\n";
            exit(-1);
    }
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int arrStart = rank * chunkSize;
    int arrEnd = (rank + 1) * chunkSize;
    if (rank == size - 1)
        arrEnd = n;

    double result = 0.0;
    for (int x = arrStart; x < arrEnd; ++x)
    {
        result += (double)funcPtr(a + (x + 0.5) * multiplier, intensity) * multiplier;
    }
    if (rank != 0)
    {
        MPI_Send(&result, 1, MPI_DOUBLE_PRECISION, 0, 100+rank, MPI_COMM_WORLD);
    }
    else
    {
        globalResult = result;
        for (int x = 1; x < size; ++x)
        {
            MPI_Status status;
            MPI_Recv(&result, 1, MPI_DOUBLE_PRECISION, x, 100+x, MPI_COMM_WORLD, &status);
            globalResult += result;
        }
    }

    MPI_Finalize();
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    if(rank == 0)
    {
        std::cout<<globalResult<<std::endl;
        std::cerr<<elapsed_seconds.count()<<std::endl;
    }

    return 0;
}

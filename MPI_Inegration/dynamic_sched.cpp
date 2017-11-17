#include <iostream>
#include <ratio>
#include <chrono>
#include <stdlib.h>
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
        std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
        return -1;
    }
    int a = atoi(argv[2]);
    int n = atoi(argv[4]);
    int intensity = atoi(argv[5]);
    int chunkSize = n / size;
    int tag = MPI_ANY_TAG;
    int rank, size;
    int master = 0;
    float multiplier = (atoi(argv[3]) - a) / (float)n;
    double integralValue = 0.0;
    double globalResult = 0.0;
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
    if(rank == 0)
    {
        bool completed = false;
        for ( int i = 0 ; i < chunkSize && (!completed); ++i)
        {
            int arrStart = i * chunkSize;
            for(int x = 1; x < size; ++x)
            {
                int index = arrStart + x;
                if ( index >= n)
                {
                    completed = true;
                    break;
                }
                std::cout<<"Master sending "<<index<<" to node : "<<x<<std::endl;
                MPI_Send(&index, 1, MPI_INT, x, 100+x, MPI_COMM_WORLD);
            }
            for(int y = 1; y < size; ++y)
            {
                double res;
                MPI_Status status;
                int index = arrStart + y;
                if ( index >= n)
                {
                    completed = true;
                    break;
                }
                MPI_Recv(&res, 1, MPI_DOUBLE_PRECISION, MPI_ANY_SOURCE, (n + arrStart + y), MPI_COMM_WORLD, &status);
                std::cout<<"Master Receiving result from "<<y<<" value is : "<<res<<std::endl;
                integralValue += res;
            }
        }
        std::cout<<"Integral Value is : "<<integralValue<<std::endl;
    }
    else
    {
        MPI_Status status;
        int indexCalc;

        // int MPI_Recv(void* buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
        // MPI_Recv(&indexCalc, 1, MPI_INT, master, 100+rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&indexCalc, 1, MPI_INT, 0, 100+rank, MPI_COMM_WORLD, &status);
        std::cout<<"Node "<<rank<<" Receiving index "<<indexCalc<<std::endl;

        double result = (double)funcPtr(a + (indexCalc+ 0.5) * multiplier, intensity) * multiplier;

        std::cout<<"Sending result : "<<result<<" to Master"<<std::endl;
        // int MPI_Send(const void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
        //MPI_Send(&result, 1, MPI_DOUBLE_PRECISION, master, 100+rank, MPI_COMM_WORLD);
        MPI_Send(&result, 1, MPI_DOUBLE_PRECISION, 0, n+indexCalc, MPI_COMM_WORLD);
    }
    MPI_Finalize();

  return 0;
}

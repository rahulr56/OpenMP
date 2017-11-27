#include <iostream>
#include <ratio>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


#define ARR_START_SEND 100
#define ARR_START_RECV 100
#define ARR_END_SEND 101
#define ARR_END_RECV 101
#define RESULT_TAG 1002
#define CHUNK_SIZE 2

#define MASTER_MPI 0

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
    float multiplier = (atoi(argv[3]) - a) / (float)n;
    double integralValue = 0.0;
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

    double globalResult = 0.0;
    int rank, size;
    int master = 0;
    int chunkSize = n / (size ) ;
    int arrIndex = 0;
    double receivedResult = 0.0;
    int startIndex, endIndex;
    MPI_Status status;

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == 0)
    {
        for(int i=1; i < size; ++i)
        {
            MPI_Send(&arrIndex,         // index being sent
                    1,                  // 1byte
                    MPI_INT,            // data type of var being sent
                    i,                  // Destination
                    ARR_START_SEND,     // tag
                    MPI_COMM_WORLD);    // MPI Communicator

            arrIndex += CHUNK_SIZE;
            if(arrIndex >= n)
            {
                arrIndex = n;
            }

            MPI_Send(&arrIndex,         // index being sent
                    1,                  // 1byte
                    MPI_INT,            // data type of var being sent
                    i,                  // Destination
                    ARR_END_SEND,       // tag
                    MPI_COMM_WORLD);    // MPI Communicator
        }
        sleep(1);
        for(int i=0; i < size;)
        {
            MPI_Recv(&receivedResult,       // Value in whuch the message is received
                    1,                      // length of data being received
                    MPI_DOUBLE_PRECISION,   // data type of var
                    MPI_ANY_SOURCE,         // Any source
                    RESULT_TAG,             // tag - 1002
                    MPI_COMM_WORLD,         // MPI Communicator
                    &status);               // stats of received message

            integralValue += receivedResult;
            if(arrIndex >= n)
            {
                MPI_Send(&n,                // index being sent
                        1,                  // 1byte
                        MPI_INT,            // data type of var being sent
                        status.MPI_SOURCE,  // Destination
                        ARR_START_SEND,     // tag
                        MPI_COMM_WORLD);    // MPI Communicator
                ++i;
            }
            else
            {
                MPI_Send(&arrIndex,         // index being sent
                        1,                  // 1byte
                        MPI_INT,            // data type of var being sent
                        status.MPI_SOURCE,  // Destination
                        ARR_START_SEND,     // tag
                        MPI_COMM_WORLD);    // MPI Communicator
                arrIndex += CHUNK_SIZE;
                if(arrIndex >= n)
                {
                    arrIndex = n;
                }
                MPI_Send(&arrIndex,         // index being sent
                        1,                  // 1byte
                        MPI_INT,            // data type of var being sent
                        status.MPI_SOURCE,  // Destination
                        ARR_END_SEND,       // tag
                        MPI_COMM_WORLD);    // MPI Communicator
            }
        }
        std::cout<<integralValue<<std::endl;
    }
    else
    {
        while(true)
        {
            // Receive array start index
            MPI_Recv(&startIndex,           // Value in whuch the message is received
                    1,                      // length of data being received
                    MPI_INT,                // data type of var
                    MASTER_MPI,             // receive from Master (0)
                    ARR_START_RECV,         // tag
                    MPI_COMM_WORLD,         // MPI Communicator
                    &status);               // stats of received message
            if(startIndex == n)
            {
                double dummyResult = 0.0;
                MPI_Send(&dummyResult,          // computed result to send to master
                        1,                      // length of data being sent
                        MPI_DOUBLE_PRECISION,   // data type of var
                        MASTER_MPI,             // Destination - Master (0)
                        RESULT_TAG,             // tag - 1002
                        MPI_COMM_WORLD);        // MPI Communicator
                break;
            }

            // Receive array end index
            MPI_Recv(&endIndex,             // Value in whuch the message is received
                    1,                      // length of data being received
                    MPI_INT,                // data type of var
                    MASTER_MPI,             // receive from Master (0)
                    ARR_END_RECV,           // tag
                    MPI_COMM_WORLD,         // MPI Communicator
                    &status);               // stats of received message

            double result = 0.0;
            for (int x = startIndex; x < endIndex; ++x)
            {
                result += (double)funcPtr(a + (x + 0.5) * multiplier, intensity) * multiplier;
            }

            // Send result to master
            MPI_Send(&result,               // computed result to send to master
                    1,                      // length of data being sent
                    MPI_DOUBLE_PRECISION,   // data type of var
                    MASTER_MPI,             // Destination - Master (0)
                    RESULT_TAG,             // tag - 1002
                    MPI_COMM_WORLD);        // MPI Communicator
        }
    }
    MPI_Finalize();
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    if(rank == 0)
        std::cerr<<elapsed_seconds.count()<<std::endl;
    return 0;
}

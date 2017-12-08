#include <iostream>
#include <string>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define MASTER_SEND_TAG 100
#define MASTER_RECV_TAG 102
#define MASTER_OFFSET_TAG 1022
#define MASTER_NODE 0

void generateMatrix(int **arr, int n)
{
    int k = 0;
    for ( register int i=0; i< n ; ++i )
    {
        for ( register int j=0; j< n ; ++j)
        {
            arr[i][j]=k++;
        }
    }
}

int computeHeatEq(int **h, int row, int col, int n)
{
    int value =0;
    if(row == 0)
    {
        if(col==0)
        {
            value = (h[0][1]+h[1][0]+h[1][1]+5*h[0][0])/9;
        }
        else if(col== (n-1))
        {
            value = (h[0][col-1]+h[1][col-1]+h[1][col]+5*h[0][col])/9;
        }
        else
        {
            value = (h[0][col-1]+ h[0][col+1] + h[1][col-1] + h[1][col] + h[1][col+1] + 4*h[0][col]) / 9;
        }
    }
    else if ( row == n-1 )
    {
        if(col == 0)
        {
            value = (h[row][1]+h[row-1][0]+h[row-1][1]+5*h[row][0])/9;
        }
        else if(col== (n-1))
        {
            value = (h[row][col-1]+h[row-1][col-1]+h[row-1][col]+5*h[row][col])/9;
        }
        else
        {
            value = (h[row][col-1]+ h[row][col+1] + h[row-1][col-1] + h[row-1][col] + h[row-1][col+1] + 4*h[row][col]) / 9;
        }
    }
    else if ( col == 0)
    {
        value = (h[row-1][0] + h [row-1][col+1] + h[row][col+1] + h[row+1][col] + h[row+1][col+1] + h[row][col]*4)/9;
    }
    else if ( col == n-1)
    {
        value = (h[row-1][col] + h[row-1][col-1] + h[row][col-1]+h[row+1][col-1] + h[row+1][col]+h[row][col]*4)/9;
    }
    else
    {
        value = (h[row][col]+h[row][col-1]+h[row][col+1]+h[row-1][col-1]+h[row-1][col]+h[row-1][col+1]+h[row+1][col-1]+h[row+1][col]+h[row+1][col+1])/9;
    }
    return value;
}

int main (int argc, char** argv) {

    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
        return -1;
    }
    int n = atoi(argv[1]);
    MPI_Status status;
    int **array = new int*[n];
    int **newArray = new int*[n];
    for(int i=0; i<n; ++i)
        array[i] =  new int[n];
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rows = n / (size-1);
    for(int i=0; i<n; ++i)
        newArray[i] =  new int[n];

    std::chrono::time_point<std::chrono::system_clock> timerStart = std::chrono::system_clock::now(); 
    for( int r = 0; r < 20; ++r)
    {
        if (rank == 0)
        {
            int offset = 0;
            if(r==0)
            {
                generateMatrix(array, n);
            }
            for (int dest = 1; dest < size; ++dest)
            {
                MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
                int arrsize = rows*n;
                if(dest == 1)
                {
                    MPI_Send( &array[offset][0], (rows*n + n),
                            MPI_INT, dest,
                            MASTER_SEND_TAG, MPI_COMM_WORLD);
                    offset += rows;
                    continue;
                }

                if( dest == (size-1))
                {
                    arrsize = (n - offset);
                    MPI_Send(&arrsize, 1, MPI_INT, dest, 2, MPI_COMM_WORLD);
                    MPI_Send( &array[offset-1][0], (arrsize+1)*n,
                            MPI_INT, dest,
                            MASTER_SEND_TAG, MPI_COMM_WORLD);
                }
                else 
                {
                    MPI_Send( &array[offset-1][0], ((rows+2)*n),
                            MPI_INT, dest,
                            MASTER_SEND_TAG, MPI_COMM_WORLD);
                }
                offset += rows;
            }
            // Start receiving .... 
            int receivedOffset;
            for (int node = 1; node < size-1; ++node)
            {
                MPI_Recv(&receivedOffset, 1, MPI_INT, MPI_ANY_SOURCE, MASTER_OFFSET_TAG, MPI_COMM_WORLD, &status);
                MPI_Recv(&array[receivedOffset][0] , rows*n, MPI_INT, status.MPI_SOURCE, (MASTER_RECV_TAG), MPI_COMM_WORLD, &status);
            }
            int sizeToRecv = n - receivedOffset;
            MPI_Recv(&receivedOffset, 1, MPI_INT, (size-1), (MASTER_OFFSET_TAG+size), MPI_COMM_WORLD, &status);
            MPI_Recv(&array[receivedOffset][0] , (sizeToRecv)*n, MPI_INT, (size-1), (MASTER_RECV_TAG+size), MPI_COMM_WORLD, &status);
        }
        else if(rank == (size - 1))
        {
            int offset, arrsize1;
            MPI_Recv(&offset, 1, MPI_INT, MASTER_NODE, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&arrsize1, 1, MPI_INT, MASTER_NODE, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&array[offset-1][0], (arrsize1+1)*n , MPI_INT, MASTER_NODE, MASTER_SEND_TAG, MPI_COMM_WORLD, &status);
            for(int row =offset-1; row < arrsize1+offset;++row)
            {
                for(int col = 0; col < n;++col)
                {
                    array[row][col] = computeHeatEq(array, row, col, n);
                }
            }
            MPI_Send(&offset, 1, MPI_INT, MASTER_NODE, MASTER_OFFSET_TAG+size, MPI_COMM_WORLD);
            MPI_Send( &array[offset][0], arrsize1*n,
                    MPI_INT, MASTER_NODE,
                    (MASTER_RECV_TAG+size), MPI_COMM_WORLD);
        }
        else if(rank == 1)
        {
            int offset;
            MPI_Recv(&offset, 1, MPI_INT, MASTER_NODE, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&array[offset][0], (rows + 1)*n, MPI_INT, MASTER_NODE, MASTER_SEND_TAG, MPI_COMM_WORLD, &status);
            for(int row = 0; row < rows;++row)
            {
                for(int col = 0; col < n;++col)
                {
                    array[row][col] = computeHeatEq(array, row, col, n);
                }
            }
            MPI_Send(&offset, 1, MPI_INT, MASTER_NODE, MASTER_OFFSET_TAG, MPI_COMM_WORLD);
            MPI_Send( &array[0][0], (rows*n),
                    MPI_INT, MASTER_NODE,
                    MASTER_RECV_TAG, MPI_COMM_WORLD);
        }
        else
        {
            int offset;
            MPI_Recv(&offset, 1, MPI_INT, MASTER_NODE, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&array[offset-1][0], (rows + 2)*n, MPI_INT, MASTER_NODE, MASTER_SEND_TAG, MPI_COMM_WORLD, &status);
            for(int row =offset-1; row < rows+offset+1;++row)
            {
                for(int col = 0; col < n;++col)
                {
                    array[row][col] = computeHeatEq(array, row, col, n);
                }
            }
            MPI_Send(&offset, 1, MPI_INT, MASTER_NODE, MASTER_OFFSET_TAG, MPI_COMM_WORLD);
            MPI_Send(&array[offset][0], (rows*n),
                    MPI_INT, MASTER_NODE,
                    MASTER_RECV_TAG, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
    std::chrono::time_point<std::chrono::system_clock> timerEnd = std::chrono::system_clock::now(); 


    if(rank == 0)
    {
        std::chrono::duration<double> elapsed_seconds = timerEnd - timerStart;
        std::cerr<<std::fixed<<elapsed_seconds.count() << std::endl;
    }

    for (int x=0;x<n;++x)
        delete[] array[x];
    delete[] array;
}

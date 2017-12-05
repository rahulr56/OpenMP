#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define MASTER_SEND_TAG 100
#define MASTER_NODE 0

void generateMatrix(int **arr, int n)
{
    int k = 0;
    for ( register int i=0; i< n ; ++i )
    {
        for ( register int j=0; j< n ; ++j)
        {
            arr[i][j]=k++;
//            std::cout<<arr[i][j]<<"\t";
        }
//        std::cout<<std::endl;
    }
}

int computeHeatEq(int **h, int row, int col, int n)
{
    int value =0;
//    std::cout<<"row : "<<row<<"\t col :"<<col<<"\t n :"<<n<<std::endl;
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
    for(int i=0; i<n; ++i)
        array[i] =  new int[n];
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rows = n / (size-1);

    for( int r = 0; r < 1; ++r)
    {
        if (rank == 0)
        {
            int offset = 0;
            std::cout<<"Generating matrix\n";
            if(r==0)
            {
                generateMatrix(array, n);
            }
            else
            {
                // assign assembled matrix to array
            }
            std::cout<<"matrix Generated\n rows :"<<rows<<std::endl;
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
                    continue;
                }
                MPI_Send( &array[offset-1][0], ((rows+2)*n),
                        MPI_INT, dest,
                        MASTER_SEND_TAG, MPI_COMM_WORLD);
                offset += rows;
            }
        }
        else if(rank == (size - 1))
        {
            int offset;
            int arrsize1;
            MPI_Recv(&offset, 1, MPI_INT, MASTER_NODE, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&arrsize1, 1, MPI_INT, MASTER_NODE, 2, MPI_COMM_WORLD, &status);
//            std::cout<<"====size : "<<(arrsize1+1)*n<<std::endl;
            MPI_Recv(&array[offset-1][0], (arrsize1+1)*n , MPI_INT, MASTER_NODE, MASTER_SEND_TAG, MPI_COMM_WORLD, &status);
            for(int row =offset-1; row < arrsize1+offset;++row)
            {
                for(int col = 0; col < n;++col)
                {
                    array[row][col] = computeHeatEq(array, row, col, n);
                }
            }
        }
        else if(rank == 1)
        {
            int offset;
            MPI_Recv(&offset, 1, MPI_INT, MASTER_NODE, 1, MPI_COMM_WORLD, &status);
            std::cout<<"RANK : "<<rank<<"\tOFFSET : "<<offset<<"\t+++++ SIze : "<<(rows+2)*n<<std::endl;
            MPI_Recv(&array[offset][0], (rows + 1)*n, MPI_INT, MASTER_NODE, MASTER_SEND_TAG, MPI_COMM_WORLD, &status);
            std::cout<<"Rank : "<<rank<<std::endl;
            for(int row = 0; row < rows;++row)
            {
                for(int col = 0; col < n;++col)
                {
                    array[row][col] = computeHeatEq(array, row, col, n);
                }
            }
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
        }
    }

    MPI_Finalize();

    for (int x=0;x<n;++x)
        delete[] array[x];
    delete[] array;
}

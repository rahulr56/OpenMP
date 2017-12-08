#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define COLS  12
#define ROWS  8
void generateMatrix(int *arr, int n)
{
    register int counter = 0; 
    for(int i = 0; i < n*n; ++i)
    {
        arr[i] = counter++;
    }
}
int main(int argc, char **argv) {

    int n = atoi(argv[1]);
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char i;

    int  *array =  new int[n*n];
    int rootP = sqrt(size);
    const int NPROWS= rootP;  /* number of rows in _decomposition_ */
    const int NPCOLS= rootP; /* number of cols in _decomposition_ */
    const int BLOCKROWS = n/rootP;  /* number of rows in _block_ */
    const int BLOCKCOLS = n/rootP; /* number of cols in _block_ */

    if (rank == 0) {
        /* 
           for (int ii=0; ii<n*n; ii++) {
           array[ii] = ii;
           }
           */ 
        generateMatrix(array, n);
    }

    int *b = new int[BLOCKROWS*BLOCKCOLS];
    for (int ii=0; ii<BLOCKROWS*BLOCKCOLS; ii++) b[ii] = 0;

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;

    MPI_Type_vector(BLOCKROWS, BLOCKCOLS, COLS, MPI_INT, &blocktype2);
    MPI_Type_create_resized( blocktype2, 0, sizeof(int), &blocktype);
    MPI_Type_commit(&blocktype);

    int disps[NPROWS*NPCOLS];
    int counts[NPROWS*NPCOLS];
    for (int ii=0; ii<NPROWS; ii++) {
        for (int jj=0; jj<NPCOLS; jj++) {
            disps[ii*NPCOLS+jj] = ii*COLS*BLOCKROWS+jj*BLOCKCOLS;
            counts [ii*NPCOLS+jj] = 1;
        }
    }
    int color = rank % rootP; // Determine color based on row

    // Split the communicator based on the color and use the
    // original rank for ordering
    MPI_Comm colComm;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &colComm);

    int row_rank, row_size;
    MPI_Comm_rank(colComm, &row_rank);
    MPI_Comm_size(colComm, &row_size);
    MPI_Scatterv(array, counts, disps, blocktype, b, BLOCKROWS*BLOCKCOLS, MPI_INT, 0, MPI_COMM_WORLD);

    /* each proc prints it's "b" out, in order */
    for (int proc=0; proc<size; proc++) 
    {
        if (proc == rank) 
        {
            printf("Rank = %d\n", rank);
            if (rank == 0) 
            {
                printf("Global matrix: \n");
                for (int ii=0; ii<n; ii++) 
                {
                    for (int jj=0; jj<n; jj++) 
                    {
                        printf("%3d ",(int)array[ii*COLS+jj]);
                    }
                    printf("\n");
                }
            }
            printf("Local Matrix:\n");
            for (int ii=0; ii<BLOCKROWS; ii++) 
            {
                for (int jj=0; jj<BLOCKCOLS; jj++) 
                {
                    printf("%3d ",(int)b[ii*BLOCKCOLS+jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}

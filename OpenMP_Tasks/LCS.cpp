#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>


#define max(a,b) a>b?a:b;

int LCSLength (char *X, int m, char* Y, int n)
{
    int C[m+1][n+1];
    for(int i=0;i<=m;++i)
    {
        C[i][0] = 0;
    }
    for(int j=0;j<=n;++j)
    {    
        C[j][0] = 0;
    }
    for (int a=1; a<=m; ++a)
    {
        for (int b=1; b<=n; ++b)
        {
            if(X[a-1] == Y[b-1])
            {
                C[a][b] = C[a-1][b-1]+1;
            }
            else
            {
                C[a][b] = max(C[a][b-1], C[a-1][b]);
            }
        }
    }
    return C[m][n];

}



int main(int argc, char* argv[])
{
    
    if (argc < 3) 
    {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
        return -1;
    }

    int n = atoi(argv[1]);
    int numThreads = atoi(argv[2]); 
    if (n< numThreads)
        numThreads = n;
    omp_set_num_threads(numThreads);

    //forces openmp to create the threads beforehand
#pragma omp parallel
    {
        int fd = open (argv[0], O_RDONLY);
        if (fd != -1) {
            close (fd);
        }
        else {
            std::cerr<<"something is amiss"<<std::endl;
        }
    }

    int * arr = new int [n];

    
}

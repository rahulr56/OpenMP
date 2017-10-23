#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <algorithm>

/*
#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b
*/
int LCSLength_parallel (char *X, int m, char* Y, int n, int numThreads)
{
    int C[m+1][n+1];
#pragma omp parallel for
    for(int i=0;i<=m;++i)
    {
#pragma omp parallel for
        for(int j=0;j<=n;++j)
            C[i][j] = 0;
    }

    int a;

#pragma omp parallel shared(X, Y, C, m, n) private(b,col,a)
    {
        for(int a = 2; a <= m+n; a++)
        {
            int b, col;
#pragma omp for 
            for(b =  std::min(m, a-1); b >= std::max(1, a-n); b--)
            {
                col = a - b;

                if(X[b-1] == Y[col-1]) {
                    C[b][col] = C[b-1][col-1] + 1;
                }
                else {
                    C[b][col] = std::max( C[b-1][col], C[b][col-1] );
                }
            }
        }
    }
    return C[m][n];

}



/*
int LCSLength (char *X, int m, char* Y, int n,int numThreads = 1)
{
    int C[m+1][n+1];
    for(int i=0;i<=m;++i)
    {
        C[i][0] = 0;
    }
    for(int j=0;j<=n;++j)
    {    
        C[0][j] = 0;
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
                C[a][b] = std::max(C[a][b-1], C[a-1][b]);
            }
        }
    }
    return C[m][n];
}

int task_thread(int **C, char* X, char* Y, int rowStart, int rowEnd, int colStart, int colEnd)
{
    // std::cout<<"task_thread  \n\nRow : \n\tstart : "<<rowStart<<"\n\t End : "<<rowEnd<<"\nColumn\n\t Col Start : "<<colStart<<"\n\t Col End : "<<colEnd<<std::endl;
    for(int i= rowStart;i<rowEnd;i++)
    {
        for(int j = colStart; j < colEnd; j++)
        {
            // std::cout<<"i : "<<i<<"\tj : "<<j<<std::endl;
            continue;
            if(X[i-1] == Y[j-1])
            {
                if ( i-1< rowStart || j-1< colStart)
                   C[i][j] = 1;
                else 
                    C[i][j] = C[i-1][j-1]+1;
            }
            else
            {
                if ( i-1 < rowStart && j-1<colStart)
                {
                   C[i][j] = 0;
                }
                else if ( i-1< rowStart)
                {    
                    C[i][j] = std::max(C[i][j-1], 0);
                }
                else if(j-1< colStart)
                {
                    C[i][j] = std::max(0, C[i-1][j]);
                }
                else 
                {
                    C[i][j] = std::max(C[i][j-1], C[i-1][j]);
                }
            }
        }
    }
}
int LCSLength_P (char *X, int m, char* Y, int n,int numThreads = 1)
{
    std::cout<<"X : "<<X<<"\nY : "<<Y<<std::endl;
    // std::cout<<"in LCSLength_P\n";
    int **C;
    C = new int *[m];
    for(int i = 0; i < m; i++)
            C[i] = new int[n];
#pragma omp parallel for
    for(int i=0;i<m;++i)
    {
#pragma omp parallel for
        for(int j=0;j<n;++j)
            C[i][j] = 0;
    }
    // std::cout<<"init C in  LCSLength_P\n";

    int colGranularity = n/numThreads;
    int rowGranularity = m/numThreads;
    // std::cerr<<"Row : "<<m<<"\tColumn : "<<n<<std::endl;
    // std::cerr<<"Row Granularity : "<<rowGranularity<<"\tColumn Granularity : "<<colGranularity<<std::endl;

    //#pragma omp parallel for
#pragma omp parallel 
    {
#pragma omp single 
    for (int a=0; a<numThreads; ++a)
    {
        int rowStart = a * rowGranularity; 
        int rowEnd = (a + 1) * rowGranularity; 
        if (a == numThreads - 1)
            rowEnd = m;
        for (int b=0 ; b<numThreads; ++b)
        {
            int colStart = b * colGranularity; 
            int colEnd = (b + 1) * colGranularity; 
            if (b == numThreads - 1)
                colEnd = n; 
    // std::cout<<"Row : \n\tstart : "<<rowStart<<"\n\t End : "<<rowEnd<<"\nColumn\n\t Col Start : "<<colStart<<"\n\t Col End : "<<colEnd<<std::endl;
#pragma omp task 
            task_thread(C,X,Y,rowStart,rowEnd,colStart, colEnd); 
        }
    }
#pragma omp taskwait
    }
    for (int x=0;x<m;++x)
    {
        for (int y=0;y<n;++y)
            std::cout<<C[x][y]<<"\t";
        std::cout<<std::endl;
    }

    return C[m-1][n-1];
}

int lcs( char *X, char *Y, int m, int n )
{
   if (m == 0 || n == 0)
     return 0;
   if (X[m-1] == Y[n-1])
     return 1 + lcs(X, Y, m-1, n-1);
   else
     return std::max(lcs(X, Y, m, n-1), lcs(X, Y, m-1, n));
} 

*/
char* genString(int size)
{
    int seed = 97; 
    char *string = new char[size];
    srand(time(NULL)+rand()); 

    for (int i=0; i < size; i++)
    {
        string[i] = static_cast<char>(seed+ (rand()%26+1));
    }

    return string;
}


int main(int argc, char* argv[])
{

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

    if (argc < 3) 
    {
        std::cerr<<"Usage: "<<argv[0]<<" <string1_length> <string2_length> <nbthreads>"<<std::endl;
        return -1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    char* str1 = genString(n);
    char* str2 = genString(m);

    int numThreads = atoi(argv[3]); 
    if (m< numThreads)
        numThreads = m;
    omp_set_num_threads(numThreads);

    std::cout<<LCSLength_parallel(str1, m ,str2,n, numThreads)<<std::endl;
}

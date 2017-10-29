# Distributed Parallel Computing 

## Reduction

## Heat Equation
### Algorithm for Block data partition
```
calculateHeatUsingBlock(heatArr, N , p, P)
{
    begin = p * (N/P)
    end = (p+1) * (N/P)
    if (begin == 0)
    {    
        heatArr[0] = (2*heatArr[0] + heatArr[1])/3
    }
    else
    {
        recv heatVal from p-1
        heatArr[begin] = (heatArr[begin] + heatVal + heatArr[begin + 1])/3
    }
    if (end == N)
    {
        heatArr[N-1] = (2*heatArr[N-1] + heatArr[N-2])/3
        end = N-1
    }
    for ( i = begin+1 ;  i < end ; ++i)
    {

        heatArr[begin] = (heatArr[begin - 1] + heatArr[begin] + heatArr[begin + 1])/3
    }
    if (p != p-1)
    {
        send heatArr[end-1] to p+1 
    }
}
```
*__Communication per iteration is__*
```
θ(1) or 0
```
*__Total Communication:__*
```
θ( P * Communication(1) ) 
```

### Algorithm for Round Robin data partition
```
calculateHeatUsingRoundRobin(heatArr, N, p, P)
{
    create an array X of size P 
    for (i = 0 ; i < P ; ++i)
    {
        X[i] = (N/P) * i + p
    }

    for (i = 0; i < P; ++i)
    {
        if(X[i] == 0)
        {
            heatArr[0] = (2 * heatArr[0] + heatArr[1])/3
            send heatArr[0] to p1 
        }
        else if (X[i] == N-1)
        {
            recv heatArr[N-1] from P-1
            heatArr[N-1] = (2 * heatArr[N-1] + heatArr[N-2])/3
        }
        else
        {
            recv heatArr[X[i]-1] from P-1
            heatArr[X[i]] = (heatArr[X[i]-1] + heatArr[X[i]] + heatArr[X[i+1]])/3
            send heatArr[X[i]] to p+1
        }
    }
}
```
*__Total Communication:__*
```
θ( N * Communication(1) ) 
```
*__Communication per iteration is:__*
```
θ( P * Communication(1) ) 
```

### Best Algorithm
Since the communication is very costly in a disrtibuted computing environment, I would choose block data partitioning over Round Robin data partition. Suppose that the cost of communication is less than that of the wait time, I would prefer Round Robin data partitioning over block data partition.


## Dense Matrix Multiplication
### Algorithm for Horizontal data partion
```
matmulHorizontalPartitioning(A, vectorX, N, p, P)
{
    if(p == 0)
    {
        create an array  of size (N X 1)
    }
    begin = p*(N/P)
    end = (p + 1) * (N/P)
    Create and initialize an array val[begin - end] to 0s.
    for (i = begin; i < end; ++i)
    {
        for (j = 0; j < N; ++j )
        {
            val[i] += (A[i][j] * vectorX[j])
        }
    }
    if (p == 0)
    {
        copy val array computed in p0 to Y.
        for (i=1; i < P; ++i)
        {
            recv computedVal from node k
            copy computedVal array to Y.
        }
    }
    else
    {
        send val to 0
    }
}
```
*__Memory Consumed:__*

Every node other than Node 0 takes the memory size of `N/P` array size. So, their memory consumption is `θ(N/P)`. Whereas Node 0 creates and array Y of size N, Hence, its memory consumption is `θ(N)`. 

*__Communication per iteration:__*
Each node computes the value of a 1d array of size `N/P`. Suppose that `X` is the communication overhead required to send an array of size `N/P`, the total communication overhead for this algorithm is 
```
θ( P * Communication(X) )
```
### Algorithm for Vertical data partion (Chain)
```
matmulVerticalPartitioning(arr, rows, cols, p, P):
{
    begin = p * (N/P)
    end = (p + 1) * (N/P)
    if(p != 0)
    {
        recv computedVal from p-1
    }
    else
    {
        create and initialize an array computedVal of size N to 0
    }
    for (i = 0; i < N; ++i)
    {
        for(j = begin; j < end; ++j)
        {
            computedVal[i] += (arr[i][j] * vectorX[j])
        }
    }
    if(p != P-1)
    {
        send computedVal to p+1
    }
    else 
    {
        print array computedVal
    }
}
```

*__Memory Consumed:__*
Node 0 creates an array of size `N` to store the computed results and sends this array to the adjacent node. The next node computes values and stores them in the same array. Hence, the total memory consumption is `θ(N)`

*__Communication per iteration:__*
As this algorithm is CHAIN structured, there are no  or `θ(1)` communictions happening in every iteration.

*__Total Communications__*
The total communication oevrhead is `θ(P * Communication(N/P))`

### Algorithm for Block data partion
```
matmulBlockPartitioning(A, vectorX, N, p, P)
{
    createan array val of size (N/P) and initialize to 0
    begin = p * (N/P)
    end = (p + 1) * (N/P)
    if(p != 0)
    {
        recv computedVal from p-1
    }
    else
    {
        create an array computedVal and initialize to 0s.
    }
    for (i = begin; i < end; ++i)
    {
        for (j = begin; j < end; ++j)
        {
            val[i] += (A[i][j] * vectorX[j])
        }
        val[i] += computedVal[i]
    }
    if( p != P-1)
    {
        send val to p+1
    }
}
```

*__Memory Consumed:__*
Each node creates a local arry of size `N/P` to compute the value.

*__Communication per iteration:__*
```
```

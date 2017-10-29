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
Communication per iteration is 
```
θ( P * Communication(1) )
```

### Algorithm for Round Robin data partition
```
calculateHeatUsingRoundRobin(arr, N, p, P)
{

}
```
Communication per iteration is:
```
θ( N * Communication(1) ) 
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
        create an array  os size (N X 1)
    }
    begin = p*(N/P)
    end = (p + 1) * (N/P)
    val = 0
    for (i = begin; i < end; ++i)
    {
        for (j = 0; j < N; ++j )
        {
            val += (A[i][j] * vectorX[j])
        }
    }
    if (p == 0)
    {
        recv computedVal from node k
        y[k] = computedVal
    }
    else
    {
        send val to 0
    }
}
```
Memory Consumed:

Every node other than Node 0 takes the memory size of 1 extra variable. So, their memory consumption is `θ(1)`. Whereas Node 0 creates and array Y of size N, Hence, its memory consumption is `θ(N)`. 

Communication per iteration:
```
```
### Algorithm for Vertical data partion
```
```

Memory Consumed:
```
```
Communication per iteration:
```
```
### Algorithm for Block data partion
```
```

Memory Consumed:
```
```
Communication per iteration:
```
```

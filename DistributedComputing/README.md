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

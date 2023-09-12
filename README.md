# SHALE-Cplusplus
C++ version of SHALE paper, ad inventory allocation

## Paper
[SHALE: An Efficient Algorithm for Allocation of Guaranteed Display Advertising](https://arxiv.org/pdf/1203.3619.pdf)

## How to run code

1. do cmake
```
cmake .
```
2. make
```
make
```
3. run binary
```
./shale
```

Output looks like
```
Allocation:
demand_node     demand          allocation
1       400     400
2       700     700
3       1100    0

Remained:
supply_node     inventory       remained
0       200     0
1       200     0
2       300     0
3       400     0
4       1200    1200
```

You may change supply/demand from supply.txt/demand.txt to supply_enough.txt/demand_enough.txt, which same as HWM (thetaIJ) result.
Output looks like
```
Allocation:
demand_node     demand          allocation
1       200     206
2       300     291
3       1000    999

Remained:
supply_node     inventory       remained
0       200     0
1       200     0
2       300     176
3       400     34
4       1200    594
```

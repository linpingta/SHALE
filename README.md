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

...
Supply:
supply_node     inventory       satisfy_demand
4               1200            3,
3               400             2,3,
2               300             2,
1               200             1,3,
0               200             1,3,

...

```

Still try to improve code, update anytime.

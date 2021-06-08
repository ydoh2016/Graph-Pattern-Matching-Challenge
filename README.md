# Graph Pattern Matching Challenge
***You may use basic pruning techniques, but since the challenge aims to find a good matching order, you are not expected to use complicated pruning techniques (like Failing Set of DAF).***
## Compile and Execute 
### main program
```
mkdir build
cd build
cmake ..
make
./main/program <data graph file> <query graph file> <candidate set file>
```
### executable program that outputs a candidate set
```
./executable/filter_vertices <data graph file> <query graph file>
```
### References
[1] Myoungji Han, Hyunjoon Kim, Geonmo Gu, Kunsoo Park, and Wook-Shin Han. 2019. Efficient Subgraph Matching: Harmonizing Dynamic Programming, Adaptive Matching Order, and Failing Set Together. In Proceedings of the 2019 International Conference on Management of Data (SIGMOD '19). Association for Computing Machinery, New York, NY, USA, 1429–1446. DOI:https://doi.org/10.1145/3299869.3319880

## Submit (이재필, 도양훈)
report : https://github.com/ydoh2016/Graph-Pattern-Matching-Challenge/blob/master/%5B%EC%95%8C%EA%B3%A0%EB%A6%AC%EC%A6%98%5DGraph-Pattern-Match-Challenge_Report_%EC%9D%B4%EC%9E%AC%ED%95%84%2C%20%EB%8F%84%EC%96%91%ED%9B%88.pdf

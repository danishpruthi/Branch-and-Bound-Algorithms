Group Information :
   Akhilesh Anandh 2011A7PS030P
   Danish          2011A7PS037P

   Problem Statement : Branch And Bound
   Problems Selected : 0/1 Knapsack and TSP
***********************************************************************************
Problem statement : 
1. Design a hybrid Branch-and-Bound Skeleton scheme for a target platform that is
   a cluster of workstations.
2. Choose two hard / pseduo-hard problems (e.g. 0/1 KnapSack, TSP, Graph Coloring,
   Factorization) and encode them using the Branch and Bound skeletons in each case.
3. Implement your solution using MPI for message passing programming on the cluster
   and OpenMP for shared memory programming within a single node.
 
Implement your solution in three steps:
	A) MPI version where each process is single-threaded
	B) OpenMP version to run in a multicore node	
	C) A hybrid version combining both of these.

***********************************************************************************
Assignment Status :All the three modules are implemented and working effectively.
***********************************************************************************
OpenMP Version 

In the OpenMP Subdirectory : 
------------------------------
For running Knapsack Problem ;
g++ -fopenmp knapsack.cpp
time ./a.out < knapsack.txt
------------------------------
For running TSP Problem :
g++ -fopenmp tsp.cpp
time ./a.out < tsp.txt
------------------------------
NOTE : On my quad core processor, the speedup for the problems is a little over 3
Also the sequential codes can be run by avoiding the openmp flag during compilation
***********************************************************************************
MPI Version 

In the MPI Subdirectory :
-----------------------------------
For running Knapsack Problem ;
mpic++ knapsack.cpp
time mpirun -np x ./a.out < knapsack.txt

where x is the number of processors
----------------------------------
For running TSP Problem 
mpic++ tsp.cpp
time mpriun -np x ./a.out < tsp.txt

where x is the number of processors
----------------------------------
***********************************************************************************
Hybrid Model

In the Hybrid Subdirectory :
----------------------------------
For running knapsack problem : 
mpic++ -fopenmp knapsack.cpp
time mpirun -np x ./a.out < knapsack.txt

where x is the number of processors
----------------------------------
For running TSP Problem 
mpic++ -fopenmp tsp.cpp
time mpriun -np x ./a.out < tsp.txt

where x is the number of processors
----------------------------------
***********************************************************************************
INPUT OUTPUT FORMAT for other test cases :
There are two test cases in each folder, incase you wish to test it on more cases
The formats are :
----------------------------------
Knapsack Problem
N             (the number of bags in the knapsack problem)            
W1 W2 W3 W4 ......Wn (Wi is the weight of the ith bag)
V1 V2 V3 V4 ......Vn (Vi is the value of the ith bag)
----------------------------------
TSP Problem
N (the number of vertices)
NxN graph  [Adjacency Matrix]

NOTE : If there is no edge from u->v then the value of Graph[u][v]=any large value
***********************************************************************************
                                END
***********************************************************************************

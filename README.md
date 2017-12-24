Make
----
   ```
   make
   ```
   For cleaning
   ```
   make clean
   ```

Prefix Sum
----
1. Source code using MPI\_Scan
   ```
   prefix_sum_scan.c
   ```

2. Source code using MPI\_Send/Receive
   ```
   prefix_sum_impl.c
   ```   

3. Run
   Using mpirun with argument N for # of digits to sum
   ```
   e.g.,
   mpirun -np 4 --hostfile hosts ./prefix_sum_scan 1000
   ```

Quick Sort
----
1. Source code for parallel quick sort using MPI
   ```
   quicksort_mpi.c
   ```

2. Run
   Using mpirun with argument N for # of digits to sort
   ```
   e.g.,
   mpirun -np 4 --hostfile hosts ./quicksort_mpi 1000
   ```


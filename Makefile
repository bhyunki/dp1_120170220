CC=mpicc

all : prefix quicksort

prefix:
	$(CC) -o prefix_sum_scan prefix_sum_scan.c 
	$(CC) -o prefix_sum_impl prefix_sum_impl.c 

quicksort:
	$(CC) -o quicksort_mpi quicksort_mpi.c -lm

clean:
	[ -x ./prefix_sum_scan ] && rm prefix_sum_scan
	[ -x ./prefix_sum_impl ] && rm prefix_sum_impl
	[ -x ./quicksort_mpi ] && rm quicksort_mpi


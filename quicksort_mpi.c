#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

void Swap(int *a, int i, int j){
	int p=a[i];
	a[i]=a[j];
	a[j]=p;
}

int Partition(int *a, int m, int p){
	//arr[m], arr[m+1], ... , arr[p-1]
	int v=a[m], i=m, j=p;

	do{
		do i++;
		while(a[i] < v);
		do j--;
		while(a[j] > v);
		if(i < j) Swap(a, i, j);
	}while(i < j);
	a[m]=a[j]; 
	a[j]=v;
	return j;
}
		

void QuickSort(int *arr, int p, int q){
	//a[p], ..., a[q]

	if(p < q){
		int j = Partition(arr, p, q+1);
		QuickSort(arr, p, j-1);
		QuickSort(arr, j+1, q);
	}
}

int main(int argc, char *argv[]){
	int *arr, i, N, rank, size, *rcvbuf, level, j, num;
	int limit, plimit, before, next;
	MPI_Status stat;
	struct timeval st, fn;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	sscanf(argv[1], "%d", &N);
	arr=(int*)calloc(N, sizeof(int));
	rcvbuf=(int*)calloc(N, sizeof(int));

	if(rank==0){
		srand(time(NULL));
		for(i=0; i<N; i++){
			arr[i]=rand()%1000;
			printf("%2d ",arr[i]);
		}
		printf("\n");
	}

	//if rank < 2^level
	//partition and send later part to [rank + 2^level]
	//e.g.
	//                     0			: level 0
	//                  [0 -> 1]			: level 1
	//       [0 -> 2]              [1 -> 3]		: level 2
	// [0 -> 4]   [2 -> 6]   [1 -> 5]   [3 -> 7]	: level 3

	if( rank == 0 )
		gettimeofday(&st, NULL);


	num = N;
	MPI_Barrier(MPI_COMM_WORLD);

	for(level=1; level<=(int)log2((double)size)+1 ; level++){
		limit = (int)pow((double) 2, level);

		if( rank < limit ){
			plimit = limit/2; // ==pow(2, level-1)
			next = rank + limit/2; // ==rank+pow(2, level-1);

			if( rank >= plimit){ //Receiver
				before = rank - limit/2; // ==rank-pow(2, level-1)
				MPI_Recv(&num, 1, MPI_INT, before, 0, MPI_COMM_WORLD, &stat);//Receive num
				MPI_Recv(arr, num, MPI_INT, before, 1, MPI_COMM_WORLD, &stat);//Receive array
			}
			else if( size > next ){ //Sender, if there is more process
				int q ;//= num - j;
			
				j = Partition (arr, 0, num);

				if( j == -1 )
					j=0;

				q = num - (j+1);
				MPI_Send(&q, 1, MPI_INT, next, 0, MPI_COMM_WORLD);//Send number of array ( q-j+1 )
				MPI_Send(arr+j+1, q, MPI_INT, next, 1, MPI_COMM_WORLD);// Send array
				num=j+1;
			}
			else //if there is no more proces, break
				break;
		}
	}

//	MPI_Barrier(MPI_COMM_WORLD);

	if( num != 0 )
		QuickSort(arr, 0, num-1); // Do QuickSort for local partial array

	for( i = plimit; i>0; i=i/2){
		if( rank < i && rank+i < size){
			// next = rank + plimit
			int q=0;
			MPI_Recv( &q, 1, MPI_INT, rank+i, 0, MPI_COMM_WORLD, &stat );
			MPI_Recv( rcvbuf, q, MPI_INT, rank+i, 1, MPI_COMM_WORLD, &stat );

			for( j=0; j<q; j++)
				arr[num+j]=rcvbuf[j];
			num+=q;
		}
		else if( rank >= i && rank < i*2){ // rank >= plimit 
		      // before = rank - plimit;
			MPI_Send( &num, 1, MPI_INT, rank-i, 0, MPI_COMM_WORLD);
			MPI_Send( arr, num, MPI_INT, rank-i, 1, MPI_COMM_WORLD);
		}
	}

	if( rank == 0 )
		gettimeofday(&fn, NULL);

	if( rank == 0 ){
		printf("\nQUICK SORT RESULT\nNUM : %d\n", num);
		for(i=0; i<num; i++)
			printf("%2d ", arr[i]);
		printf("\n");
		printf("Elapsed Time : %ld.%06ld\n", fn.tv_sec-st.tv_sec, fn.tv_usec-st.tv_usec);
	}

	MPI_Finalize();

	return 0;
}

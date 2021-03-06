#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int *sndbuf, *rcvbuf, N, i, rank, size, *res, temp;
	double start, finish;
	MPI_Status stat;

	struct timeval st, fn;

	MPI_Init(&argc, &argv);

	if(argc<2){
		printf("Usage example :\n\te.g., mpirun -np 4 -hostfile hosts ./prefix_sum_scan 4\n");
		return -1;
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	sscanf(argv[1], "%d", &N);
	sndbuf=(int*)calloc(N, sizeof(int));
	rcvbuf=(int*)calloc(N, sizeof(int));
	res=(int*)calloc(N, sizeof(int));
	
	start=MPI_Wtime();
	gettimeofday(&st, NULL);

	if(rank==0){
		srand(time(NULL));
		for(i=0; i<N; i++){
			sndbuf[i]=rand()%10;
			printf("%d ", sndbuf[i]);
		}
		printf("\n");
	}


	temp=0;
	for( i=0; i<=N/size; i++){
		sndbuf[i*size]+=temp;
		MPI_Scatter(sndbuf+i*size, 1, MPI_INT, rcvbuf+i, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scan(rcvbuf+i, res+i, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		
		if(rank==size-1)
			MPI_Send(res+i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		else if(rank==0){
			MPI_Recv(&temp, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, &stat);	
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	finish=MPI_Wtime();
	gettimeofday(&fn, NULL);

	for( i=0; i<N/size; i++){
		printf("result[%3d]-%3d: %6d ( %ld.%06ld )\n", rank , i, res[i], fn.tv_sec-st.tv_sec, fn.tv_usec-st.tv_usec);
//		printf("%d %d\n", fn.tv_sec - st.tv_sec, fn.tv_usec - st.tv_usec);
	}

	if( rank < N%size )
		printf("result[%3d]-%3d: %6d ( %ld.%06ld )\n", rank , i, res[i], fn.tv_sec-st.tv_sec, fn.tv_usec-st.tv_usec);
//		printf("result[%d]-%d: %d\n", rank, i, res[i]);

//		free(sndbuf);
//		free(rcvbuf);

	MPI_Finalize();

	return 0;
}

#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[])
{	
	const int tag = 42;
	int id, ntasks, source_id, dest_id, err, i;
	MPI_Status status;
	int msg[2];


	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Mpi initialization failed\n");
		return 1; // exit(1);
	}

	err = MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
	err = MPI_Comm_rank(MPI_COMM_WORLD, &id);
	// printf("%d\n", ntasks);
	if (ntasks < 2) {
		printf("Your program need at least 2 processors to run.\n");
		return 1; // exit(0);
	}

	if (id == 0) {
		for (i = 1; i < ntasks; ++i) {
			err = MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, tag, \
			 				MPI_COMM_WORLD, &status);
			source_id = status.MPI_SOURCE;
			printf("Received message %d %d from %d\n", msg[0], msg[1], source_id);
		}
	}
	else {
		msg[0] = id;
		msg[1] = ntasks;
		dest_id = 0;
		err = MPI_Send(msg, 2, MPI_INT, dest_id, tag, MPI_COMM_WORLD);
	}

	err = MPI_Finalize();
	if (id == 0) {
		printf("Ready\n");
	}
	// exit(0);
	return 0;
}
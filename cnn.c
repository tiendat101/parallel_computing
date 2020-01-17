#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#define N 12  //size of input data (N * N)
#define A 3  //size of filter window (A * A)

#define N_cities 12

//
int data[N][N];
int filter[A][A] = {{1, 1, 1},
					{0, 0, 0},
					{-1, -1, -1}};
// int i, j;

/////////////////////////////////////
int tsp_data[N_cities][N_cities];
int tsp_result[N_cities];
int tsp_temp[N_cities];
int visit[N_cities];
int best;
int cost;



//
int read_input(char* file_name) {
	FILE *f;
	f = fopen(file_name, "r");
	if (f == NULL) {
		return 0;
	}
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			fscanf(f, "%d", &data[i][j]);			
		}
	}
	fclose(f);	
	return 1;
}

int get_data(int h) {
	int x, y;
	x = (h / (A * A)) / (N - A + 1);
	y = (h / (A * A)) % (N - A + 1);

	x = (h % (A * A)) / A + x;
	y = (h % (A * A)) % A + y;	
	return data[x][y];
}

int get_filter(int h) {
	int x = h % (A * A) / A;
	int y = h % (A * A) % A;
	return filter[x][y];
}


///////////////////////////////////////////////////////
// TSP

int read_input_tsp(char* file_name) {
	FILE *f;
	f = fopen(file_name, "r");
	if (f == NULL) {
		return 0;
	}
	for (int i = 0; i < N_cities; ++i) {
		for (int j = 0; j < N_cities; ++j) {
			fscanf(f, "%d", &tsp_data[i][j]);			
		}
	}
	fclose(f);	
	return 1;
}

void check_tsp(int k) {
	for (int i = 0; i < N_cities; ++i) {
		if (visit[i] == 0) {
			visit[i] = 1;
			cost += tsp_data[tsp_temp[k - 1]][i];
			tsp_temp[k] = i;
			//
			if (k == N_cities - 1) {
				if (cost < best) {
					best = cost;
					for (int i = 0; i < N_cities; ++i) {
						tsp_result[i] = tsp_temp[i];
					}
				}
			}
			else {
				check_tsp(k + 1);
			}
			// revert previous values
			visit[i] = 0;
			cost -= tsp_data[tsp_temp[k - 1]][i];
		}
	}
}

void init_tsp() {
	for (int i = 0; i < N_cities; ++i) {
		visit[i] = 0;
	}
	tsp_result[0] = 0;
	tsp_temp[0] = 0;
	visit[0] = 1;
	best = 2000000000;
	cost = 0;
}


int main(int argc, char *argv[])
{
	// // 
	int rank, n_process;
	int T = (N - A + 1) * (N - A + 1); // output will have shape of (N - A + 1) * (N - A + 1)
	int p[T], conv[T];
	for (int i = 0; i < T; ++i) p[i] = 0;
	int lena = A * A * (N - A + 1) * (N -A + 1); // length of the virtual array a
	int err;
	MPI_Status status;
	const int tag = 42;
	int dest_id;
	double start_time;
	double time[10];
	for (int i = 0; i < 10; ++i) {
		time[i] = 0;
	}
	double mean_time = 0;
	char input_tsp[20];
	char input_cnn[20];


	//
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Mpi initialization failed\n");
		return 1; // exit(1);
	}	

	MPI_Comm_size(MPI_COMM_WORLD, &n_process);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
			// printf("Problem 1: Convolution filter. \n");


	for (int k = 0; k < 10; ++k) {

		for (int i = 0; i < T; ++i) p[i] = 0;

		// read input data in process 0
		if (rank == 0) {
			start_time = MPI_Wtime();
			sprintf(input_cnn, "cnn_input%d.txt", k);
			int nerr = read_input(input_cnn);
			if (!nerr) {
				printf("Error Reading File!\n");
				return 0;
			}
			printf("Data: \n");
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					printf("%d ", data[i][j]);			
				}
				printf("\n");
			}
		}

		// brocast data to other processes
		MPI_Bcast(data, N*N, MPI_INT, 0, MPI_COMM_WORLD);

		// convolution
		int a, f;
		for (int i = rank; i < lena; i += n_process) {
			// printf("rank is %d\n", rank);
			a = get_data(i);
			f = get_filter(i);
			p[i / (A * A)] += a * f;
			// printf("%d %d\n", a, f);
		}


		MPI_Reduce(p, conv, T, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // conv is result of convolution computing

		// MPI_Bcast(conv, T, MPI_INT, 0, MPI_COMM_WORLD);

		if (rank == 0) {
			printf("Result: \n");
			for (int i = 0; i < T; ++i) {
				if (i % (N - A + 1) == 0) {
					printf("\n");
				}
				printf("%d ", conv[i]);
			}
			printf("\n Total run time: %f\n", MPI_Wtime() - start_time);
		}

			// // MPI_Wtime

			if (rank == 0) {
				// printf("Total run time: %f\n", MPI_Wtime() - start_time);
				printf("%f\n", MPI_Wtime() - start_time);
				time[k] = MPI_Wtime() - start_time;
				mean_time += time[k];
				printf("%f\n", time[k]);
			}

	}

	if (rank == 0) {
		printf("Avarage run time: %f\n", mean_time / 10);
	}

	// //////////////////////////////////////////////////////////////
	// /* Problem 2: TSP using Backtracking
	// */

	// for (int k = 0; k < 10; ++k) {
	// 	// int k = 0;
	// 	// read input data
	// 	if (rank == 0) {
	// 		start_time = MPI_Wtime();
	// 		// input_tsp = 'tsp_input' + str(k) + '.txt';
	// 		sprintf(input_tsp, "tsp_input%d.txt", k);

	// 		printf("\n Problem 2: TSP using Backtracking \n");

	// 		err = read_input_tsp(input_tsp);
	// 		if (!err) {
	// 			printf("Can not read input file. \n");
	// 			return 1;
	// 		}
	// 		for (int i = 0; i < N_cities; ++i) {
	// 			for (int j = 0; j < N_cities; ++j) {
	// 				printf("%d ", tsp_data[i][j]);
	// 			}
	// 			printf("\n");
	// 		}
	// 	}

	// 	// brocast data to other processes
	// 	MPI_Bcast(tsp_data, N_cities*N_cities, MPI_INT, 0, MPI_COMM_WORLD);

	// 	// initialize values
	// 	init_tsp();

	// 	// solution
	// 	for (int i = rank; i < N_cities; i += n_process) {
	// 		if (visit[i] == 0) {
	// 			visit[i] = 1;
	// 			cost += tsp_data[tsp_temp[0]][i];
	// 			// printf("%d\n", cost);
	// 			tsp_temp[1] = i;
	// 			//
	// 			if (1 == N_cities - 1) {
	// 				if (cost < best) {
	// 					best = cost;
	// 					for (int i = 0; i < N_cities; ++i) {
	// 						tsp_result[i] = tsp_temp[i];
	// 					}
	// 				}
	// 			}
	// 			else {
	// 				check_tsp(2);
	// 			}
	// 			// revert previous values
	// 			visit[i] = 0;
	// 			cost -= tsp_data[tsp_temp[0]][i];
	// 		}
	// 	}

	// 	// if (rank == 0) {
	// 	// 	printf("%d\n", best);
	// 	// 	for (int i = 0; i < N_cities; ++i) {
	// 	// 		printf("%d ", tsp_result[i]);
	// 	// 	}
	// 	// }

	// 	// collect all solutions from all precesses
	// 	int costi;
	// 	int best_rank = 0;
	// 	if (rank == 0) {
	// 		for (int i = 1; i < n_process; ++i) {
	// 			err = MPI_Recv(&costi, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
	// 			if (err != MPI_SUCCESS) {
	// 				printf("MPI_Recv Error! \n");
	// 				return 1;
	// 			}
	// 			// printf("%d ", costi); printf("\n");
	// 			if (costi < best) {
	// 				best = costi;
	// 				best_rank = status.MPI_SOURCE;
	// 			}
	// 		}
	// 	}
	// 	else {
	// 		dest_id = 0;
	// 		err = MPI_Send(&best, 1, MPI_INT, dest_id, tag, MPI_COMM_WORLD);
	// 		if (err != MPI_SUCCESS) {
	// 			printf("MPI_Send Error. \n");
	// 			return 1;
	// 		}
	// 	}

	// 	// //
	// 	// if (rank == best_rank) {
	// 	// 	dest_id = 0;
	// 	// 	err = MPI_Send(tsp_result, N_cities, MPI_INT, dest_id, 2, MPI_COMM_WORLD);
	// 	// 	if (err != MPI_SUCCESS) {
	// 	// 		printf("MPI_Send Error. \n");
	// 	// 		return 1;
	// 	// 	}		
	// 	// }

	// 	if (rank == 0) {
	// 		printf("Minimum cost: %d\n", best);
	// 		printf("Path: ");
	// 		for (int i = 0; i < N_cities; ++i) {
	// 			printf("%d ", tsp_result[i]);
	// 		}
	// 		printf("\n");
	// 	}


		
	// // MPI_Wtime

	// 	if (rank == 0) {
	// 		// printf("Total run time: %f\n", MPI_Wtime() - start_time);
	// 		printf("%f\n", MPI_Wtime() - start_time);
	// 		time[k] = MPI_Wtime() - start_time;
	// 		mean_time += time[k];
	// 		printf("%f\n", time[k]);
	// 	}

	// }










	// //
	// if (rank == 0) {
	// 	printf("Avarage run time: %f\n", mean_time / 10);
	// }


	err = MPI_Finalize();
	if (rank == 0) {
		printf("\n The end. \n");
	}

	return 0;
} 
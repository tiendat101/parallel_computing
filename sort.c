#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

const int SERVER_RANK = 0;
const int MESSAGE_TAG_0 = 0;
const int MESSAGE_TAG_1 = 1;
const int ARRAY_SIZE = 10000;
int A[10000];
void swap(int* a, int* b);
void SelSort(int arr[], int N);
void disArr(int arr[], int N);
int read_input(char* file_name);
void bubbleSort(int a[], int n);

int main( int argc, char *argv[]) {
	int rank, number_of_processes, data, root, sum, T;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	
	if (rank == 0) {
			int nerr = read_input("input_1.txt");
			if (!nerr) {
				printf("Error Reading File!\n");
				return 0;
			}

			for(int i = 1; i < number_of_processes; i++){
				MPI_Send(&A, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);		
			}	
	}
		
	for(int i = 1; i < number_of_processes; i++){
		if(rank == i)
		{
			MPI_Recv(&A, ARRAY_SIZE, MPI_INT, SERVER_RANK, 0, MPI_COMM_WORLD, &status);
		}	
	}

	printf("Data before sorting rank %d: \n", rank);
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		printf("%d ", A[i]);			
	}

	printf("\n");

	bubbleSort(A, ARRAY_SIZE);

	for(int i = 1; i < number_of_processes; i++){
		if(rank == i){
			MPI_Send(&A, ARRAY_SIZE, MPI_INT, SERVER_RANK, 1, MPI_COMM_WORLD);
		}	
	}


	for(int i = 1; i < number_of_processes; i++){
		if(rank == 0){
			MPI_Recv(&A, ARRAY_SIZE, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
		}	
	}


	if(rank == 0){
		printf("Data after sorting: \n");
		for (int i = 0; i < ARRAY_SIZE; ++i) {
		printf("%d ", A[i]);			
	}
	printf("\n");	
	}

	MPI_Finalize();
}


int read_input(char* file_name) 
{
	FILE *f;
	f = fopen(file_name, "r");
	if (f == NULL) 
	{
		return 0;
	}
	for (int i = 0; i < ARRAY_SIZE; ++i)
	{
		fscanf(f, "%d", &A[i]);			
	}
	fclose(f);
	return 1;
}


void bubbleSort(int a[], int n)
{
	int i, j;
	int temp = 0;
	for (i = 0 ; i < n - 1; i++)
  	{
    	for (j = 0 ; j < n - i - 1; j++)
    	{
      		if (a[j] > a[j+1]) /* For decreasing order use < */
      	{
        temp = a[j];
        a[j] = a[j+1];
        a[j+1] = temp;
      }
    }
  }
  printf("\n");
}

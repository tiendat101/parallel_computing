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
void merge(int arr[], int l, int m, int r); 
void mergeSort(int arr[], int l, int r); 

int main( int argc, char *argv[]) {
	int rank, number_of_processes, data, sum, T;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	
	if (rank == 0) {
			int nerr = read_input("input_2.txt");
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

	mergeSort(A, 0, ARRAY_SIZE - 1); 
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


void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    int L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 

void mergeSort(int arr[], int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
  
        merge(arr, l, m, r); 
    } 
} 

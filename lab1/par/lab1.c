#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"

#define ROW 8
#define COLUMN 8
#define send_data_tag 2001
#define return_data_tag 2002

void problemOneMaster(int matrix[ROW][COLUMN], int iterations, int np) {
	MPI_Status status;
	int i, j, p;
	int array[ROW][COLUMN];
	for (p = 1; p < np; p++) {
		i = (int)(p - 1) / 4;
		j = ((p - 1) % 4) * 2;
		int c_buffer[5] = {matrix[i][j], matrix[i][j+1], i, j, iterations};
		MPI_Send(&c_buffer, 5, MPI_INT, p, send_data_tag, MPI_COMM_WORLD);
	}

	for (p = 1; p < np; p++) {
		int results[4];
		MPI_Recv(&results, 4, MPI_INT, MPI_ANY_SOURCE, return_data_tag, MPI_COMM_WORLD, &status);
		array[results[2]][results[3]] = results[0];
		array[results[2]][results[3]+1] = results[1];
	}

	for (i = 0; i < ROW; ++i) {
		for (j = 0; j < COLUMN; ++j) {
				printf("%d\t", array[i][j]);
		}
		printf("\n");
	}	
}

void problemOneSlave() {
	MPI_Status status;
	int recv_buffer[5];
	int send_buffer[4];

	MPI_Recv(&recv_buffer, 5, MPI_INT, 0, send_data_tag, MPI_COMM_WORLD, &status);

	int k;
	int temp[2] = { recv_buffer[0], recv_buffer[1] };
	for (k = 0; k <= recv_buffer[4]; k++) {
		usleep(1000);
		temp[0] = temp[0] + (recv_buffer[2]+recv_buffer[3]) * k;
		temp[1] = temp[1] + (recv_buffer[2]+recv_buffer[3]+1) * k;
	}

	send_buffer[0] = temp[0];
	send_buffer[1] = temp[1];
	send_buffer[2] = recv_buffer[2];
	send_buffer[3] = recv_buffer[3];

	MPI_Send(&send_buffer, 4, MPI_INT, 0, return_data_tag, MPI_COMM_WORLD);
}

void problemTwoMaster(int matrix[ROW][COLUMN], int iterations, int np) {
	MPI_Status status;
	int i, j, p;
	int array[ROW][COLUMN];
	for (p = 1; p < np; p++) {
		i = (int)(p - 1) / 4;
		j = ((p - 1) % 4) * 2;
		int c_buffer[5] = {matrix[i][j], matrix[i][j+1], i, j, iterations};
		MPI_Send(&c_buffer, 5, MPI_INT, p, send_data_tag, MPI_COMM_WORLD);
	}

	for (p = 1; p < np; p++) {
		int results[4];
		MPI_Recv(&results, 4, MPI_INT, MPI_ANY_SOURCE, return_data_tag, MPI_COMM_WORLD, &status);
		array[results[2]][results[3]] = results[0];
		array[results[2]][results[3]+1] = results[1];
	}

	for (i = 0; i < ROW; ++i) {
		for (j = 0; j < COLUMN; ++j) {
				printf("%d\t", array[i][j]);
		}
		printf("\n");
	}
}

void problemTwoSlave() {
	MPI_Status status;
	int recv_buffer[5];
	int send_buffer[4];

	MPI_Recv(&recv_buffer, 5, MPI_INT, 0, send_data_tag, MPI_COMM_WORLD, &status);

	int k, j;
	int temp[8] = { recv_buffer[0], recv_buffer[0], recv_buffer[0], recv_buffer[0], recv_buffer[0],recv_buffer[0], recv_buffer[0], recv_buffer[0] };
	for (k = 0; k <= recv_buffer[4]; k++) {
		for (j = 0; j <= recv_buffer[3]+1; j++) {
			usleep(1000);
			if (j == 0) {
				temp[j] = temp[j] + (recv_buffer[2] * k);
			} else {
				temp[j] = temp[j] + temp[j-1] * k;
			}
		}
	}

	send_buffer[0] = temp[recv_buffer[3]];
	send_buffer[1] = temp[recv_buffer[3]+1];
	send_buffer[2] = recv_buffer[2];
	send_buffer[3] = recv_buffer[3];

	MPI_Send(&send_buffer, 4, MPI_INT, 0, return_data_tag, MPI_COMM_WORLD);
}

int main (int argc, char *argv[]) {
	if (argc < 2) {
    printf("No arguments.\n");
    return 0;
  }

	int err, np, myid, i, j;

	int problemValue = atoi(argv[1]);
  int initalValue = atoi(argv[2]);
  int iterations = atoi(argv[3]);	

	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS)
	{
		printf("Erreur d'initialisation de MPI\n");
		return 1;
	}

	MPI_Comm_size (MPI_COMM_WORLD, &np);
	MPI_Comm_rank (MPI_COMM_WORLD, &myid);

	if (myid == 0) {
		struct timeval begin, end;
		gettimeofday(&begin, NULL);

		// Initialize the array with initial value provided from program arguments.
		int matrix[ROW][COLUMN];
		for (i = 0; i < ROW; i++) {
			for ( j = 0; j < COLUMN; j++) {
		    matrix[i][j] = initalValue;
		  }
		}

		switch (problemValue) {
			case 1:
				problemOneMaster(matrix, iterations, np);
				break;
			case 2:
				problemTwoMaster(matrix, iterations, np);
				break;
			default:
				printf("Invalid program choice. Please select 1 or 2.\n");
				break;
		}

		gettimeofday(&end, NULL);
		double time_spent = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
  	printf("Time spent to execute the program: %fs\n", time_spent);
	} else {
		switch (problemValue) {
			case 1:
				problemOneSlave();
				break;
			case 2:
				problemTwoSlave();
				break;
		}
	}

	MPI_Finalize();
	return 0;
}

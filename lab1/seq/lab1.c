#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define ROW 8
#define COLUMN 8

void problemOne(int (*matrix)[8][8], int iterations) {
  printf("Executing problem one\n");
  for (size_t k = 0; k <= iterations; k++) {
    // Execute calculations
    for (size_t i = 0; i < ROW; i++) {
      for (size_t j = 0; j < COLUMN; j++) {
        usleep(1000);
        (*matrix)[i][j] = (*matrix)[i][j] + (i+j) * k;
      }
    }
  }
}

void problemTwo(int (*matrix)[8][8], int iterations) {
  printf("Executing problem two\n");
  for (size_t k = 0; k <= iterations; k++) {
    // Execute calculations
    for (size_t i = 0; i < ROW; i++) {
      for (size_t j = 0; j < COLUMN; j++) {
        usleep(1000);
        if (j == 0) {
          (*matrix)[i][j] = (*matrix)[i][j] + (i * k);
        } else {
          (*matrix)[i][j] = (*matrix)[i][j] + (*matrix)[i][j-1] * k;
        }
      }
    }
  }
}

int main(int argc, char const *argv[]) {
  // Start clock
  // clock_t begin = clock();
  struct timeval begin, end;

  if (argc < 2) {
    printf("No arguments.\n");
    return 0;
  }
  gettimeofday(&begin, NULL);

  int problem, value, iterations;
  problem = atoi(argv[1]);
  value = atoi(argv[2]);
  iterations = atoi(argv[3]);

  // Initialize the array with initial value provided from program arguments.
  int matrix[ROW][COLUMN];
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      matrix[i][j] = value;
    }
  }

  // Execute right program base on program argument.
  switch (problem) {
    case 1:
      problemOne(&matrix, iterations);
      break;
    case 2:
      problemTwo(&matrix, iterations);
      break;
    default:
      printf("Invalid program choice. Please select 1 or 2.\n");
      break;
  }

  // Print the final results matrix.
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }

  // End clock and print time
  gettimeofday(&end, NULL);
  double time_spent = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
  printf("Time spent to execute the program: %fs\n", time_spent);

  return 0;
}

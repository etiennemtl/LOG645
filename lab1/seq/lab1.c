#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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
  clock_t begin = clock();

  if (argc < 2) {
    printf("No arguments.\n");
    return 0;
  }
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
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  float time = (((float)end-(float)begin)/ 1000000.0F)*1000;
  // double time_spent = end / CLOCKS_PER_SEC;
  printf("Time spent to execute the program: %fs\n", time_spent);
  printf("Sequential process finished the work in %f ms\n", time);
  return 0;
}

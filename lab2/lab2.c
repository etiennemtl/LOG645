#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <omp.h>

#define ROW 10
#define COLUMN 10

void problemOne(int valeurInitiale, int nombreIterations);
void problemTwo(int valeurInitiale, int nombreIterations);
void initMatrix(int (*matrix)[ROW][COLUMN], int value);

int main(int argc, char const *argv[]) {
  if (argc < 4) {
    printf("No arguments.\n");
    return 0;
  }

  int numeroProbleme = atoi(argv[1]);
  int valeurInitiale = atoi(argv[2]);
  int nombreIterations = atoi(argv[3]);

  // Execute right program base on program argument.
  switch (numeroProbleme) {
    case 1:
      printf("\n\n");
      problemOne(valeurInitiale, nombreIterations);
      break;
    case 2:
      printf("\n\n");
      problemTwo(valeurInitiale, nombreIterations);
      break;
    default:
      printf("\n\n");
      printf("Invalid program choice. Please select 1 or 2.\n");
      break;
  }

  return EXIT_SUCCESS;
}

void problemOne(int valeurInitiale, int nombreIterations)
{
  int i, j, k;
  int matrix[ROW][COLUMN];

  // init matrix with initial value
  initMatrix(&matrix, valeurInitiale);

  // execute operations
  for (k = 1; k <= nombreIterations; k++) {
    for (i = 0; i < ROW; i++) {
      for (j = 0; j < COLUMN; j++) {
        usleep(50000);
        matrix[i][j] = matrix[i][j] + i + j;
      }
    }
  }

  // print the final results matrix.
  printf("%s\n", "Matrice séquentielle");
  printf("---------------------------------------------------------------------------\n");
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  printf("--------------------------------------------------------------------------\n");
  printf("\n");

  // init matrix with initial value
  initMatrix(&matrix, valeurInitiale);

  // execute operation in parallel
  #pragma omp parallel for private(j, k)
  for (i = 0; i < ROW; i++) {
    for (k = 1; k <= nombreIterations; k++) {
      for (j = 0; j < COLUMN; j++) {
        usleep(50000);
        matrix[i][j] = matrix[i][j] + i + j;
      }
    }
  }

  // Print the final results matrix.
  printf("%s\n", "Matrice parallèle");
  printf("---------------------------------------------------------------------------\n");
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  printf("---------------------------------------------------------------------------\n");

}

void problemTwo(int valeurInitiale, int nombreIterations)
{
  int i, j, k;
  int matrix[ROW][COLUMN];

  // init matrix with initial value
  initMatrix(&matrix, valeurInitiale);

  // execute operations
  for (k = 1; k <= nombreIterations; k++) {
    for (j = COLUMN-1; j >= 0; j--) {
      for (i = 0; i < ROW; i++) {
        usleep(50000);
        if (j == COLUMN-1) {
          matrix[i][j] = matrix[i][j] + i;
        } else {
          matrix[i][j] = matrix[i][j] + matrix[i][j+1];
        }
      }
    }
  }

  // print the final results matrix.
  printf("%s\n", "Matrice séquentielle");
  printf("---------------------------------------------------------------------------\n");
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  printf("--------------------------------------------------------------------------\n");
  printf("\n");

  // init matrix with initial value
  initMatrix(&matrix, valeurInitiale);

  // execute operation in parallel
  #pragma omp parallel for private(j, k)
  for (i = 0; i < ROW; i++) {
    for (k = 1; k <= nombreIterations; k++) {
      for (j = COLUMN-1; j >= 0; j--) {
        usleep(50000);
        if (j == COLUMN-1) {
          matrix[i][j] = matrix[i][j] + i;
        } else {
          matrix[i][j] = matrix[i][j] + matrix[i][j+1];
        }
      }
    }
  }

  // Print the final results matrix.
  printf("%s\n", "Matrice parallèle");
  printf("---------------------------------------------------------------------------\n");
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  printf("---------------------------------------------------------------------------\n");
}

void initMatrix(int (*matrix)[ROW][COLUMN], int value) 
{
  for (int i = 0; i < ROW; ++i)
  {
    for (int j = 0; j < COLUMN; ++j)
    {
      (*matrix)[i][j] = value;
    }
  }
}

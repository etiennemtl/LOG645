#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define ROW 10
#define COLUMN 10

void problemOne(int valeurInitiale, int nombreIterations);
void problemTwo(int valeurInitiale, int nombreIterations);

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
      problemOne(valeurInitiale, nombreIterations);
      break;
    case 2:
      // problemTwo(valeurInitiale, nombreIterations);
      break;
    default:
      printf("Invalid program choice. Please select 1 or 2.\n");
      break;
  }

  return EXIT_SUCCESS;
}

void problemOne(int valeurInitiale, int nombreIterations) {
  // Start clock
  struct timeval begin, end;
  gettimeofday(&begin, NULL);
  int matrix[ROW][COLUMN];

  int i, j, k;
  for (k = 0; k <= nombreIterations; k++) {
    for (i = 0; i < ROW; i++) {
      for (j = 0; j < COLUMN; j++) {
        usleep(50000);
        if (k == 0) {
          matrix[i][j] = valeurInitiale;
        } else {
          matrix[i][j] = matrix[i][j] + i + j;
        }
      }
    }
  }

  // Print the final results matrix.
  printf("Matrice sequentielle\n");
  printf("---------------------------------------------------------------------\n");
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }

  // End clock and print time
  gettimeofday(&end, NULL);
  double time_spent = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
  printf("Sequential time spent: %fs\n", time_spent);
  printf("---------------------------------------------------------------------\n");

  struct timeval proc_begin, proc_end;
  double total_proc_time;
  int nprocs;

  for (i = 0; i < ROW; i++) {
    for (j = 0; j < COLUMN; j++) {
      matrix[i][j] = valeurInitiale;
    }
    printf("\n");
  }

  #pragma omp parallel private(proc_begin, proc_end) default(shared)
  {
    nprocs = omp_get_num_threads();
    gettimeofday(&proc_begin, NULL);
    #pragma omp for collapse(3)
    for (k = 1; k <= nombreIterations; k++) {
      for (i = 0; i < ROW; i++) {
        for (j = 0; j < COLUMN; j++) {
          usleep(50000);
          // if (k == 0) {
          //   #pragma omp atomic write
          //   matrix[i][j] = valeurInitiale;
          // } else {
          //   #pragma omp atomic
          //   matrix[i][j] = matrix[i][j] + i + j;
          // }
          #pragma omp atomic
          matrix[i][j] = matrix[i][j] + i + j;
        }
      }
    }
    gettimeofday(&proc_end, NULL);
    #pragma omp atomic
    total_proc_time += (proc_end.tv_sec - proc_begin.tv_sec) + ((proc_end.tv_usec - proc_begin.tv_usec)/1000000.0);
  }

  printf("Matrice parallele\n");
  printf("---------------------------------------------------------------------\n");
  // Print the final results matrix.
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COLUMN; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }

  double tp = total_proc_time / nprocs;
  printf("Average time spent by process: %f\n", tp);
  printf("Acceleration: %f\n", time_spent/tp);
  printf("Efficiency: %f\n", time_spent/(tp*nprocs));
  printf("---------------------------------------------------------------------\n");

}

// void problemTwo(int valeurInitiale, int nombreIterations) {
//   // Start clock
//   struct timeval begin, end;
//   gettimeofday(&begin, NULL);

//   int i, j, k;
//   for (k = 1; k <= nombreIterations; k++) {
//     for (i = 0; i < ROW; i++) {
//       for (j = COLUMN-1; j >= 0; j--) {
//         usleep(50000);
//         if (j == COLUMN-1) {
//           #pragma omp atomic
//           (*matrix)[i][j] = (*matrix)[i][j] + i;
//         } else {
//           #pragma omp atomic
//           (*matrix)[i][j] = (*matrix)[i][j] + (*matrix)[i][j+1];
//         }
//       }
//     }
//   }

//   // End clock and print time
//   gettimeofday(&end, NULL);
//   double time_spent = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
//   printf("Sequential time spent: %fs\n", time_spent);

//   struct timeval proc_begin, proc_end;
//   double total_proc_time;
//   int nprocs;


//   #pragma omp parallel private(proc_begin, proc_end) default(shared)
//   {
//     nprocs = omp_get_num_threads();
//     gettimeofday(&proc_begin, NULL);
//     #pragma omp for
//     for (k = 1; k <= nombreIterations; k++) {
//       for (i = 0; i < ROW; i++) {
//         for (j = COLUMN-1; j >= 0; j--) {
//           usleep(50000);
//           if (j == COLUMN-1) {
//             #pragma omp atomic
//             (*matrix)[i][j] = (*matrix)[i][j] + i;
//           } else {
//             #pragma omp atomic
//             (*matrix)[i][j] = (*matrix)[i][j] + (*matrix)[i][j+1];
//           }
//         }
//       }
//     }
//     gettimeofday(&proc_end, NULL);
//     #pragma omp atomic
//     total_proc_time += (proc_end.tv_sec - proc_begin.tv_sec) + ((proc_end.tv_usec - proc_begin.tv_usec)/1000000.0);
//   }

//   double tp = total_proc_time / nprocs;
//   printf("Average time spent by process: %f\n", tp);
//   printf("Acceleration: %f\n", time_spent/tp);
//   printf("Efficiency: %f\n", time_spent/(tp*nprocs));
// }

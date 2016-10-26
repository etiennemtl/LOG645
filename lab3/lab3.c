#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mpi.h"

void printMatrix(double*** matrix, int n, int m, int np);

int main(int argc, char* argv[])
{
    if (argc < 6)
    {
        printf("Il manque des arguments.\n");
        return EXIT_FAILURE;
    }
    
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int np = atoi(argv[3]);
    double td = atof(argv[4]);
    double h = atof(argv[5]);
//    int nbproc = atoi(argv[6]);
    
    int err, size, id;
    MPI_Status status;
    
    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS)
    {
        printf("Erreur d'initialisation de MPI\n");
        return EXIT_FAILURE;
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (id == 0) {
        int i, j, k;
        
        double*** matrix = (double ***) malloc(n * sizeof(double**));
        for (i = 0; i < m; i++) {
            matrix[i] = (double **) malloc(m * sizeof(double*));
            for (j = 0; j < n; j++) {
                matrix[i][j] = (double *) malloc(np * sizeof(double));
            }
        }
        
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                matrix[i][j][0] = (double) i*(m-i-1) * j*(n-j-1);
            }
        }
        
        double a = 1.0 - (4.0*td)/(h*h);
        double b = td/(h*h);
        
        
        for (i = 1; i < m - 1 ; i++) {
            for (j = 1; j < n - 1; j++) {
                for (k = 0; k < np; k++) {
                    matrix[i][j][k+1] = (double) ( ( a * matrix[i][j][k] ) + ( b * (matrix[i-1][j][k] + matrix[i+1][j][k] + matrix[i][j-1][k] + matrix[i][j+1][k]) ) );
                }
            }
        }
        
        printMatrix(matrix, n, m, np);
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void printMatrix(double*** matrix, int n, int m, int np)
{
    int i, j;
    for (i = 0; i < m ; i++) {
        for (j = 0; j < n; j++) {
            printf("%d\t",(int) matrix[i][j][np]);
        }
        printf("\n");
    }
}
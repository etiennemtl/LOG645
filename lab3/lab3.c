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
        
        double*** matrix = (double ***) malloc((np+1) * sizeof(double**));
        for (k = 0; k <= np; k++) {
            matrix[k] = (double **) malloc(m * sizeof(double*));
            for (i = 0; i < m; i++) {
                matrix[k][i] = (double *) malloc(n * sizeof(double));
            }
        }
        
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                matrix[0][i][j] = (double) i*(m-i-1) * j*(n-j-1);
            }
        }
        
        double a = 1.0 - (4.0*td)/(h*h);
        double b = td/(h*h);
        
        for (k = 1; k <= np; k++) {
            for (i = 1; i < m - 1 ; i++) {
                for (j = 1; j < n - 1; j++) {
                    matrix[k][i][j] = (double) ( ( a * matrix[k-1][i][j] ) + ( b * (matrix[k-1][i-1][j] + matrix[k-1][i+1][j] + matrix[k-1][i][j-1] + matrix[k-1][i][j+1]) ) );
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
            printf("%d\t",(int) matrix[np][i][j]);
        }
        printf("\n");
        printf("\n");
    }
}
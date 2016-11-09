#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mpi.h"

void executeseq(int n, int m, int np, double td, double h)
{
    int i, j, k;
    
    // Creation d'un conteneur de matrice dynamique
    double*** matrix = (double ***) malloc((np+1) * sizeof(double**));
    for (k = 0; k <= np; k++) {
        matrix[k] = (double **) malloc(n * sizeof(double*));
        for (i = 0; i < n; i++) {
            matrix[k][i] = (double *) malloc(m * sizeof(double));
        }
    }
    
    // Initialisation de la matrice 0
    for (k = 0; k <= np; k++) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                matrix[k][i][j] = 0;
            }
        }
    }
    
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            matrix[0][i][j] = (double) i*(n-i-1) * j*(m-j-1);
        }
    }
    
    // Calcul des valeurs statiques
    double a = 1.0 - (4.0*td)/(h*h);
    double b = td/(h*h);
    
    // Execution de l'alogorithme pour np iterations
    for (k = 1; k <= np; k++) {
        for (i = 1; i < n - 1 ; i++) {
            for (j = 1; j < m - 1; j++) {
                matrix[k][i][j] = (double) ( ( a * matrix[k-1][i][j] ) + ( b * (matrix[k-1][i-1][j] + matrix[k-1][i+1][j] + matrix[k-1][i][j-1] + matrix[k-1][i][j+1]) ) );
            }
        }
    }
    
    // Affichage de la matrice
    for (i = 0; i < n ; i++) {
        for (j = 0; j < m; j++) {
            printf("%d\t",(int) matrix[np][i][j]);
        }
        printf("\n");
        printf("\n");
    }
}

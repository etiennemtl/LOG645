#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mpi.h"
#include "seq.h"
#include "par.h"

#define MASTER 0
#define NO_NEIGH 0
#define BEGIN 1000
#define TRANSMIT_LEFT 1001
#define TRANSMIT_RIGHT 1002
#define END 1003

int main(int argc, char* argv[])
{
    if (argc < 6)
    {
        printf("Il manque des arguments.\n");
        return EXIT_FAILURE;
    }
    
    // Recuperation des valeurs passe en arguments
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int np = atoi(argv[3]);
    double td = atof(argv[4]);
    double h = atof(argv[5]);
    
    int err, size, id;
    int i, j, k;
    double a = 1.0 - (4.0*td)/(h*h);
    double b = td/(h*h);
    MPI_Status status;
    
    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS)
    {
        printf("Erreur d'initialisation de MPI\n");
        return EXIT_FAILURE;
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (id == MASTER) {
        /************************************
         *       Programme sequentiel       *
         ************************************/
        executeseq(n, m, np, td, h);
        
        
        /************************************
         *       Programme parallèle        *
         ************************************/
        
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
        
        int ishorizontal = n > m ? 1 : 0;
        int tasknumber = ishorizontal == 1 ? n : m;
        
        int numrows = tasknumber/(size-1);
        int numrowsextra = tasknumber%(size-1);
        int offset = 0;
        int left, right, rows;
        
        //Send to workers
        for(i = 1; i < size; i++) {
            rows = (i <= numrowsextra) ? numrows+1 : numrows;
            if (i == 1) {
                left = NO_NEIGH;
            } else {
                left = i - 1;
            }
            if (i == size-1) {
                right = NO_NEIGH;
            } else {
                right = i + 1;
            }
            
            MPI_Send(&rows, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
            MPI_Send(&offset, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
            MPI_Send(&left, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
            MPI_Send(&right, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
            MPI_Send(&ishorizontal, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
            if (ishorizontal == 1) {
                MPI_Send(&matrix[0][offset][0], rows*m, MPI_DOUBLE, i, BEGIN, MPI_COMM_WORLD);
            } else {
                MPI_Send(&matrix[0][0][offset], rows*n, MPI_DOUBLE, i, BEGIN, MPI_COMM_WORLD);
            }
            
            offset = offset + rows;
        }
        
        // Receive from workers
        for (i = 1; i < size; i++) {
            MPI_Recv(&offset, 1, MPI_INT, i, END, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, i, END, MPI_COMM_WORLD, &status);
            if (ishorizontal == 1) {
                MPI_Recv(&matrix[np][offset][0], rows*m, MPI_DOUBLE, i, END, MPI_COMM_WORLD, &status);
            } else {
                MPI_Recv(&matrix[np][0][offset], rows*n, MPI_DOUBLE, i, END, MPI_COMM_WORLD, &status);
            }
        }
        
        // Print matrix
        // Affichage de la matrice
        for (i = 0; i < n ; i++) {
            for (j = 0; j < m; j++) {
                printf("%d\t",(int) matrix[np][i][j]);
            }
            printf("\n");
            printf("\n");
        }
        
    } else {
        int left, right, rows, offset, begin, end;
        int ishorizontal;
        
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
                    matrix[k][i][j] = 0.0;
                }
            }
        }
        
        MPI_Recv(&rows, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        MPI_Recv(&offset, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        MPI_Recv(&left, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        MPI_Recv(&right, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        MPI_Recv(&ishorizontal, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        if (ishorizontal == 1) {
            MPI_Recv(&matrix[0][offset][0], rows*m, MPI_DOUBLE, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(&matrix[0][0][offset], rows*n, MPI_DOUBLE, MASTER, BEGIN, MPI_COMM_WORLD, &status);
        }
        
        printf("I am process %d and I have %d %d %d %d %d %lf\n", id,rows, offset, left, right, ishorizontal, ishorizontal == 1 ? matrix[0][offset][0] : matrix[0][0][offset]);
        
        begin = offset;
        end = offset + rows - 1;
        if (begin == 0) {
            begin = 1;
        }
        if (ishorizontal == 1 ? (end+rows == m) : (end+rows == n)) {
            end--;
        }
        
        int jiterator = ishorizontal == 1 ? m : n;
        
        for (k = 1; k<=np ; k++) {
            if (left != NO_NEIGH) {
                if (ishorizontal == 1) {
                    MPI_Send(&matrix[k][offset][0], 1, MPI_DOUBLE, left, TRANSMIT_LEFT, MPI_COMM_WORLD);
                    MPI_Recv(&matrix[k][offset-1][0], 1, MPI_DOUBLE, left, TRANSMIT_RIGHT, MPI_COMM_WORLD, &status);
                } else {
                    MPI_Send(&matrix[k][0][offset], 1, MPI_DOUBLE, left, TRANSMIT_LEFT, MPI_COMM_WORLD);
                    MPI_Recv(&matrix[k][0][offset-1], 1, MPI_DOUBLE, left, TRANSMIT_RIGHT, MPI_COMM_WORLD, &status);
                }
            }
            if (right != NO_NEIGH) {
                if (ishorizontal == 1) {
                    MPI_Send(&matrix[k][offset+rows-1][0], 1, MPI_DOUBLE, right, TRANSMIT_RIGHT, MPI_COMM_WORLD);
                    MPI_Recv(&matrix[k][offset+rows][0], 1, MPI_DOUBLE, right, TRANSMIT_LEFT, MPI_COMM_WORLD, &status);
                } else {
                    MPI_Send(&matrix[k][0][offset+rows-1], 1, MPI_DOUBLE, right, TRANSMIT_RIGHT, MPI_COMM_WORLD);
                    MPI_Recv(&matrix[k][0][offset+rows], 1, MPI_DOUBLE, right, TRANSMIT_LEFT, MPI_COMM_WORLD, &status);
                }
            }
            
  
            for (i = 1; i <= end - 1 ; i++) {
                for (j = 1; j < jiterator - 1; j++) {
                    matrix[k][i][j] = (double) ( ( a * matrix[k-1][i][j] ) + ( b * (matrix[k-1][i-1][j] + matrix[k-1][i+1][j] + matrix[k-1][i][j-1] + matrix[k-1][i][j+1]) ) );
                }
            }
        }
        
        printf("I am process %d and I have %lf\n", id, ishorizontal ==1 ? matrix[np][offset][0] : matrix[np][0][offset]);
        
        MPI_Send(&offset, 1, MPI_INT, MASTER, END, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, END, MPI_COMM_WORLD);
        if (ishorizontal == 1) {
            MPI_Send(&matrix[np][offset][0], rows*m, MPI_DOUBLE, MASTER, END, MPI_COMM_WORLD);
        } else {
            MPI_Send(&matrix[np][0][offset], rows*n, MPI_DOUBLE, MASTER, END, MPI_COMM_WORLD);
        }
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}

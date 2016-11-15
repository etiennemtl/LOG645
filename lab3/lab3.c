#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <mpi.h>

// Constants
#define MASTER 0
#define NO_NEIGH -1
#define BEGIN_TAG 100
#define TRANSMIT_LEFT_TAG 101
#define TRANSMIT_RIGHT_TAG 102
#define END_TAG 103

// Global variables
int size, id, n, m, np, procs, matrixsize;
double td, h;

// Functions declarations
void executeseq(int n, int m, int np, double matrix[np][n][m]);
void executepar(int n, int m, int np, double matrix[np][n][m]);
void initmatrix(int rows, int columns, double matrix[rows][columns]);
void zeromatrix(int rows, int columns, int iterations, double matrix[iterations][rows][columns]);
void printmatrix(int rows, int columns, double matrix[rows][columns]);
double getcurrenttime();

int main(int argc, char* argv[])
{
    double start, end, timeseq, timepar;
    MPI_Init(&argc, &argv);
    
    // Validate parameters
    if (argc >= 6)
    {
        n     = atoi(argv[1]);
        m     = atoi(argv[2]);
        np    = atoi(argv[3]);
        td    = atof(argv[4]);
        h     = atof(argv[5]);
        procs = atoi (argv[6]);
        matrixsize = np + 1;
    }
    else
    {
        printf("Il manque des arguments, quitting.\n");
        return EXIT_FAILURE;
    }
    
    // Retrieve the actual rank (id) and the numbers of procs
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (id == MASTER) {
        printf("n=%d, m=%d, np=%d, td=%.5f, h=%.5f\n", n, m, np, td, h);
        double matrix[matrixsize][n][m];
        zeromatrix(n, m, matrixsize, matrix);
        
        printf("Version sequentiel\n");
        // Initialize la matrice
        initmatrix(n, m, matrix[0]);
        printf("initiale\n====\n");
        // Affiche la matrice initial
        printmatrix(n, m, matrix[0]);
        // Demarrer le timer
        start = getcurrenttime();
        // Traitement  sequentiel
        executeseq(n, m, np, matrix);
        // Arreter le timer
        end = getcurrenttime();
        timeseq = end - start;
        // Affiche la matrice initial
        printf("final\n=====\n");
        printmatrix(n, m, matrix[np]);
        
        
        printf("\n\n");
        printf("Version parallele\n");
        // Initialize la matrice
        initmatrix(n, m, matrix[0]);
        printf("initiale\n====\n");
        // Affiche la matrice initial
        printmatrix(n, m, matrix[0]);
        // Demarrer le timer
        start = getcurrenttime();
        // Traitement  sequentiel
        executepar(n, m, np, matrix);
        // Arreter le timer
        end = getcurrenttime();
        timepar = end - start;
        // Affiche la matrice final
        printf("final\n=====\n");
        printmatrix(n, m, matrix[np-1]);
        
        printf("Temps d'execution sequentiel: %f\n", timeseq);
        printf("Temps d'execution parallele: %f\n", timeseq);
        printf("Accélération : %f\n", timeseq/timepar);
        printf("Efficacité: %f\n", (timeseq/timeseq)/size);
    }
    else
    {
        double matrix[matrixsize][n][m];
        zeromatrix(n, m, matrixsize, matrix);
        executepar(n, m, np, matrix);
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void executeseq(int n, int m, int np, double matrix[np][n][m])
{
    int i, j, k;
    
    for (k = 1; k <= np; k++)
    {
        for (i = 1; i < n - 1; i++)
        {
            for (j = 1; j < m - 1; j++)
            {
                matrix[k][i][j] = (1.0 - 4*td / h*h) * matrix[k-1][i][j] + (td/h*h) * (matrix[k-1][i - 1][j] + matrix[k-1][i + 1][j] + matrix[k-1][i][j - 1] + matrix[k-1][i][j + 1]);
            }
        }
    }
}

void executepar(int n, int m, int np, double matrix[np][n][m])
{
    int i, j, k;
    int workers, tasks, extra, offset, left, right, rows;
    MPI_Status status;
    
    if (id == MASTER)
    {
//        int ishorizontal = n >= m ? 1 : 0;
        workers = size - 1;
        tasks   = n / workers;
        extra   = n % workers;
        offset  = 0;
        
//        int tasknumber = ishorizontal == 1 ? n : m;
        
//        int numrows = tasknumber/(size-1);
//        int numrowsextra = tasknumber%(size-1);
        
        //Send to workers
        for(i = 1; i < size; i++)
        {
            rows = (i <= extra) ? tasks + 1 : tasks;
            left = (i == 1) ? NO_NEIGH : i - 1;
            right = (i == workers) ? NO_NEIGH : i + 1;
            
            MPI_Send(&rows, 1, MPI_INT, i, BEGIN_TAG, MPI_COMM_WORLD);
            MPI_Send(&offset, 1, MPI_INT, i, BEGIN_TAG, MPI_COMM_WORLD);
            MPI_Send(&left, 1, MPI_INT, i, BEGIN_TAG, MPI_COMM_WORLD);
            MPI_Send(&right, 1, MPI_INT, i, BEGIN_TAG, MPI_COMM_WORLD);
//            MPI_Send(&ishorizontal, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
            MPI_Send(&matrix[0][offset][0], rows*m, MPI_DOUBLE, i, BEGIN_TAG, MPI_COMM_WORLD);
//            if (ishorizontal == 1) {
//                MPI_Send(&matrix[0][offset][0], rows*m, MPI_DOUBLE, i, BEGIN, MPI_COMM_WORLD);
//            } else {
//                MPI_Send(&matrix[0][0][offset], rows*n, MPI_DOUBLE, i, BEGIN, MPI_COMM_WORLD);
//            }
            
            offset = offset + rows;
        }
        
        // Receive from workers
        for (i = 1; i < size; i++)
        {
            MPI_Recv(&offset, 1, MPI_INT, i, END_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, i, END_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&matrix[np][offset][0], rows*m, MPI_DOUBLE, i, END_TAG, MPI_COMM_WORLD, &status);
//            if (ishorizontal == 1) {
//                MPI_Recv(&matrix[np][offset][0], rows*m, MPI_DOUBLE, i, END, MPI_COMM_WORLD, &status);
//            } else {
//                MPI_Recv(&matrix[np][0][offset], rows*n, MPI_DOUBLE, i, END, MPI_COMM_WORLD, &status);
//            }
        }
    }
    else
    {
        int left, right, rows, offset, begin, end;
//        int ishorizontal;
        
        MPI_Recv(&rows, 1, MPI_INT, MASTER, BEGIN_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&offset, 1, MPI_INT, MASTER, BEGIN_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&left, 1, MPI_INT, MASTER, BEGIN_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&right, 1, MPI_INT, MASTER, BEGIN_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrix[0][offset][0], rows*m, MPI_DOUBLE, MASTER, BEGIN_TAG, MPI_COMM_WORLD, &status);
//        MPI_Recv(&ishorizontal, 1, MPI_INT, MASTER, BEGIN_TAG, MPI_COMM_WORLD, &status);
        
        begin = offset;
        end = offset + rows - 1;
        if (offset == 0) {
            begin = 1;
        }
        if (offset + rows == m) {
            end--;
        }
//        if (ishorizontal == 1 ? (offset+rows == n) : (offset+rows == m)) {
//            end--;
//        }
    
//        int jiterator = ishorizontal == 1 ? m : n;
        
        for (k = 1; k <= np ; k++)
        {
            // Left neighbor
            if (left != NO_NEIGH)
            {
                MPI_Send(&matrix[k-1][offset][0], m, MPI_DOUBLE, left, TRANSMIT_RIGHT_TAG, MPI_COMM_WORLD);
                MPI_Recv(&matrix[k-1][offset - 1][0], m, MPI_DOUBLE, left, TRANSMIT_LEFT_TAG, MPI_COMM_WORLD, &status);
            }
            
            // Right neighbor
            if (right != NO_NEIGH)
            {
                MPI_Send(&matrix[k-1][offset + rows - 1][0], m, MPI_DOUBLE, right, TRANSMIT_LEFT_TAG, MPI_COMM_WORLD);
                MPI_Recv(&matrix[k-1][offset + rows][0], m, MPI_DOUBLE, right, TRANSMIT_RIGHT_TAG, MPI_COMM_WORLD, &status);
            }
            
            for (i = begin; i <= end; i++)
            {
                for (j = 1; j < m - 1; j++)
                {
                    matrix[k][i][j] = (1.0 - 4*td / h*h) * matrix[k-1][i][j] + (td/h*h) * (matrix[k-1][i - 1][j] + matrix[k-1][i + 1][j] + matrix[k-1][i][j - 1] + matrix[k-1][i][j + 1]);
                }
            }
            
//            if (left != NO_NEIGH) {
//                if (ishorizontal == 1) {
//                    MPI_Send(&matrix[k-1][offset][0], m, MPI_DOUBLE, left, TRANSMIT_LEFT, MPI_COMM_WORLD);
//                    MPI_Recv(&matrix[k-1][offset-1][0], m, MPI_DOUBLE, left, TRANSMIT_RIGHT, MPI_COMM_WORLD, &status);
//                } else {
//                    MPI_Send(&matrix[k-1][0][offset], n, MPI_DOUBLE, left, TRANSMIT_LEFT, MPI_COMM_WORLD);
//                    MPI_Recv(&matrix[k-1][0][offset-1], n, MPI_DOUBLE, left, TRANSMIT_RIGHT, MPI_COMM_WORLD, &status);
//                }
//            }
//            if (right != NO_NEIGH) {
//                if (ishorizontal == 1) {
//                    MPI_Send(&matrix[k-1][offset+rows-1][0], m, MPI_DOUBLE, right, TRANSMIT_RIGHT, MPI_COMM_WORLD);
//                    MPI_Recv(&matrix[k-1][offset+rows][0], m, MPI_DOUBLE, right, TRANSMIT_LEFT, MPI_COMM_WORLD, &status);
//                } else {
//                    MPI_Send(&matrix[k-1][0][offset+rows-1], n, MPI_DOUBLE, right, TRANSMIT_RIGHT, MPI_COMM_WORLD);
//                    MPI_Recv(&matrix[k-1][0][offset+rows], n, MPI_DOUBLE, right, TRANSMIT_LEFT, MPI_COMM_WORLD, &status);
//                }
//            }
            
//            if (ishorizontal == 1) {
//                for (i = begin; i <= end; i++) {
//                    for (j = 1; j <= jiterator - 2; j++) {
//                        matrix[k][i][j] = (1.0 - 4*td / h*h) * matrix[k-1][i][j] + (td/h*h) * (matrix[k-1][i - 1][j] + matrix[k-1][i + 1][j] + matrix[k-1][i][j - 1] + matrix[k-1][i][j + 1]);
//                    }
//                }
//            } else {
//                for (i = 1; i < jiterator - 1; i++) {
//                    for (j = begin; j <= end - 1; j++) {
//                        matrix[k][i][j] = (1.0 - 4*td / h*h) * matrix[k-1][i][j] + (td/h*h) * (matrix[k-1][i - 1][j] + matrix[k-1][i + 1][j] + matrix[k-1][i][j - 1] + matrix[k-1][i][j + 1]);
//                    }
//                }
//            }
        }
        
        MPI_Send(&offset, 1, MPI_INT, MASTER, END_TAG, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, END_TAG, MPI_COMM_WORLD);
        MPI_Send(&matrix[np][offset][0], rows*m, MPI_DOUBLE, MASTER, END_TAG, MPI_COMM_WORLD);
//        if (ishorizontal == 1) {
//            MPI_Send(&matrix[np][offset][0], rows*m, MPI_DOUBLE, MASTER, END, MPI_COMM_WORLD);
//        } else {
//            MPI_Send(&matrix[np][0][offset], rows*n, MPI_DOUBLE, MASTER, END, MPI_COMM_WORLD);
//        }
    }
}

void initmatrix(int rows, int columns, double matrix[rows][columns])
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < columns; j++)
        {
            matrix[i][j] = i * j * (rows - i - 1) * (columns - j - 1);
        }
    }
}

void zeromatrix(int rows, int columns, int iterations, double matrix[iterations][rows][columns])
{
    int i, j, k;
    for (k = 0; k < iterations; k++)
    {
        for (i = 0; i < rows; i++)
        {
            for (j = 0; j < columns; j++)
            {
                matrix[k][i][j] = 0.0;
            }
        }
    }
    
}

void printmatrix(int rows, int columns, double matrix[rows][columns])
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < columns; j++)
        {
            printf("%5.2f\t", matrix[i][j]);
        }
        printf("\n");
    }
}

double getcurrenttime()
{
    struct timeval tp;
    gettimeofday (&tp, NULL);
    return (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
}

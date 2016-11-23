// lab4.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <cstdio>

int main(int argc, char* argv[])
{
	int m, n, np, i, j, k;
	double td, h;
	if (argc == 6)
	{
		m = atoi(argv[1]);
		n = atoi(argv[2]);
		np = atoi(argv[3]);
		td = atof(argv[4]);
		h = atof(argv[5]);
	}
	else
	{
		printf("Nombre d'arguments invalide. Sutting down...\n");
		return EXIT_FAILURE;
	}

	double*** matrix = (double ***)malloc(2 * sizeof(double**));
	for (k = 0; k < 2; k++) {
		matrix[k] = (double **)malloc(n * sizeof(double*));
		for (i = 0; i < n; i++) {
			matrix[k][i] = (double *)malloc(m * sizeof(double));
		}
	}

	for (k = 0; k < 2; k++) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < m; j++) {
				matrix[k][i][j] = 0.0;
			}
		}
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			matrix[0][i][j] = i * j * (n - i - 1) * (m - j - 1);
		}
	}

	int current = 1;

	for (k = 1; k <= np; k++)
	{
		for (i = 1; i < n - 1; i++)
		{
			for (j = 1; j < m - 1; j++)
			{
				matrix[current][i][j] = (1.0 - 4 * td / h*h) * matrix[1 - current][i][j] +
					(td / h*h) * (matrix[1 - current][i - 1][j] +
						matrix[1 - current][i + 1][j] +
						matrix[1 - current][i][j - 1] +
						matrix[1 - current][i][j + 1]);
			}
		}
		current = 1 - current;
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			printf("%5.2f\t", matrix[1 - current][i][j]);
		}
		printf("\n");
	}

	std::getchar();
    return EXIT_SUCCESS;
}

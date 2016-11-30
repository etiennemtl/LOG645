__kernel void HeatTransfer(__global double *intialmatrix, __global double *finalmatrix, int n, int m, int np, double td, double h)
{
	int id = get_global_id(0);
	int i, j;
	if (id == 0)
	{
		printf("matrice initiale\n");
		for (i = 0; i < n; i++) {
			for (j = 0; j < m; j++) {
				printf("%5.2f\t", intialmatrix[i*m+j]);
			}
			printf("\n");
		}
	}

	i = (int)(id / n);
	j = (id%n);

	if (i == 0 || i == m-1 || j == 0 || j == n-1) 
	{
		finalmatrix[i*m+j] = 0.00;
	}
	else 
	{
		double value = intialmatrix[i * m + j];
		double left = intialmatrix[i * m + (j - 1)];
		double right = intialmatrix[i * m + (j + 1)];
		double top = intialmatrix[(i + 1) * m + j];
		double bottom = intialmatrix[(i - 1) * m + j];

		finalmatrix[i*m+j] = (1 - (4 * td) / (h*h))*value + (td / (h*h))*(left + right + top + bottom);
	}
}
__kernel void HeatTransfer(__global double *intialmatrix, __global double *finalmatrix, int n, int m, int np, double td, double h)
{
	int id = get_global_id(0);
	int i = (int)(id / m);
	int j = (id%m);

	if (i == 0 || i == n-1 || j == 0 || j == m-1) 
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
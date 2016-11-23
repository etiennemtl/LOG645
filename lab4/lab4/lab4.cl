__kernel void HeatTransfer(__global double *intialmatrix, __global double *finalmatrix, int n, int m, int np, double td, double h)
{
	int id = get_global_id(0);
	printf("%d", id);
}
// lab4.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <cstdio>
#include <CL\cl.h>
#include <vector>

char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength);

int main(int argc, char* argv[])
{
	typedef std::vector< std::vector<double> > matrix;
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

	matrix seqmatrix[2] = { matrix(n, std::vector<double>(m)), matrix(n, std::vector<double>(m)) };

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			seqmatrix[0][i][j] = i * j * (n - i - 1) * (m - j - 1);
		}
	}

	int current = 1;

	for (k = 1; k <= np; k++)
	{
		for (i = 1; i < n - 1; i++)
		{
			for (j = 1; j < m - 1; j++)
			{
				seqmatrix[current][i][j] = (1.0 - 4 * td / h*h) * seqmatrix[1 - current][i][j] +
					(td / h*h) * (seqmatrix[1 - current][i - 1][j] +
						seqmatrix[1 - current][i + 1][j] +
						seqmatrix[1 - current][i][j - 1] +
						seqmatrix[1 - current][i][j + 1]);
			}
		}
		current = 1 - current;
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			printf("%5.2f\t", seqmatrix[1 - current][i][j]);
		}
		printf("\n");
	}

	/**********************************************************************
	 *							PARA									  *
	 **********************************************************************/
	
	double *initialmatrix = (double *)malloc(n*m*sizeof(double));
	double *finalmatrix = (double *)malloc(n*m*sizeof(double));

	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem Initialmatrixmobj = NULL;
	cl_mem Finalmatrixmobj = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	size_t szFinalLength;
	char* strkernel = oclLoadProgSource("./lab4.cl", "", &szFinalLength);

	/* Get platform/device information */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* Create OpenCL Context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create command queue */
	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);

	Initialmatrixmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, n * m * sizeof(double), NULL, &ret);
	Finalmatrixmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, n * m * sizeof(double), NULL, &ret);

	/* Create kernel from source */
	program = clCreateProgramWithSource(context, 1, (const char **)&strkernel, (const size_t *)&szFinalLength, &ret);
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create task parallel OpenCL kernel */
	kernel = clCreateKernel(program, "HeatTransfer", &ret);

	/* Set OpenCL kernel arguments */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&Initialmatrixmobj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&Finalmatrixmobj);
	ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&n);
	ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&m);
	ret = clSetKernelArg(kernel, 4, sizeof(int), (void *)&np);
	ret = clSetKernelArg(kernel, 5, sizeof(double), (void *)&td);
	ret = clSetKernelArg(kernel, 6, sizeof(double), (void *)&h);

	/* Copy input data to memory buffer */
	ret = clEnqueueWriteBuffer(command_queue, Initialmatrixmobj, CL_TRUE, 0, n * m * sizeof(double), initialmatrix, 0, NULL, NULL);

	size_t global_item_size = 2;
	size_t local_item_size = 1;

	/* Execute OpenCL kernel as task parallel */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	/* Copy result to host */
	ret = clEnqueueReadBuffer(command_queue, Finalmatrixmobj, CL_TRUE, 0, n * m * sizeof(double), finalmatrix, 0, NULL, NULL);

	/* Display result */
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			printf("%5.2f\t", finalmatrix[i*j]);
		}
		printf("\n");
	}

	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(Initialmatrixmobj);
	ret = clReleaseMemObject(Finalmatrixmobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(strkernel);

	std::getchar();
    return EXIT_SUCCESS;
}

char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
	// locals
	FILE* pFileStream = NULL;
	size_t szSourceLength;
	// open the OpenCL source code file
	if (fopen_s(&pFileStream, cFilename, "rb") != 0)
	{
		return NULL;
	}
	size_t szPreambleLength = strlen(cPreamble);
	// get the length of the source code
	fseek(pFileStream, 0, SEEK_END);
	szSourceLength = ftell(pFileStream);
	fseek(pFileStream, 0, SEEK_SET);
	// allocate a buffer for the source code string and read it in
	char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
	memcpy(cSourceString, cPreamble, szPreambleLength);
	if (fread((cSourceString)+szPreambleLength, szSourceLength, 1, pFileStream) != 1)
	{
		fclose(pFileStream);
		free(cSourceString);
		return 0;
	}
	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFileStream);
	if (szFinalLength != 0)
	{
		*szFinalLength = szSourceLength + szPreambleLength;
	}
	cSourceString[szSourceLength + szPreambleLength] = '\0';
	return cSourceString;
}

#include <stdio.h>
#include <mpi.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void solveMatrix (int n,  long double  **a,  long double  *f,  long double  *x)
{
	double m;
	for (int i = 1; i < n; i++)
	{
		m = a[i][i - 1]/a[i-1][i-1];
		a[i][i] = a[i][i] - m*a[i-1][i];
		f[i] = f[i] - m*f[i-1];
	}

	x[n-1] = f[n-1]/a[n-1][n-1];

	for (int i = n - 2; i >= 0; i--)
    {
	    x[i]=(f[i]-a[i][i + 1]*x[i+1])/a[i][i];

    }
}


int main(int argc, char const *argv[])
{
    char* endptr = NULL;
    MPI_Status status;
    int rank = 0, size = 0;
    int n = strtol(argv[1], &endptr, 10);
    if(MPI_Init(&argc, &argv)){
        perror("Error in creating proc");
        return -1;
    }
    if(MPI_Comm_size(MPI_COMM_WORLD, &size)){
        perror("Error in receiving size");
        return -1;
    }
    if(MPI_Comm_rank(MPI_COMM_WORLD, &rank)){
        perror("Error in receiving rank");
        return -1;
    }
    double t1 = 0.0;
    if (rank == 0)
        t1 = MPI_Wtime();
    for(int i = rank; i < n; i+= size){
        long double step = (long double)(i / 10.0);
        ///make matrix
        int number_of_points =  (i + 1) * 4000;
        long double  h = ( long double )(1 / number_of_points);
        long double  **matrix = ( long double **)calloc(number_of_points, sizeof( long double  *));
        for(int k = 0; k < number_of_points; k++){
            matrix[k] = calloc(number_of_points, sizeof( long double ));
        }
        long double * y_res = ( long double *)calloc(number_of_points, sizeof( long double ));
        long double * res_arr = ( long double *)calloc(number_of_points, sizeof( long double ));
        res_arr[0] = 1;
        res_arr[number_of_points - 1] = step;
        matrix[0][0] = 1;
        matrix[number_of_points - 1][number_of_points - 1] = 1;
        for(int j = 1; j < number_of_points - 1; j++){
            res_arr[i] = 1;
            matrix[j][j - 1] = ( long double ) (number_of_points * number_of_points);
            matrix[j][j + 1] = ( long double ) (number_of_points * number_of_points);
            matrix[j][j] = -2.0* number_of_points* number_of_points  - h*h;
            // printf("%Lf", matrix[j][j] );
        }

        solveMatrix(number_of_points, matrix, res_arr, y_res);
        // printf("%Lf ",step );
        // printf("%d ", i);
        for(int  k  =0; k < number_of_points; k ++){
            // printf(" %Lf ", y_res[k]);
        }
        // printf("\n");

        for(int j = 0; j < number_of_points; j++){
            free(matrix[j]);
        }
        free(matrix);

    }

    if (rank == 0)
        printf("TIME  ==  %lf", MPI_Wtime() - t1);
    MPI_Finalize();
    return 0;
}

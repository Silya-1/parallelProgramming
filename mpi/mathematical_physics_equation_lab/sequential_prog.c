#include <stdio.h>
#include <mpi.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


double u_x (double x){
	return x;
}


double u_t (double t){
	return t;
}


double f_t_x(double t, double x){
	return x * t;
}

void count_step(double ** res, int i , int j, double tau, double h, double T_0, double a){
	res[i + 1][j + 1] = f_t_x(T_0 + i * tau, a + (j + 1) * h);
	res[i + 1][j + 1] +=  0.5 * tau * (res[i][j + 2]  - 2 * res[i][j + 1] + res[i][j]) / (h * h);
	res[i + 1][j + 1] -= (res[i][j + 2] - res[i][j]) / (2*h);
	res[i + 1][j + 1] *= tau;
	res[i + 1][j + 1] += res[i][j + 1];
}

int main(int argc, char const *argv[])
{
	//T_0 = 0 a = 0 b = 1 T_1 = 1
	printf("%d\n", -1 % 5);
	double a, b, T_1, T_0;
	a = 0;
	b = 1;
	T_1 = 1;
	T_0 = 0;
	char *endptr = NULL;
	double tau, h = 0;
	tau = strtof(argv[1], &endptr);
    if (errno == ERANGE){
        perror("Number is out of range");
        return -1;
    } else if (*endptr != '\0'){
        perror("Invalid number");
        return -1;
    } else if (tau < 0){
    	perror("Incorrect number");
        return -1;
    }

    h = strtof(argv[1], &endptr);
    if (errno == ERANGE){
        perror("Number is out of range");
        return -1;
    } else if (*endptr != '\0'){
        perror("Invalid number");
        return -1;
    } else if (tau < 0){
    	perror("Incorrect number");
        return -1;
    }

    long num_h =   ceil(((b - a) / h)) + 1;
    long num_tau = ceil((T_1 - T_0) / tau)+ 1;
    double **res;
	res = (double**)calloc(num_tau, sizeof(double *));

    for (int i = 0; i < num_tau; i++){
    	res[i] = (double*)calloc(num_h, sizeof(double *));
    }
    for (int i = 0; i < num_h; i++){
    	res[0][i] = u_x(a + i * h);
    }

    for(int i = 0; i < num_tau; i++){
    	res[i][0] = u_t(T_0 + i * h);
    }

	int i,j = 0;
    for (i = 0; i < (num_tau - 1); i++){
    	for (j = 0; j < (num_h - 2); j++){
    			count_step(res, i, j, tau, h, T_0, a);
    	}

    	res[i + 1][j + 1] = f_t_x(T_0 + i * tau, a + (j + 1) * h) - (res[i][j + 1] - res[i][j]) / h;
    	res[i + 1][j + 1] *= tau;
    	res[i + 1][j + 1] += res[i][j + 1];
    }

    for (int i = 0; i < num_tau; i++){
    	for (int j = 0; j < num_h; j++){
    		printf("%lf ", res[i][j]);
    	}
    	printf("\n");
    }

	return 0;
}

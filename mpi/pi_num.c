#include <stdio.h>
#include "mpi.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


double  pi_partial_amount(int k){
	if(k < 0){
		errno = EINVAL;
		return 0;
	}
	return (pow(-1, k) / pow(4, k)) * (2 / (double)(4 * k + 1) + 2 / (double)(4 * k + 2) + 1 / (double)(4 * k + 3));
}

int main(int argc, char* argv[]){
	if(argc != 2){
		perror("Not enough arguments");
		return -1;
	}
	int rank, size, message, count, i, num;
	double res = 0;
	char *endptr = NULL;
	num = strtol(argv[1], &endptr, 10);
    if (errno == ERANGE){
        perror("Number is out of range");
        return -1;
    }
    else if (*endptr != '\0'){
        perror("Invalid number");
        return -1;
    }
    else if(num < 0){
    	perror("Negative number");
        return -1;
    }

   	
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
    double t1, t2, dt;
    t1 = MPI_Wtime();
    double step_res = 0.0;
    for(i = 0; i < num / size; i++){
    	if(i % 2 != 0){
    		step_res += pi_partial_amount(rank + i * size);
    	}else{
    		step_res += pi_partial_amount((size - rank - 1) + i * size);
    	}
    }
    if((i * size + rank) < num){
    	step_res += pi_partial_amount(rank + i * size);
    	
    }
    if(MPI_Reduce(&step_res, &res, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD)){
    	perror("Error in reducing results");
    }
    if (rank == 0){
    	printf("%lf\n", res);
    }
    MPI_Finalize();
    t2 = MPI_Wtime();
	dt = t2 - t1;
	printf(" Parallel time:  %lf\n", dt);
	t1 = MPI_Wtime();
	step_res = 0;
	for(i = 0; i < num; i++){    	
    		step_res += pi_partial_amount(i);
    }
    printf("%lf\n", step_res);
    t2 = MPI_Wtime();
    double ac_time = dt;
    dt = t2 -t1;
    printf(" Solo time:  %lf\n", dt);
    printf("Acceleration: %lf\n", dt / ac_time);
    printf("Eff : %lf\n", dt/ac_time / size );
    return 0;

}
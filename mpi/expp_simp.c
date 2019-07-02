#include <stdio.h>
#include <mpi.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gmp.h>
#define MAX 1000000


void factorial(unsigned int n, mpf_t res) {
	mpf_set_d(res, 1.0);
    while (n > 1) {
      mpf_mul_ui(res, res, n);
      n = n - 1;
  	}
}


int main(int argc, char  *argv[])
{

	MPI_Status status;
	mpf_set_default_prec(MAX);
	if(argc != 3){
		perror("Not enough arguments");
		return -1;
	}
	int rank, size, message, count, num;
	double x_d = 0.0;
	char *endptr = NULL;
	double t_1, t_2;
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

    x_d = strtof(argv[2], &endptr);
    if (errno == ERANGE){
        perror("Number is out of range");
        return -1;
    }
    else if (*endptr != '\0'){
        perror("Invalid number");
        return -1;
    }

    t_1 = MPI_Wtime();
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
    mpf_t x, step_x;
	mpf_init(x);
	mpf_init(step_x);
    mpf_t proc_res;
	mpf_init(proc_res);
	mpf_set_d(proc_res, 1.0);
	mpf_set_d(step_x, 1.0);
	mpf_set_d(x, x_d);
	int i = 0;
    for(i = rank * num / size; i < ((rank + 1) * num / size); i++){
    	if(i == 0)
    		continue;
    	mpf_mul(step_x, step_x, x);
    	mpf_div_ui(step_x, step_x, i);
    	mpf_add(proc_res, proc_res, step_x);
    }
    char buf[MAX + 1];
    char last_num[MAX + 1];
    if(rank == 0){
    	for (int i = 1; i < size; i++){
    		MPI_Recv(buf, MAX, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
    		MPI_Recv(last_num, MAX, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
    		mpf_set_str (x, buf, 10);
    		mpf_mul(x, x, step_x);
    		mpf_set_str(step_x, last_num, 10);
    		mpf_add(proc_res, proc_res, x);	
    	}
    	t_2 = MPI_Wtime();
    	gmp_printf ("num is  %.Ff\n", proc_res);
    	printf("TIME  =   %f\n", t_2 - t_1);
    	mpf_clear(proc_res);
    	mpf_clear(x);
    	mpf_clear(step_x);
    }else{
    	
    	gmp_sprintf(buf, "%.Ff", proc_res);
    	gmp_sprintf(last_num, "%.Ff", step_x);
    	MPI_Send(buf, strlen(buf), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    	MPI_Send(last_num, strlen(last_num), MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    	mpf_clear(x);
    	mpf_clear(step_x);
    	mpf_clear(proc_res);
    }

    MPI_Finalize();
	return 0;
}


//gcc -lgmp



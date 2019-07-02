#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>



double f_x(double x){
	return sin(x);
}
 

int main(int argc, char  *argv[]){

	double tau, a , b, res = 0, x = 0;
	int num_of_threads, n;
	char* endptr = NULL;
	a = -1;
	b = 1;
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

    num_of_threads = strtol(argv[2], &endptr, 10);
    if (errno == ERANGE){
        perror("Number is out of range");
        return -1;
    }
    else if (*endptr != '\0'){
        perror("Invalid number");
        return -1;
    }
    else if(num_of_threads < 0){
    	perror("Negative number");
        return -1;
    }


    omp_set_num_threads(num_of_threads);
    n =  (int)((b - a) / tau);
    #pragma omp parallel for firstprivate(tau)\
    		firstprivate(a)\
    		reduction(+:res)\
    		schedule(dynamic, 10)
    
    for (int i = 0; i < n; i++){
        x = a + i * tau;
       // printf("%lf     %d    %lf\n",tau ,  num_of_threads, x);
        res += f_x(x) * tau;
    }

    printf("%lf\n", res);
 

}
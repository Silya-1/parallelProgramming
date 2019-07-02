#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <gmp.h>
#include <errno.h>
#define MAX 100
void factorial(unsigned int n, mpf_t res) {
	mpf_set_d(res, 1.0);
    while (n > 1) {
      mpf_mul_ui(res, res, n);
      n = n - 1;
  	}
}


void teylor(unsigned int n, mpf_t res){
	mpf_t fact;
	mpf_init(fact);
	mpf_pow_ui(res, res, n);
	factorial(n, fact);
	mpf_div(res, res, fact);
	mpf_clear(fact);
}


double timer()
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (double)ts.tv_sec + 1e-6 * (double)ts.tv_usec;
}


int main(int argc, char const *argv[])
{
    mpf_set_default_prec(MAX);
    int power, n, num_of_threads = 0;
    errno = 0;
    char buf[MAX + 1];
    char *endptr = NULL;
    double t, x_d = 0.0;
    num_of_threads = strtol(argv[1], &endptr, 10);	
    x_d = strtof(argv[2], &endptr);
    n = strtol(argv[3], &endptr, 10);	
    //init result array of our prog
    mpf_t *res = malloc(num_of_threads * sizeof(mpf_t));
    mpf_t e_x;
    mpf_init(e_x);
    mpf_set_d(e_x, 0.0);
    for(int i = 0; i < num_of_threads; i++){
        mpf_init(res[i]);
        mpf_set_d(res[i], 0.0);
    }
 

    srand(time(NULL));
    t = timer();
    omp_set_num_threads(num_of_threads); 
    int lock = 0;
    //main parallel section
    #pragma omp parallel
	{
        int id =  omp_get_thread_num();
        mpf_t x, step_x;
	    mpf_init(x);
	    mpf_init(step_x);
	    mpf_set_d(step_x, 1.0);
	    mpf_set_d(x, x_d);

        // mpf_t ch;
        // mpf_init(ch);
        // mpf_set_d(ch, 0.0);
        int i = 0;
        for(i= id * n / num_of_threads; i < ((id + 1) * n / num_of_threads); i++){
    	    if(i == 0){
                mpf_set_d(res[i], 1.0);
    		    continue;
            }
    	    mpf_mul(step_x, step_x, x);
    	    mpf_div_ui(step_x, step_x, i);
    	    mpf_add(res[id], res[id], step_x);
        }
        // mpf_add(ch, ch, res[id]);
        // printf("%s", ch);
        #pragma omp barier
        for(; lock < num_of_threads;){
            if(id == lock){
                // gmp_sprintf(buf, "%.Ff", ch);
                // printf("%s\n", buf);
                if(lock < num_of_threads - 1){
                    mpf_mul(res[id + 1], res[id + 1], step_x);
                    mpf_add(e_x, res[id], e_x);
                    lock += 1;
                }else{
                    mpf_add(e_x, res[id], e_x);
                    lock += 1;
                }
            }
        }
        mpf_clear(x);
        mpf_clear(step_x);
    }
    t = timer() - t;
    printf("%lf\n", t);
    gmp_sprintf(buf, "%.Ff", e_x);
    printf("%s\n", buf);
    //free memory
    for(int i = 0; i < num_of_threads; i++){
        mpf_clear(res[i]);
    }
    mpf_clear(e_x);
    free(res);
    return 0;
}


   // mpf_t fact;
    // mpf_init(fact);
    // mpf_set_d(fact, 1.0);
    // mpf_set(res[0], fact);
    // gmp_sprintf(buf, "%.Ff", res[0]);
    // mpf_clear(fact);
    
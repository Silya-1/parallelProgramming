#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>

void merge(int *a, int *b, int *c, int na, int nb);
void merge_sort(int *a, int na);
void print(int *a, int na);
int check_sort(int *a, int n);
double timer();

int pow2(int i)
{
	return 1 << i;
}

int main (int argc, char *argv[])
{
	int i, n, *a, init_n;
	double t;
	if (argc < 2) {
		printf("Usage: %s num_elements.\n", argv[0]);
		return 1;
	}
	n = atoi(argv[1]);
	a = (int*)malloc(sizeof(int) * n);
	srand(time(NULL));

	for (i = 0; i < n; i++) a[i] = rand() % 1000;

	//if (n < 101) print(a, n);
	t = timer();
    int num_of_trhreads = atoi(argv[2]);
    num_of_trhreads = num_of_trhreads % 2 == 0? num_of_trhreads : num_of_trhreads - 1;
	omp_set_num_threads(num_of_trhreads); 
    a = (int*)realloc(a, (n + n % num_of_trhreads) * sizeof(int));
    memset(a + n, 0, n % num_of_trhreads * sizeof(int));
    init_n = n;
    n += n % num_of_trhreads;
    //print(a, n);
	#pragma omp parallel
	{
		/* Распараллелить используя сбор по алгоритму гиперкуб. */
        int id =  omp_get_thread_num();
        int i_pow = 1;
        int len = 0;
        len = n / num_of_trhreads;
		merge_sort(a + id * len, len);
        int iterarte = num_of_trhreads;
        #pragma omp barrier   
        while(iterarte > 0){
            int ids_stage = pow2(i_pow);
            int len_stage = len * ids_stage / 2;
            if(id % ids_stage == 0 && (id + ids_stage / 2) < n){
                int *b = (int*)malloc(sizeof(int) * ids_stage * len);
                merge(a + id * len, a + (id + ids_stage / 2) * len, b, len_stage, len_stage);
                memcpy(a + id * len, b, sizeof(int) * ids_stage * len);
            }
            #pragma omp barrier
            iterarte -= 2;
            i_pow++;
        }
        if(id == 0){
            int ids_stage = pow2(i_pow);
            int len_stage = len * ids_stage / 2;
            int *b = (int*)malloc(sizeof(int) * n);
            merge(a, a + (id + ids_stage / 2) * len, b, len_stage, n - len_stage);
            memcpy(a, b, sizeof(int) * n);
        }
	}
	t = timer() - t;
    int *b = (int*)malloc(sizeof(int) * init_n);
    memcpy(b, a + (n - init_n), sizeof(int) * init_n);
    n = init_n;
	// if (n < 101) print(b, init_n);
	printf("%f\n", t);
	//printf("Time: %f sec, sorted: %d\n", t, check_sort(a, n));
	free(a);
    free(b);
	return 0;
}

double timer()
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (double)ts.tv_sec + 1e-6 * (double)ts.tv_usec;
}

int check_sort(int *a, int n)
{
	int i;
	for (i = 0; i < n - 1; i++) {
		if (a[i] > a[i+1]) {
			return 0;
		}
	}
	return 1;
}

void print(int *a, int na)
{
	int i;
	for (i = 0; i < na; i++) printf("%d ", a[i]);
	printf("\n");
}

/*
 * Процедура слияния массивов a и b в массив c.
 */
void merge(int *a, int *b, int *c, int na, int nb)
{
	int i = 0, j = 0;
	while (i < na && j < nb) {
		if (a[i] <= b[j]) {
			c[i + j] = a[i];
			i++;
		} else {
			c[i + j] = b[j];
			j++;
		}
	}
	if (i < na) {
		memcpy(c + i + j, a + i, (na - i) * sizeof(int));
	} else {
		memcpy(c + i + j, b + j, (nb - j) * sizeof(int));
	}
}

/*
 * Процедура сортировки слиянием.
 */
void merge_sort(int *a, int na)
{
	if(na < 2) return;
	merge_sort(a, na / 2);
	merge_sort(a + na / 2, na - na / 2);

	int *b = (int*)malloc(sizeof(int) * na);
	
	merge(a, a + na / 2, b, na / 2, na - na / 2);
	
	memcpy(a, b, sizeof(int) * na);
	
	free(b);
}

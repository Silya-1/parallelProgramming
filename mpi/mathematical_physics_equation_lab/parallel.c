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

double a, b, T_1, T_0, tau, h;


void count_step(double * res, int i , int j, int step_h, double next_val, int hor_len, double prev_val){
	res[(i + 1) * hor_len + j + 1] = f_t_x(T_0 + i * tau, a + (step_h + 1) * h);
	res[(i + 1) * hor_len + j + 1] +=  0.5 * tau * (next_val  - 2 * res[i * hor_len + j + 1] + prev_val) / (h * h);
	res[(i + 1) * hor_len + j + 1] -= (next_val - prev_val) / (2*h);
	res[(i + 1) * hor_len + j + 1] *= tau;
	res[(i + 1) * hor_len + j + 1] += res[i * hor_len + j + 1];
}

int main(int argc, char const *argv[])
{
	//T_0 = 0 a = 0 b = 1 T_1 = 1
	a = 0;
	b = 1;
	T_1 = 1;
	T_0 = 0;
	char *endptr = NULL;
	MPI_Status status;
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
    int size, rank;
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


    int i, j, hor_len = 0;
    double *res = NULL;
    if( num_h / size <= 2){
        perror("Enter smaller step");
        MPI_Finalize();
        return 0;
    }
    if(rank != (size - 1))
        hor_len = num_h / size;
    else
        hor_len = num_h / size + num_h % size;

    res = (double*)calloc(num_tau * hor_len, sizeof(double));
    int prom_round = (int) num_h / size;

    for (j = 0; j < hor_len; j++)
        res[j] = u_x(a + (j +  rank * prom_round) * h);

    if(rank == 0){
        for (int i = 0; i < num_tau; i++){
           res[0 + i * hor_len] = u_t(T_0 + i * h); 
        }
    }


    double prev_val = 0;
    double next_val = 0;
    MPI_Request request;

    for(i = 0; i < (num_tau - 1); i++){


        MPI_Isend(&res[(i + 1) * hor_len  - 1], 1, MPI_DOUBLE, (rank + 1) % size, 1, MPI_COMM_WORLD, &request);//

        for (j = 0; j < (hor_len - 2); j++){
            
            if(j != 0)
                count_step(res, i, j, j + rank * prom_round, res[i * hor_len + j + 2], hor_len, res[i * hor_len + j]);
            else if (j == 0){
                MPI_Recv(&prev_val, 1, MPI_DOUBLE, (size + (rank - 1)) % size, 1, MPI_COMM_WORLD, &status);
                if(rank)
                    count_step(res, i, j - 1, j - 1 + rank * prom_round, res[i * hor_len + j + 1], hor_len, prev_val);     
                count_step(res, i, j, j + rank * prom_round, res[i * hor_len + j + 2], hor_len, res[i * hor_len + j]);
            }
        }



        MPI_Isend(&res[i * hor_len], 1, MPI_DOUBLE, (size + (rank - 1)) % size, 1, MPI_COMM_WORLD, &request);
        MPI_Recv(&next_val, 1, MPI_DOUBLE, (rank + 1) % size, 1, MPI_COMM_WORLD, &status);

        if(rank != (size - 1))
            count_step(res, i, j, j + rank * prom_round, next_val, hor_len, res[i * hor_len + j]);

        else{
            res[(i + 1) * hor_len + j + 1] = f_t_x(T_0 + i * tau, a + ((j + 1) + (rank * prom_round)) * h) - (res[i * hor_len + j + 1] - res[i * hor_len + j]) / h;
            res[(i + 1) * hor_len + j + 1] *= tau;
            res[(i + 1) * hor_len + j + 1] += res[i * hor_len + j + 1];
        }
    }

    if(rank){
        MPI_Send(&hor_len, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
        MPI_Send(res, hor_len * num_tau, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
        /*for (i = 0; i < num_tau; ++i){
            printf("%lf", res[i * hor_len]);
        }*/
    }else{
        double buf[num_tau * (num_h / size + num_h % size)];
        double total_res[num_tau][num_h];

        for (int i = 0; i < num_tau; i++){
            for (int j = 0; j < hor_len; j++){
                total_res[i][j] = res[i * hor_len + j];
            }
        }

        for (j = 1; j < size; j++){
            MPI_Recv(&hor_len, 1, MPI_INT, j, j, MPI_COMM_WORLD, &status);
            printf("%d     %d\n", hor_len, j);
            fflush(stdout);
            MPI_Recv(buf, hor_len * num_tau, MPI_DOUBLE, j, j, MPI_COMM_WORLD, &status);

            for (int i = 0; i < num_tau; i++){
                for (int k = 0; k < hor_len; k++){
                    total_res[i][k + j * prom_round] = buf[i * hor_len + k];//
                }
            }
            hor_len = 0;   

        }

        for (int i = 0; i < num_tau; i++){
            for (int j = 0; j < num_h; j++){
                printf("%lf ", total_res[i][j]);
            }
            printf("\n");
        }
       
    }



    MPI_Finalize();

	return 0;//(rank * num_h / size)
}









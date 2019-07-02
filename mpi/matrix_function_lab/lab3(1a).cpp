#include<iostream>
#include<string>
#include <mpi.h>
#include <cmath>
#include <fstream>
#include <sys/time.h>
double timer()
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (double)ts.tv_sec + 1e-6 * (double)ts.tv_usec;
}

int main(int argc, char  *argv[])
{
    /* code */
    int n = std::stoi(argv[1]);

    int rank, size;
    MPI_Status status;

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

    double * a = new double [n*n];
    double * b = new double [n*n];
    MPI_Request request;

    //init elemnts of array
    for(auto i = 0; i < n; i++){
        for(auto j = 0; j < n; j++){
            if(i == 0 || j == n -1)
                b[i* n + j] = 10*i + j;
            else
                b[i*n + j] = 0;
            a[i*n + j] = 0;
        }
    }

    double t = timer();
    for(auto i = rank + 1; i < n; i += size){
        for(auto k = i - 1; k >= 0; k--){
            if(k + 1 != n){
                b[(i - k) * n + k] = sin(0.00001*b[(i - k - 1) *n + k + 1]);
                if(n - i + k - 1 != k){
                    b[(n - k - 1) * n + n - i + k - 1] = sin(0.00001*b[(n - k - 2)* n + n - i + k]);
                }
            }
        }
        // std::cout<<i<<std::endl;
    }

    // for(auto i = 0; i < n; i++){
    //     a[i] = 0;
    //     a[i * n  + n-1] = 0;
    // }
     if(rank == 0){
        t = timer() - t;
        std::cout<<t<<std::endl;
     }
    if(size > 1)
        MPI_Reduce(b,a, n *  n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   

        //std::ofstream fout("cout.txt");
        // for(auto i = 1; i < n; i++){
        //     for(auto j = 0; j < n - 1; j++){
        //         fout << a[i][j] << " ";
        //     }
        //     fout << '\n';
        // }
    delete[] a;
    delete[] b;
    MPI_Finalize();
    return 0;
}

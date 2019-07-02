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

    double** a = new double* [n];
    int buf_size = std::max(n * n / size,(1 -  (size - 1) / size) * n * n);  
    double* b = new double [buf_size];
    for(auto i = 0; i < n; i++){
        a[i] = new double[n];
    }
    MPI_Request request;

    //init elemnts of array
    for(auto i = 0; i < n; i++){
        for(auto j = 0; j < n; j++){
            a[i][j] = 10*i + j;

        }
    }

    double t = timer();
    int k = 0;
    for(auto i = rank * n/ size; i < std::min((rank + 1) * n/ size, n - 3); i += 1){
        for(auto j = 4; j < n; j++){
            a[i][j] = sin(0.00001*a[i+3][j-4]); 
            b[k++] = a[i][j];
        }
    }

    if(rank == 0){
        std::cout<<timer() - t<<std::endl;
        for(auto k = 1; k < size; k++){
            int len =  k == size - 1? n * n / size : (1 -  (size - 1) / size) * n * n;
            MPI_Recv(b, len, MPI_DOUBLE, k, 0, MPI_COMM_WORLD, &status);
            int iter = 0;
            for(auto i = k * n/ size; i < std::min((k + 1) * n/ size, n - 3); i += 1){
                for(auto j = 4; j < n; j++){
                    a[i][j] = b[iter++];
                }
            }
        }
    }else{
        MPI_Send(b, k, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    if(rank == 0){

        // std::ofstream fout("cout.txt");
        // for(auto i = 0; i < n; i++){
        //     for(auto j = 0; j < n; j++){
        //         fout << a[i][j] << " ";
        //     }
        //     fout << '\n';
        // }
    }
    for(auto i = 0; i < n; i++)
        delete[] a[i];
    delete[] a;
    delete[] b;
    MPI_Finalize();
    return 0;
}

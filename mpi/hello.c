#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]){
    int rank, size, i, message;
    printf("ssss");
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
    printf("RANK:%d of %d\n", rank, size);
    MPI_Finalize();
    return 0;
}
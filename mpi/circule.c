#include <stdio.h>
#include "mpi.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
int main(int argc, char* argv[]){
	int rank, size, message, count;
	char* buf;
	MPI_Status status;
	int len = 0;
	if((len = strlen(argv[1])) < 1){
		perror("Error in receiving lenth");
		return -1;
	} 
	if(argc < 2){
		perror("Not enough arguments");
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

    buf = (char*)calloc(len + 1, sizeof(char));
    if(buf == NULL && errno == ENOMEM){
    	perror("No memory available");
    	return -1;
    }

    if(rank == 0){
    	//printf("My rank is :%d, in buf: %s\n", rank, argv[1]);
    	MPI_Send(argv[1], len, MPI_CHAR, (rank + 1) % size, 0, MPI_COMM_WORLD);
    	MPI_Recv(buf, len, MPI_CHAR, size - 1, 0, MPI_COMM_WORLD, &status);
    	MPI_Get_count(&status, MPI_CHAR, &count);
    	if(count != len){
    		perror("error in receiving");
    		return -1;
    	}
    	printf("My rank is :%d, in buf: %s\n", rank, buf);    
        fflush(stdout);

    }else{
    	MPI_Recv(buf, len, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
    	printf("My rank is :%d, in buf: %s\n", rank, buf);
        fflush(stdout);
    	MPI_Get_count(&status, MPI_CHAR, &count);
    	if(count != len){
    		perror("error in receiving");
    		return -1;
    	}
    	MPI_Send(argv[1], len, MPI_CHAR, (rank + 1) % size, 0, MPI_COMM_WORLD);
    }
    free(buf);
    MPI_Finalize();
    return 0;

}
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <mpi.h>
#include <errno.h>
#include <stdlib.h> 


#define first_hash 1024


unsigned long max = ULONG_MAX;

unsigned long hash(unsigned char *str){
	unsigned long hash = 5381;
    int c;
    while (c = *str++)
    	hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}
 
 void reverse(char* s){
    int i, j;
    char c;
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(unsigned long n, char* s){
	int i = 0;
    do {      
        s[i++] = n % 10 + '0';   
    } while ((n /= 10) > 0);   
    s[i] = '\0';
    reverse(s);
} 



int main(int argc, char* argv[]){

	char* endptr = NULL;
	char* data;
	unsigned long dif;
	MPI_Status status;
	int rank, size, message, count, i, num, len;
	if((len = strlen(argv[1])) < 1){
		perror("Error in receiving size");
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

    if(rank == 0){
    	data = calloc(size + 1, sizeof(char));
    	strcpy(data, argv[1]);
		dif = strtoul(argv[2], &endptr, 10);
	    if (errno == ERANGE){
	        perror("Number is out of range");
	        return -1;
	    }
	    else if (*endptr != '\0'){
	        perror("Invalid number");
	        return -1;
	    }
	    else if(dif < 0){
	    	perror("Negative number");
	        return -1;
	    }
	    for(int i = 1; i < size; i++){
	    	MPI_Send(&dif, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
	    }
	    MPI_Recv(&dif, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	    printf("res hash %lu  data:%s\n", dif, data);
    }else{
    	MPI_Recv(&dif, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status);
    	unsigned long step = max / size;
    	unsigned long hs = 0;
    	unsigned long result = 0;
    	unsigned char str_1[1024];
		unsigned char str_2[1024];
		int flag = 0;
		MPI_Status status;
		MPI_Request request;
		itoa(first_hash, str_2);
    	for(unsigned long j = rank * step; i < (rank + 1) * step; j++){
    		itoa(j, str_1);
			strcat(str_1, str_2);
			hs = hash(str_1);
			if(hs < (max - dif)){
				for (i = 0; i < size; i++){
					if(i != rank)
	    				MPI_Send(&dif, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);	
	    		}
	    		break;
			}
			MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
			if(flag){
				MPI_Recv(&result, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				break;
			}
    	}
    }
   
    MPI_Finalize();
	return 0;
}









#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int ProcNum, ProcRank;
    int useless;
    MPI_Status Status;

    useless = 0;


    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0)
    {
        printf("Rank %d\n", ProcRank);
        fflush(stdout);
        MPI_Send(&useless, 1, MPI_INT, ProcRank + 1, ProcRank, MPI_COMM_WORLD);
        fflush(stdout);
    } else
    if (ProcRank == ProcNum - 1)
    {
    	fflush(stdout);
        MPI_Recv(&useless, 1, MPI_INT, ProcRank - 1, ProcRank - 1, MPI_COMM_WORLD, &Status);
        printf("Rank %d\n", ProcRank);
        fflush(stdout);
    } else
    {
    	fflush(stdout);
        MPI_Recv(&useless, 1, MPI_INT, ProcRank - 1, ProcRank - 1, MPI_COMM_WORLD, &Status);
        printf("Rank %d\n", ProcRank);
        fflush(stdout);
        MPI_Send(&useless, 1, MPI_INT, ProcRank + 1, ProcRank, MPI_COMM_WORLD);
    }

    MPI_Finalize();


    return 0;
}

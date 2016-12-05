#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int nintervals; 

void pi()
{
  int myid, numprocs, i, n;
    double PI25DT = 3.141592653589793238462643;
    double mypi, pi, h, sum, x;

    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    if (myid == 0) n = nintervals;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
    h   = 1.0 / (double) n;
    sum = 0.0;
    for (i = myid + 1; i <= n; i += numprocs) {
        x = h * ((double)i - 0.5);
        sum += 4.0 / (1.0 + x*x);
    }
    mypi = h * sum;
    
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0,
	       MPI_COMM_WORLD);
    
    if (myid == 0) printf("Pi: %.16f Error: %.16f\n",
			  pi, fabs(pi - PI25DT));
}

#define error(format, ...) fprintf(stderr, format , __VA_ARGS__)

int main(int argc, char **argv)
{
    int nprocs;
    int myrank;
    int stat;
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    double t1;

    MPI_Init(&argc, &argv);
    stat = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if ( stat != 0 ) error ("MPI_Comm_size returned an error code : %d", stat);

    stat = MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if ( stat != 0 ) error ("MPI_Comm_rank returned an error code : %d", stat);

    if (myrank == 0) {
        t1 = MPI_Wtime();
        if(argc == 3) {
            nintervals = atoi(argv[1]);
            sleep(atoi(argv[2]));
            printf("Args: argc=%d n=%d sleep=%d\n", argc, atoi(argv[1]), atoi(argv[2]));
         }
         else {
             fprintf(stderr,"usage: hipi nintervals sleeptime\n");
             MPI_Abort(MPI_COMM_WORLD,1);
	 }
    }

    MPI_Get_processor_name(processor_name, &namelen);

    printf("Process %d out of %d on host %s\n", myrank, nprocs, processor_name);

    pi();

    if (myrank == 0) printf("Elapsed time: %f\n", MPI_Wtime() - t1); 

    MPI_Finalize();
    return 0;
}

// Time functions for later use (needs debugging, causes pi to return NaN).

#define TIMEBUFLEN 26

  char buffer[TIMEBUFLEN];
  int millisec;
  struct tm* tm_info;
  struct timeval tv;

void prtime_msec() {

  fprintf(stdout, "Time: 12:34:56\n");
  gettimeofday(&tv, NULL);
  return;

  millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
  if (millisec>=1000) { // Allow for rounding up to nearest second
    millisec -=1000;
    tv.tv_sec++;
  }

  tm_info = localtime(&(tv.tv_sec));

  strftime(buffer, TIMEBUFLEN, "%Y:%m:%d %H:%M:%S", tm_info);
  fprintf(stdout, "Time: %s.%03d\n", buffer, millisec);
}

void prtime()
{
  time_t timer;
  char buffer[26];
  struct tm* tm_info;

  time(&timer);
  tm_info = localtime(&timer);

  strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
  puts(buffer);
}


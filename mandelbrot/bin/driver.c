#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <assert.h>
#include "mandelbrot.h"

#define DEBUG 1

#define error(format, ...) fprintf(stderr, format , __VA_ARGS__)
#define debug(format, ...) if (DEBUG == 1) fprintf(stdout, format , __VA_ARGS__)

#define PREDEF 1
#define MASTER_SLAVE 2

int X_MAX = 10000;
int Y_MAX = 10000;

int dump_mandelbrot(char *buffer, char *filename)
{
    char ppm_to_jpg[1000];
    FILE * fp = fopen(filename, "w");
    fprintf(fp, "P6\n# CREATOR: Yadu Nand / mandel program\n");
    fprintf(fp, "%d %d\n255\n",X_MAX,Y_MAX);
    fwrite(buffer, sizeof(char), 3*X_MAX*Y_MAX, fp);
    //snprintf(ppm_to_jpg, 1000, "convert %s -resize 1000x1000 %s", filename, filename);
    //system(ppm_to_jpg);
    fclose(fp);
    return 0;
}


int predef_run_server(int myrank, int nprocs, int strategy, int itermax, char* output_filename)
{
    int i;
    int slice_size;
    int slice_start;
    int slice_end;
    int rcv_buf_size;
    MPI_Status stat;
    //double start, stop;
    char * buffer = malloc(X_MAX * Y_MAX * 3 * sizeof(char));
    slice_size = Y_MAX / nprocs;
    //printf("Nprocs : %d", nprocs);
    //printf("slice_size : %d", slice_size);
    slice_start = slice_size * myrank;
    if (myrank == (nprocs - 1)){
        slice_end = Y_MAX - 1;
    }else{
        slice_end = slice_start + slice_size ;
    }
    mandelbrot(slice_start, slice_end, buffer, itermax);
    // Collate results from all ranks
    for ( i = 1; i < nprocs ; i++ ){
        //printf("[%d] Starting message to proc: %d\n", 0, i);
        if (i == (nprocs - 1)){
            rcv_buf_size = Y_MAX - (slice_size*i);
        }else{
            rcv_buf_size = slice_size;
        }
        MPI_Recv(buffer+(X_MAX*3*slice_size*i), rcv_buf_size*3*X_MAX, MPI_CHAR, i, 0,
                 MPI_COMM_WORLD, &stat);
    }

    dump_mandelbrot(buffer, output_filename);
    free(buffer);
    return 0;
}

int predef_run_client(int myrank, int nprocs, int strategy, int itermax)
{
    int slice_size;
    int slice_start;
    int slice_end;
    //double start, stop;
    char * buffer = malloc(X_MAX * Y_MAX * 3 * sizeof(char));
    slice_size = Y_MAX / nprocs;
    printf("Nprocs : %d", nprocs);
    printf("slice_size : %d", slice_size);
    slice_start = slice_size * myrank;
    if (myrank == (nprocs - 1)){
        slice_end = Y_MAX - 1;
    }else{
        slice_end = slice_start + slice_size ;
    }
    // Do the actual work per rank
    mandelbrot(slice_start, slice_end, buffer+(X_MAX*3*slice_start), itermax);

    // Send results to server to collate
    MPI_Send(buffer+(X_MAX*3*slice_start), (slice_end-slice_start)*3*X_MAX, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    free(buffer);
    return 0;
}

#define TASK_REQ 1

int master(int myrank, int nprocs, int strategy, int itermax, char *output_filename)
{
    int i, offset, slice_size;
    MPI_Status stat;

    int task_descriptor[4] = {0,0,0,0};
    char * buffer = malloc(X_MAX * Y_MAX * 3 * sizeof(char));
    //char * request = malloc(10*sizeof(char));
    // Slices are in multiples on rows.
    slice_size = SLICE_SIZE;

    // In this world slaves ask for work! and there is work to do
    for ( i = 0 ; i < Y_MAX - SLICE_SIZE ; i+=SLICE_SIZE ) {
        MPI_Recv(task_descriptor, 4, MPI_INT, MPI_ANY_SOURCE, TASK_REQ, MPI_COMM_WORLD, &stat);
        task_descriptor[0] = 1; // There's work!
        task_descriptor[1] = i;
        task_descriptor[2] = i+SLICE_SIZE;
        MPI_Send(task_descriptor, 4, MPI_INT, stat.MPI_SOURCE, TASK_REQ, MPI_COMM_WORLD);
    }

    debug("%s", "[MASTER] ALL TASKS DISPATCHED\n");
    // At this point all work has been dispatched, and results are to be collected
    // Tell workers that their work is done
    for ( i = 1 ; i < nprocs; i++ ) {
        debug("[MASTER] Sending die to %d  \n", i);
        MPI_Recv(task_descriptor, 4, MPI_INT, i, TASK_REQ, MPI_COMM_WORLD, &stat);
        task_descriptor[0] = 0; // There's no more work!
        task_descriptor[1] = 0;
        task_descriptor[2] = 0;
        MPI_Send(task_descriptor, 4, MPI_INT, i, TASK_REQ, MPI_COMM_WORLD);
    }

    debug("%s", "[MASTER] ALL WORKERS ASKED TO RETURN RESULTS\n");
    // At this point slaves know that the master expect results back
    // Master listens to results sent from slaves, who send one slice
    // at a time

    // Slaves send  buffer with offset in the mpi_tag
    char *recv_buffer = malloc( sizeof(char)*3 * X_MAX * SLICE_SIZE);

    for ( i = 0 ; i < Y_MAX - SLICE_SIZE; i+=SLICE_SIZE ) {
        MPI_Recv(recv_buffer, X_MAX* 3 *SLICE_SIZE,
                 MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
        offset = stat.MPI_TAG;
        //printf("Offset : %d\n", offset);
        memcpy(buffer+(X_MAX*3*offset), recv_buffer, X_MAX*3*SLICE_SIZE);
    }

    // At this point all results have been copied back to the Master
    dump_mandelbrot(buffer, output_filename);
    free(recv_buffer);
    free(buffer);
    return 0;
}


int slave(int myrank, int nprocs, int strategy, int itermax)
{
    int slice_size;
    MPI_Status stat;
    int offset_queue[Y_MAX];
    int offset_head = -1;

    int task_descriptor[4] = {0,0,0,0};
    char * buffer = malloc(X_MAX * Y_MAX * 3 * sizeof(char));
    //char * request = malloc(10*sizeof(char));
    // Slices are in multiples on rows.
    slice_size = SLICE_SIZE;

    // Worker asks for work in a loop
    while (offset_head < Y_MAX) {
        MPI_Send(task_descriptor, 4, MPI_INT, 0, TASK_REQ, MPI_COMM_WORLD);
        MPI_Recv(task_descriptor, 4, MPI_INT, 0, TASK_REQ, MPI_COMM_WORLD, &stat);

        printf("[SLAVE:%d] TASK: %d START: %d END: %d \n", myrank, task_descriptor[0],
              task_descriptor[1], task_descriptor[2]);
        // No more work
        if ( task_descriptor[0] == 0 ) break;

        // There's work to do
        mandelbrot(task_descriptor[1], task_descriptor[2],
                   buffer+(X_MAX*3*task_descriptor[1]), itermax);
        offset_head++;
        offset_queue[offset_head] = task_descriptor[1];
    }

    // At this point all work has been completed.
    // Slaves will send results back
    for ( ; offset_head >= 0; offset_head-=1 ) {
        MPI_Send(buffer+(X_MAX*3*offset_queue[offset_head]),
                         slice_size*3*X_MAX, MPI_CHAR, 0,
                         offset_queue[offset_head], // HACK :)
                         MPI_COMM_WORLD);
    }
    // At this point all results have been copied back to the Master
    free(buffer);
    return 0;
}

void usage(void){
    printf("./mandelbrot -r <resolution> -s <strategy 0/1> -i <iterations> -f <output_filename> <\n");
    return;
}


int main(int argc, char **argv)
{
    int nprocs;
    int myrank;
    int stat;
    int namelen;
    char ch;
    char output_filename[1000];
    int itermax;
    double start, stop;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int strategy = MASTER_SLAVE;

    while ((ch=getopt(argc,argv,"s:i:f:r:")) != -1){
        switch(ch){
        case 's' : strategy = atoi(optarg) ;
            break;
        case 'i' : itermax  = atoi(optarg) ;
            break;
        case 'r' : X_MAX    = atoi(optarg) ; Y_MAX = X_MAX;
            break;
        case 'f' : snprintf(output_filename, 1000, "%s", optarg);
            break;
        case '?' : printf(" Error : Unknown argument list" );
            usage();
            exit(1) ;
        }
    }

    debug("[DEBUG]: filename : %s\n", output_filename);
    debug("[DEBUG]: itermax  : %d\n", itermax);
    debug("[DEBUG]: strategy : %d\n", strategy);

    MPI_Init(&argc, &argv);
    stat = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if ( stat != 0 ) error ("MPI_Comm_size returned an error code : %d", stat);

    stat = MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if ( stat != 0 ) error ("MPI_Comm_rank returned an error code : %d", stat);

    MPI_Get_processor_name(processor_name, &namelen);

    printf("Process %d on %s out of %d\n", myrank, processor_name, nprocs);
    //printf("Nprocs: %d, Nrank: %d \n", nprocs, myrank);
    start = MPI_Wtime();
    if (strategy == PREDEF){
        if ( myrank == 0 ){
            predef_run_server(myrank, nprocs, strategy, itermax, output_filename);
        }else{
            predef_run_client(myrank, nprocs, strategy, itermax);
        }
    }else if(strategy == MASTER_SLAVE){
        if ( myrank == 0 ){
            master(myrank, nprocs, strategy, itermax, output_filename);
        }else{
            slave(myrank, nprocs, strategy, itermax);
        }
    }
    stop = MPI_Wtime();
    if (myrank == 0){
        fprintf(stdout, "NPROCS: %d Time to completion: %f seconds\n", nprocs, stop-start);
    }
    MPI_Finalize();
    return 0;
}

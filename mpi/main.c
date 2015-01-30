/*
    Argonne Leadership Computing Facility benchmark
    BlueGene/P version
    Messaging rate
    Written by Vitali Morozov <morozov@anl.gov>
    Updated 20090929: made bi-directional
*/

#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXN 150000     // repetition rate for a single pair test
#define MAXSKIP 20      // skip first tests to warm-up
#define N 6             // number of simumtaneous communications (available links)
#define NDIMS 3         // Cartesian mesh dimension
//#define Hz 850.e6      // CPU clock rate, in Hz
#define Hz 2100.e6      // CPU clock rate, in Hz
#define DATALENGTH 1024      // message size, in doubles
#define L3CACHE 32     // last level cache in MB, used to flash the cache before each test
#define LOOPS 30     // Enough loops to keep active for 1min
#define REQSPERDIM 4 // Max number of simultaneous requests per dimension

#ifdef HALO_TIMER_BGQ_TIMEBASE
typedef unsigned long long halo_time_t;

halo_time_t  timebase();

static halo_time_t timebase_diff(halo_time_t t1, halo_time_t t2) {
  return t2 - t1;
}

static double timebase_us(halo_time_t t) {
  return (double)( t ) / Hz * 1e6
}

#define halo_timer(t1) *(t1) = timebase()
#define halo_time_ticks(t) (t)
#define halo_time_diff(t1, t2) timebase_diff((t1), (t2))
#define halo_time_us(td) timebase_us(td)
#endif

#ifdef HALO_TIMER_CLOCK_REALTIME
#include <time.h>
typedef struct timespec halo_time_t;

static halo_time_t timespec_diff(halo_time_t t1, halo_time_t t2) {
  halo_time_t res;
  res.tv_sec = t2.tv_sec - t1.tv_sec;
  if (t2.tv_nsec > t1.tv_nsec) {
    res.tv_nsec = t2.tv_nsec - t1.tv_nsec;
  } else {
    res.tv_sec--;
    res.tv_nsec = 1000000000L - t1.tv_nsec + t2.tv_nsec;
  }

  return res;
}

static double timespec_us(halo_time_t t) {
  return (t.tv_sec * 1e6 + ((double)t.tv_nsec) / 1e3);
}

#define halo_timer(t1) clock_gettime(CLOCK_REALTIME, t1)
#define halo_time_ticks(t) ((t).tv_nsec + ((long long)(t).tv_sec) * 1000000000)
#define halo_time_diff(t1, t2) timespec_diff((t1), (t2))
#define halo_time_us(td) timespec_us(td)

#endif

int main( int argc, char *argv[] )
{
    int rc;
    int taskid, ntasks, i, j;
    int LENGTH = DATALENGTH;
    if ( argc == 2 ){
        LENGTH = atoi(argv[1]);
    }
    double sb[NDIMS][2][LENGTH], rb[NDIMS][2][LENGTH]; // buffers: 2 displacements in each dimension
    halo_time_t t1, t2, tdelay;
    int dims[NDIMS], periods[NDIMS], reorder;
    MPI_Comm comm_cart;
    MPI_Request req[REQSPERDIM*NDIMS];
    MPI_Status  stt[REQSPERDIM*NDIMS];
    double seed, delay;
    int one;
    useconds_t udelay;
    int rank_src[2][NDIMS], rank_dst[2][NDIMS]; // For positive displacement and dimension 1, source rank is rank_src[0][1], destination rank is rank_dst[0][1];
                                                // For negative displacement and dimension 0, source rank is rank_src[1][0], destination rank is rank_dst[1][0];
    char *src, *trg; // memcpy buffers used to flash last level cache
    size_t LB;
    int loops = LOOPS;
    int outer;
    LB = L3CACHE * 1024 * 1024;
    src = (char *)malloc( LB );
    trg = (char *)malloc( LB );

    rc = MPI_Init( &argc, &argv );
    assert(rc == MPI_SUCCESS);

    rc = MPI_Comm_rank( MPI_COMM_WORLD, &taskid );
    assert(rc == MPI_SUCCESS);

    rc = MPI_Comm_size( MPI_COMM_WORLD, &ntasks );
    assert(rc == MPI_SUCCESS);

    for ( i = 0; i < NDIMS; i++ ) dims[i] = 0;
    rc = MPI_Dims_create( ntasks, NDIMS, dims );
    assert(rc == MPI_SUCCESS);

    if ( taskid == 0 ) printf( "Cartesian topology: %d x %d x %d\n", dims[0], dims[1], dims[2] );
    if ( taskid == 0 ) fprintf( stderr, "Message size : %d \n", LENGTH*8 );
    for ( i = 0; i < NDIMS; i++ ) periods[i] = 0; // no periods for this test
    reorder = 1; // reorder is allowed
    rc = MPI_Cart_create( MPI_COMM_WORLD, NDIMS, dims, periods, reorder, &comm_cart);
    assert(rc == MPI_SUCCESS);

    rc = MPI_Barrier( comm_cart );
    assert(rc == MPI_SUCCESS);
    halo_timer(&t1);

    usleep( udelay );

    rc = MPI_Barrier( comm_cart );
    assert(rc == MPI_SUCCESS);
    halo_timer(&t2);

    tdelay = halo_time_diff(t1, t2);
    //fprintf( stderr, "Hello from rank %d of %d tasks: delay %ld microseconds\n", taskid, ntasks, udelay );
    if ( taskid == 0 ) fprintf( stderr, "Delay time: %lld pclks, %18.12lf microseconds\n",
        halo_time_ticks(tdelay), halo_time_us(tdelay) );

    fprintf(stdout, "TOTAL_RANKS: %d \n", ntasks);
    fprintf(stdout, "RANK_ID    : %d \n", taskid);

    for( i = 0; i < NDIMS; i++ )
    {
        rc = MPI_Cart_shift( comm_cart, i,  1, &rank_src[0][i], &rank_dst[0][i] );
        assert(rc == MPI_SUCCESS);
        rc = MPI_Cart_shift( comm_cart, i, -1, &rank_src[1][i], &rank_dst[1][i] );
        assert(rc == MPI_SUCCESS);

        int j;
        for ( j = 0; j < 2; j++ )
        {
          if ( taskid == 0 )
          {
            fprintf(stderr, "rank_src[%i][%i]=%i\n", j, i, rank_src[j][i]);
            fprintf(stderr, "rank_dst[%i][%i]=%i\n", j, i, rank_dst[j][i]);
          }
        }
    }

    ///////////// Test Sendrecv without delay //////////////////
    memcpy( trg, src, LB );

    rc = MPI_Barrier( comm_cart );
    assert(rc == MPI_SUCCESS);
    halo_timer(&t1);

    for ( i = 0; i < NDIMS; i++ )
    {
        rc = MPI_Sendrecv( &sb[i][0][0], LENGTH, MPI_DOUBLE, rank_dst[0][i], 0, &rb[i][0][0], LENGTH, MPI_DOUBLE, rank_src[0][i], 0, comm_cart, MPI_STATUS_IGNORE );
        assert(rc == MPI_SUCCESS);
        rc = MPI_Sendrecv( &sb[i][1][0], LENGTH, MPI_DOUBLE, rank_dst[1][i], 1, &rb[i][1][0], LENGTH, MPI_DOUBLE, rank_src[1][i], 1, comm_cart, MPI_STATUS_IGNORE );
        assert(rc == MPI_SUCCESS);
    }

    rc = MPI_Barrier( comm_cart );
    assert(rc == MPI_SUCCESS);
    halo_timer(&t2);

    if( taskid == 0 ) fprintf( stderr, "Sendrecv no delay for %6d doubles: %lld pclks, %18.12lf microseconds\n",
        LENGTH, halo_time_ticks(timespec_diff(t1, t2)), halo_time_us(halo_time_diff(t1, t2)) );

    rc = MPI_Finalize();
    assert(rc == MPI_SUCCESS);

    free( src );
    free( trg );

    return 0;
}

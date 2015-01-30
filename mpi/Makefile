ifeq ($(HALO_SYS),)
	HALO_SYS := gnu
endif

ifeq ($(HALO_SYS),crayxe6-gnu)
	FF     = ftn
	CC     = cc
	LD     = $(CC)
else ifeq ($(HALO_SYS),crayxe6-cray)
	FF     = ftn
	CC     = cc
	LD     = $(CC)
else ifeq ($(HALO_SYS),bgq)
	FF     = /bgsys/drivers/ppcfloor/comm/fast/bin/mpixlf77
	CC     = /bgsys/drivers/ppcfloor/comm/fast/bin/mpixlc
	LD     = $(CC)
else ifeq ($(HALO_SYS),gnu)
	FF     = mpif90
	CC     = mpicc
	LD     = $(CC)
else
	CC     = $(error Bad HALO_SYS: $(HALO_SYS))
	FF     = $(CC)
	LD     = $(CC)
endif


CINCLUDE :=
ifeq ($(HALO_SYS),bgq)
	CINCLUDE  = -I/bgsys/drivers/ppcfloor
	CINCLUDE += -I/bgsys/drivers/ppcfloor/firmware/include
	CINCLUDE += -I/bgsys/drivers/ppcfloor/spi/include/kernel
	CINCLUDE += -I/bgsys/drivers/ppcfloor/spi/include/kernel/cnk
	CINCLUDE += -I/bgsys/drivers/ppcfloor/arch/include
endif

FFLAGS = -g
CFLAGS = -g $(CINCLUDE) #-qnostaticlink
LDFLAGS = $(CFLAGS)
LIB =

ifeq ($(HALO_SYS),crayxe6-gnu)
        CFLAGS += -Wall -DHALO_TIMER_CLOCK_REALTIME -DFTN_UNDERSCORE
	LIB +=
else ifeq ($(HALO_SYS),crayxe6-cray)
        CFLAGS += -DHALO_TIMER_CLOCK_REALTIME -DFTN_UNDERSCORE
	LIB +=
else ifeq ($(HALO_SYS),bgq)
        CFLAGS += -Wall -DHALO_TIMER_BGQ_TIMEBASE
	LIB += -L$(HOME)/lib -ltimebase
else ifeq ($(HALO_SYS),gnu)
        CFLAGS += -DHALO_TIMER_CLOCK_REALTIME -DFTN_UNDERSCORE
	LIB += -lgfortran
endif

TARGET = mmps
OBJS =  main.o durand.o

ifeq ($(HALO_SYS),bgq)
  OBJS += q5d.o
endif

all: grid

%.o : %.f
	$(FF) $(FFLAGS) -c $<

%.o : %.c
	$(CC) $(CFLAGS) -c $<

grid: $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIB)

clean:
	rm -f $(TARGET) $(OBJS)


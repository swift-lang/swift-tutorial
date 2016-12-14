type file;

string curdir = java("java.lang.System","getProperty","user.dir");

app (file out, file err) mpi_hello (int time, int nproc)
{
    mpiwrap nproc mpiapp time stdout=@out stderr=@err;
}

int    nsim   = toInt(arg("nsim",   "10"));
int    time   = toInt(arg("time",   "1"));
int    nproc  = toInt(arg("nproc",  "56"));

global string mpiapp = arg("mpiapp", curdir+"/mpi_hello");

foreach i in [0:nsim-1] {
  file mpiout <single_file_mapper; file=strcat("output/mpi_",i,".out")>;
  file mpierr <single_file_mapper; file=strcat("output/mpi_",i,".err")>;
  (mpiout, mpierr) = mpi_hello(time, nproc);
}

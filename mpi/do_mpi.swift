type file;

app (file out, file err) do_mpi ()
{
    mpi_app stderr=filename(err) stdout=filename(out);
}

int nsim   = toInt(arg("nsim",   "10"));

foreach i in [0:nsim-1] {
  file mpiout <single_file_mapper; file=strcat("output/mpi_",i,".out")>;
  file mpierr <single_file_mapper; file=strcat("output/mpi_",i,".log")>;
  (mpiout, mpierr) = do_mpi();
}

type file;

app (file out, file err) do_mpi ()
{
    halo stderr=filename(err) stdout=filename(out);
}

file out <"mpi.out">;
file err <"mpi.err">;

(out,err) = do_mpi();

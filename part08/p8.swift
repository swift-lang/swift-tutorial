type file;

string curdir = java("java.lang.System","getProperty","user.dir");

app (file out, file err) pi (int intervals, int duration )
{
    mpiwrap 48 pibinary intervals duration stdout=@out stderr=@err;
}

app (file out) summarize (file pi_runs[] )
{
    grep "^Pi:" filenames(pi_runs) stdout=@out;
}

int  duration    = toInt(arg("duration",  "0"));  # Min duration of pi run (artificial)

file pi_out[] <simple_mapper; prefix="output/pi_", suffix=".out">;
file pi_err[] <simple_mapper; prefix="output/pi_", suffix=".err">;

global string pibinary = arg("executable", curdir+"/hipi");

foreach interval in [10,100,1000,10000,100000] {
    (pi_out[interval],pi_err[interval]) = pi(interval, duration);
}

file summary <"output/summary.out">;

summary = summarize(pi_out);

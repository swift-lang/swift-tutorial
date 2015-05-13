type file;

app (file image, file out, file err) mandelbrot (file mandel_sh, int iterations, int resolution)
{
    bash @mandel_sh "-i" iterations "-s 1 -r" resolution "-f" @image stdout=@out stderr=@err;
}

app (file movie, file montage, file out, file err) assemble (file[] mandel_imgs)
{
    assemble @movie @montage @mandel_imgs stdout=@out stderr=@err;
}

int  itermax     = toInt(arg("niter", "20"));     # number of iterations for mandelbrot
int  step        = toInt(arg("step", "5"));       # number of iterations for mandelbrot
int  resolution  = toInt(arg("res",  "10000"));   # Resolution of result

// 5 -> 100 iterations stepping by 5
file mandel_img[] <simple_mapper; prefix="output/mandel_", suffix=".jpg">;
file mandel_out[] <simple_mapper; prefix="output/mandel_", suffix=".out">;
file mandel_err[] <simple_mapper; prefix="output/mandel_", suffix=".err">;
file mandel_sh <"./bin/run_mandelbrot">;

foreach i in [5:itermax:step]{
    tracef("i = %i \n", i);
    (mandel_img[i], mandel_out[i], mandel_err[i]) = mandelbrot(mandel_sh, i, resolution);
}

file movie   <"output/mandel.gif">;
file montage <"output/montage.jpg">;
file assemble_out <"output/assemble.out">;
file assemble_err <"output/assemble.err">;
(movie, montage, assemble_out, assemble_err) = assemble (mandel_img);

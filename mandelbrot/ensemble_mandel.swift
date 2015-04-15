type file;

app (file image, file out, file err) mandelbrot (int iterations, int resolution)
{
    mandelbrot "-i" iterations "-s 1 -r" resolution "-f" @image stdout=@out stderr=@err;
}

app (file image, file out, file err) assemble (file[] mandel_imgs)
{
    assemble @image @mandel_imgs stdout=@out stderr=@err;
}

int  itermax     = toInt(arg("nsim", "100"));     # number of iterations for mandelbrot
int  step        = toInt(arg("step", "5"));       # number of iterations for mandelbrot
int  resolution  = toInt(arg("res",  "10000"));   # Resolution of result

// 5 -> 100 iterations stepping by 5
file mandel_img[] <simple_mapper; prefix="output/mandel_", suffix=".ppm">;
file mandel_out[] <simple_mapper; prefix="output/mandel_", suffix=".out">;
file mandel_err[] <simple_mapper; prefix="output/mandel_", suffix=".err">;

foreach i in [5:itermax:step]{
    tracef("i = %i \n", i);
    (mandel_img[i], mandel_out[i], mandel_err[i]) = mandelbrot(i, resolution);
    //images[i] = mandelimage;
}

file movie <"output/movie.mp4">;
file assemble_out <"output/assemble.out">;
file assemble_err <"errput/assemble.err">;
(movie, assemble_out, assemble_err) = assemble (mandel_img);

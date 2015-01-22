type file;

app (file out) sortdata (file unsorted)
{
    sort "-n" filename(unsorted) stdout=filename(out);
}

file unsorted <"unsorted.txt">;
file sorted   <"sorted.txt">;

sorted = sortdata(unsorted);

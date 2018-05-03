int i;
int j;
int k;
int limit;
int counter;
float test;

{
    limit := 10;
    read(limit);
    counter := 0;

    i:=0;
    j:=1;
    write(1);
    write(j);
    k:=1;
    write(2);
    write(k);

    while (counter < limit-2) do {
        i:=j;
        j:=k;
        k:= i + j;
        write(counter + 3);
        write(k);
        counter := counter + 1;
    }
}
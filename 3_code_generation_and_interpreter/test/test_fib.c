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
    write(1);
    write(i);
    j:=1;
    write(2);
    write(j);
    k:=1;
    write(3);
    write(k);

    while (counter < limit-3) do {
        i:=j;
        j:=k;
        k:= i + j;
        write(counter + 4);
        write(k);
        counter := counter + 1;
    }
}
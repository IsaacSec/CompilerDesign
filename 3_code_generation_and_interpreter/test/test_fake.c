int a;
int b;
float c;
float d;
float temp;
int if_var;
int counter; 
int length;
int subcount;

{
    read(length);
    counter:=0;
    a:=0;

    write(length);

    while (counter < length) do {
        read(a);
        read(c);
        read(d);

        if (a = 0) then {
            temp := c + d;
        }
        
        if (a = 1) then {
            temp := c - d;
        }

        if (a = 2) then {
            temp := c * d;
        }

        if (a = 3) then {
            temp := c / d;
        }

        counter := counter + 1;
        write(temp);

        subcount:=0;
        while (subcount < temp) do {
            write(subcount);
            subcount := subcount + 1;
        }

        if (temp < 70) then {
            write(0);
        } else {
            write(1);
        }
    }
}
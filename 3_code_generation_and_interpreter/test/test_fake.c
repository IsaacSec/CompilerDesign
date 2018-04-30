float f_speed;
float i_speed;
float distance;
float time; 
float acceleration; 
int counter; 

counter := 0 ;
acceleration := 2.0;
i_speed := 0.0;
time := 0.0;

write(5+3);
write(5/3);
write(5.0/3);
write(5*3);
write(5-3.0);

while (time < 10) do{
    time := time + 1;
    f_speed := acceleration * time;
    write (time);
} 

write(f_speed) ;
int numOfCourses;
int counter;
float average;
float temp;

read(numOfCourses);
counter := 0;
average :=0;
temp := 0;

while(counter < numOfCourses ) do{
  read(temp);
  average := average + temp ;
  counter := counter + 1 ;
}
average := average / counter; 

if (average < 70) then {
  write(0);
} else {
  write(1);
}

write(average);



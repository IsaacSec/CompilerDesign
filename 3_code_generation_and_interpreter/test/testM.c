int counter;
int multipleOf;
int temp;

{
	read(multipleOf);
	counter := 1;
	temp := 0;
	while(counter < 20) do {
		temp := multipleOf * counter;
		write(temp);
		counter := counter + 1;
	}
}

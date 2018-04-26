/* This is the "simple" version of the test 
   program */
/* Definitions go first */

int j;
float i;
int h;
int h2;
float h3;
float j7;
int i7;
int h7;

/* There is no "main" program, just blocks */
{

  h3 := 10.0;
  i := 0;
  j := 2;

  while (j<5) do {
    j := j - 1;
  }

  h := j/i;
  h2 := h / h3;
  h := 100*h;
 
  h := (10<3);
  j := (h = h2);
  dsadas := 0;

  if (i7<23) then {
    i7 := 99;
  } else {
    i7 := 13;
    i := 3.46;
  }
  read(i);
  write(4+5);
}

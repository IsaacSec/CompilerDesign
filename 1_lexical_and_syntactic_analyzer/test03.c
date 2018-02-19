/* This si a test of a comment */

int i;
float j;

{

 /* Multi-line
  *
  * comment
  */

 j := 10.0;
 i := 0;

  while (i < 10) do {
     j := j - 1.1;

     /* Will flag an error below */
     i := i++;
  }

}
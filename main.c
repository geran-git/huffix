/*

*/
#include <stdio.h>
#include <strings.h>
int main(int argc, char *argv[])
{
  int count;

  printf ("\nHuffiX archiver - Version 1.0 - 2016\n");

  if (4 != argc)
  {
  	/*Quick help*/
  	printf("Usage: huff ifile [-c|-x] ofile\n");
  	printf("\t-c\t compress ifile to ofile\n");
  	printf("\t-x\t extract ifile to ofile\n");
  	return 0;
  }
  else
  {
  	/*Check mode option*/
  	if ( 0 == strcmp(argv[2], "-c"))
  	{
      int bitcode[256]; = {0};

  		/*Going to compress*/
  		printf ("Compressing, please wait...\n");
      /* Let's build code table later */
      /* The main goal for this moment is to implement code lookup and transform bit sequence to bytes */
      /* Lookup table is simple - array where code_value = bitcode[byte_to_encode] */
      /* Let's fill table with codes later */



  	}
  	else if ( 0 == strcmp(argv[2], "-x"))
  	{
  		/*Going to extract*/
  		printf ("Extracting, please wait...\n");
  	}
  	else
  	{
 		printf ("Wrong argument (%s) specified\n", argv[2]);
   	}
  }
  /*
    for (count = 1; count < argc; count++)
	{
	  printf("argv[%d] = %s\n", count, argv[count]);
	}
    */
   return 0;
 }


/*

*/
#include <stdio.h>
#include <strings.h>

#define READBUF 256
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
      long freq_tbl[256] = {0};
      long fsize = 0L;
      long l = 0l;
      unsigned char buf[READBUF] = {0};
      int bytes_read = 0;
      int i = 0;

      FILE *f = fopen(argv[1], "r");
      if (NULL != f)
      {
      /*Read file and count data frequency*/
        printf ("Compressing %s, please wait...\n", argv[1]);
        if (-1 != fseek(f, 0L, SEEK_END))
        {
          fsize = ftell(f);
          rewind(f);
          printf ("File size: %ld bytes\n", fsize);
          /*!!!ADD NOTHING TO COMPRESS MESSAGE!!!*/

          fsize = 0L;

          do
          {
            bytes_read = fread(buf, 1, READBUF, f);
            fsize += bytes_read;

            for (i = 0; i < bytes_read; i++)
            {
              freq_tbl[buf[i]] += 1;
            }

          }
          while (0 == feof(f));

          printf ("Bytes processed: %ld\n", fsize);
          printf ("Frequency table:\n");
          
          for (i = 0; i < 256; i++)
          {
            printf ("Code: %.2X Freq: %ld\n", i, freq_tbl[i]);
          }
        }
      }
      else
      {
        printf ("Failed to open %s\n", argv[1]);
      }
  		/*Going to compress*/
  		printf ("Compressing, please wait...\n");
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


/*

*/
#include <stdio.h>
#include <limits.h>
#include <strings.h>
#include <time.h>


#define READ_SIZE 256
#define TREE_SIZE 512
struct node 
{
  unsigned int top;
  unsigned int left;
  unsigned int right;
  unsigned int code;
  unsigned int freq;
} tree[TREE_SIZE] = {0};

int main(int argc, char *argv[])
{
  int count;
  time_t time1, time2;
  char* c_time_string;

    /* Obtain current time. */
  time1 = time(NULL);

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
      long freq_tbl[256] = {0L};
      long fsize = 0L;
      long l = 0l;
      unsigned char buf[READ_SIZE] = {0};
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
            bytes_read = fread(buf, 1, READ_SIZE, f);
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
            printf ("%.2X: %ld ", i, freq_tbl[i]);
            if ((i+1)%8 == 0) printf("\n");
          }
          /*Scaling frequencies*/
          long scale = fsize / (long)(UINT_MAX)+256L;
          scale =  (scale > 0) ? scale : 1;


          printf ("Scaled frequency table %ld:\n", scale);
          /*Fill tree leaves*/
          int j = 0;

          for (i = 0; i < 256; i++)
          {
            if (freq_tbl[i] > 0) 
            {
              tree[j].code = i;
              tree[j].freq = (unsigned int)(freq_tbl[i]/scale)+1u;
              j++;

              printf ("%.2X: %d ", tree[j-1].code, tree[j-1].freq);

              if (j%8 == 0) printf("\n");
            }
          }
          /*Building tree*/
          int imin1, imin2;
          do 
          {
            /*1. Determine two leaves with smallest frequencies*/
            imin1 = -1;
            imin2 = -1;
            freq1 = 0;
            freq2 = 0;
            for (i = 0; i < tree_cnt; i++)
            {
              if ((-1 == imin1) && (-1 == imin2))
              {
               if (tree[i].freq > 0)
               {
                  imin1 = i;
                  freq1 = tree[i].freq;
               }

              }
              else if ((imin1 >= 0) && (-1 == imin2))
              {
               if (tree[i].freq > 0)
               {
                  imin2 = i;
                  freq2 = tree[i].freq;
               }

              }
              else
              {
                if (freq1 < tree[i].freq)
                {
                  freq2 = freq1;
                  imin2 = imin1;
                  imin1 = i;

                  freq1 = tree[i].freq;
                }
              }

/*2. If two elements found create new node*/
            }

          } while ();

        }
      }
      else
      {
        printf ("Failed to open %s\n", argv[1]);
      }
  		/*Going to compress*/
  		printf ("Compressing, please wait...\n");


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
   time2 = time(NULL);
   printf("Completed in %f sec\n", difftime(time2, time1));
   return 0;
 }


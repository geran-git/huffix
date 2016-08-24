/*

*/
#include <stdio.h>
#include <limits.h>
#include <strings.h>
#include <time.h>

#define RD_BUF_SIZE 256
#define WR_BUF_SIZE 256
/*Because bits of huffman code represented as 0 and 1 bytes*/
/*buffer for encoded data must be 8 times bigger than write buffer*/
#define HF_BUF_SIZE (WR_BUF_SIZE*8) 
#define TREE_SIZE 512

typedef uchar uchar;
typedef unsigned int uint;

struct node 
{
  uint up;
  uint left;
  uint right;
  uint code;
  uint freq;
} tree[TREE_SIZE] = {0};

/*Table of codes reserved for each symbol*/
/*Its size based on assumption that max code length could be 255 bits in worst case*/
/*Codes will be stored in byte sequences, not in bits. I suggest it would be easier*/
/*to manipulate bytes. Memory consumption not taken into account, so we have 255 bytes*/
/*for each bit and 1 for code length*/

uchar code_tbl[256][256] = {0};

/*bit sequence max length*/
uchar max_len = 0;

/*buffer for byte sequence representation of encoded data*/
uchar *bit_buf = NULL;

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
      uchar buf[READ_SIZE] = {0};
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

          printf("Bytes processed: %ld\n", fsize);
          printf("Frequency table:\n");
          
          for (i = 0; i < 256; i++)
          {
            printf("%.2X: %ld ", i, freq_tbl[i]);
            if ((i+1)%8 == 0) printf("\n");
          }
          /*Scaling frequencies*/
          long scale = fsize / (long)(UINT_MAX)+256L;
          scale =  (scale > 0) ? scale : 1;

          printf ("Scaled frequency table %ld:\n", scale);
          /*Fill tree leaves*/
          int tree_cnt = 0;

          for (i = 0; i < 256; i++)
          {
            if (freq_tbl[i] > 0) 
            {
              tree[tree_cnt].code = i;
              tree[tree_cnt].freq = (uint)(freq_tbl[i]/scale)+1u;
              tree_cnt++;

              printf("%.2X: %d ", tree[tree_cnt-1].code, tree[tree_cnt-1].freq);

              if (tree_cnt%8 == 0) printf("\n");
            }
          }
          printf("\n");

          /*Building tree*/
          int i1, i2;
          int freq1, freq2;

          do 
          {
            /*1. Determine two leaves with smallest frequencies*/
            i1 = -1;
            i2 = -1;
            freq1 = 0;
            freq2 = 0;
            /*i1 - index of node with smallest frequency (for current step)*/
            /*i2 - index of second node (updated with i1 when i1 changes)*/

            for (i = 0; i < tree_cnt; i++)
            {
              if (0 == tree[i].up)
              {
                /*Proceed searching nodes with smallest frequencies*/
                if ((0 <= i1) && (0 <= i2))
                {
                  if (freq1 >= tree[i].freq)
                  {
                    freq2 = freq1;
                    i2 = i1;
                    i1 = i;
                    freq1 = tree[i].freq;
                  }
                }
                else if ((-1 == i1) && (-1 == i2))
                {
                  /*Initial value - First node*/
                  if (tree[i].freq > 0)
                  {
                    i1 = i;
                    freq1 = tree[i].freq;
                  }
                }
                else if ((-1 == i2) && (0 <= i1))
                {
                  if (tree[i].freq > 0)
                  {
                    /*Initial value - Second node*/
                    if (tree[i].freq >= freq1)
                    {
                      i2 = i;
                      freq2 = tree[i].freq;
                    }
                    else
                    {
                      i2 = i1;
                      freq2 = freq1;
                      i1 = i;
                      freq1 = tree[i].freq;
                    }
                  }
                }
                else
                  printf("Tried to build a tree, but something went wrong...");                
              }
            }

            /*2. If two elements found create new node*/
            /*  and add it to the end of array*/
            if (0 <= i1 && 0 <= i2  )
            {
              tree[tree_cnt].freq = freq1 + freq2;
              tree[tree_cnt].left = i1;
              tree[tree_cnt].right = i2;
              tree[i1].up = tree_cnt;
              tree[i2].up = tree_cnt;

              printf("Left Node \t[%d] Freq %d Left %d Right %d Up %d\n", i1, tree[i1].freq, tree[i1].left, tree[i1].right, tree[i1].up);         
              printf("Right Node \t[%d] Freq %d Left %d Right %d Up %d\n", i2, tree[i2].freq, tree[i2].left, tree[i2].right, tree[i2].up);         
              printf("Up node \t[%d] Freq %d Left %d Right %d Up %d\n", tree_cnt, tree[tree_cnt].freq, tree[tree_cnt].left, tree[tree_cnt].right, tree[tree_cnt].up);
              tree_cnt++;
            }

            /*Only one free node found - tree completed */
          } while (-1 != i2);

          printf("\nTree completed, total nodes %d...\n", tree_cnt);

          

          /*Generate code table*/
          for (i = 0; i < tree_cnt; i++)
          {
            /*For each leave build code sequence*/
            if (0 == tree[i].left)
            {
              uchar len = 0;
              uint node = i;

              printf("Code \t[%d] Sequence ", tree[node].code);

              while(0 != tree[node].up)
              {
                code_tbl[tree[i].code][len+1] = (node == tree[tree[node].up].left) ? 0 : 1;
                node = tree[node].up;
                len++;
                printf("%d", code_tbl[tree[i].code][len]);
               /* if (255 == len) break;*/
              }

              code_tbl[tree[i].code][0] = len;

              if (max_len < len) max_len = len;

              printf(" %d\n", code_tbl[tree[i].code][0]);
              /*Bytes now stored in MSB->LSB order, but it would be more convinient*/
              /*to build bit stream if we put them in LSB->MSB order */  
              if (len >= 2)
              {
                uint j = 0;
                uchar temp = 0;
                for (j = 0; j < len/2; j++)
                {
                  temp = code_tbl[tree[i].code][j+1];
                  code_tbl[tree[i].code][j+1] = code_tbl[tree[i].code][len - j];
                  code_tbl[tree[i].code][len - j] = temp;
                }
              }
            }
          } /*Generate code table*/


          bit_buf = malloc(max_len * sizeof(uchar) * READ_SIZE);
          rewind(f);

          uint hf_buf_offset = 0;
          uchar symbol = 0;
          uchar code_len = 0;
          uchar *bit_code_ptr = NULL;

          do
          {
            bytes_read = fread(buf, 1, READ_SIZE, f);

            for (i = 0; i < bytes_read; i++)
            {
              symbol = buf[i];
              hcode_len = code_tbl[symbol][0];
              huf_code_ptr = &(code_tbl[symbol][1]);

              if (hbuf_offset + hcode_len < HF_BUF_SIZE)
              {
                memcpy(hf_buf + hf_buf_offset, hf_code_ptr, hf_code_len);
                hf_buf_offset += hf_code_len;
              }
              else
              {
                /*Move hbuf contents to write_buf*/
                /*No need to check buffer overflow cause wr_buf size*/
                /*depends on hf_buf size*/
                for (h = 0; h < hf_buf_offset; h += 8)
                {
                  wr_buf[h/8] = hf_buf[h    ] * 0x01 + 
                                hf_buf[h + 1] * 0x02 +
                                hf_buf[h + 2] * 0x04 +
                                hf_buf[h + 3] * 0x08 +
                                hf_buf[h + 4] * 0x10 +
                                hf_buf[h + 5] * 0x20 +
                                hf_buf[h + 6] * 0x40 +
                                hf_buf[h + 7] * 0x80; 
                }
                /**/
              }
            }
          }
          while (0 == feof(f));
            

        }/*if (-1 != fseek(f, 0L, SEEK_END))*/
      } /*if (NULL != f)*/
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


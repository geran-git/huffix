/*

*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#define RD_BUF_SIZE (256*1024)
#define WR_BUF_SIZE (256*1024)
/*Because bits of huffman code represented as 0 and 1 bytes*/
/*buffer for encoded data must be 8 times bigger than write buffer*/
#define HF_BUF_SIZE (WR_BUF_SIZE*8) 
#define TREE_SIZE 512

typedef unsigned char uchar;
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
//uchar *hf_buf = NULL;
uchar hf_buf[HF_BUF_SIZE];

int main(int argc, char *argv[])
{
  time_t time1, time2;
  char* c_time_string;

  uchar rd_buf[RD_BUF_SIZE] = {0};
  uchar wr_buf[WR_BUF_SIZE] = {0};

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
      int bytes_read = 0;
      int iCnt = 0;

      FILE *f_in = fopen(argv[1], "r");

      if (NULL != f_in)
      {
      /*Read file and count data frequency*/
        printf ("Compressing %s, please wait...\n", argv[1]);
        if (-1 != fseek(f_in, 0L, SEEK_END))
        {
          fsize = ftell(f_in);
          rewind(f_in);
          printf ("File size: %ld bytes\n", fsize);
          /*!!!ADD NOTHING TO COMPRESS MESSAGE!!!*/

          fsize = 0L;

          do
          {
            bytes_read = fread(rd_buf, 1, RD_BUF_SIZE, f_in);
            fsize += bytes_read;

            for (iCnt = 0; iCnt < bytes_read; iCnt++)
            {
              freq_tbl[rd_buf[iCnt]] += 1;
            }
          }
          while (0 == feof(f_in));

          printf("Bytes processed: %ld\n", fsize);
          printf("Frequency table:\n");
          
          for (iCnt = 0; iCnt < 256; iCnt++)
          {
            printf("%.2X: %ld ", iCnt, freq_tbl[iCnt]);
            if ((iCnt+1)%8 == 0) printf("\n");
          }
          /*Scaling frequencies*/
          long scale = fsize / (long)(UINT_MAX)+256L;
          scale =  (scale > 0) ? scale : 1;

          printf ("Scaled frequency table %ld:\n", scale);
          /*Fill tree leaves*/
          int tree_cnt = 0;

          for (iCnt = 0; iCnt < 256; iCnt++)
          {
            if (freq_tbl[iCnt] > 0) 
            {
              tree[tree_cnt].code = iCnt;
              tree[tree_cnt].freq = (uint)(freq_tbl[iCnt]/scale)+1u;
              tree_cnt++;

              printf("%.2X: %d ", tree[tree_cnt-1].code, tree[tree_cnt-1].freq);

              if (tree_cnt%8 == 0) printf("\n");
            }
          }
          printf("\n");

          /*Building tree*/
          int iNode1, iNode2;
          int iFreq1, iFreq2;

          do 
          {
            /*1. Determine two leaves with smallest frequencies*/
            iNode1 = -1;
            iNode2 = -1;
            iFreq1 = 0;
            iFreq2 = 0;
            /*iNode1 - index of node with smallest frequency (for current step)*/
            /*iNode2 - index of second node (updated with iNode1 when iNode1 changes)*/

            for (iCnt = 0; iCnt < tree_cnt; iCnt++)
            {
              if (0 == tree[iCnt].up)
              {
                /*Proceed searching nodes with smallest frequencies*/
                if ((0 <= iNode1) && (0 <= iNode2))
                {
                  if (iFreq1 >= tree[iCnt].freq)
                  {
                    iFreq2 = iFreq1;
                    iNode2 = iNode1;
                    iNode1 = iCnt;
                    iFreq1 = tree[iCnt].freq;
                  }
                }
                else if ((-1 == iNode1) && (-1 == iNode2))
                {
                  /*Initial value - First node*/
                  if (tree[iCnt].freq > 0)
                  {
                    iNode1 = iCnt;
                    iFreq1 = tree[iCnt].freq;
                  }
                }
                else if ((-1 == iNode2) && (0 <= iNode1))
                {
                  if (tree[iCnt].freq > 0)
                  {
                    /*Initial value - Second node*/
                    if (tree[iCnt].freq >= iFreq1)
                    {
                      iNode2 = iCnt;
                      iFreq2 = tree[iCnt].freq;
                    }
                    else
                    {
                      iNode2 = iNode1;
                      iFreq2 = iFreq1;
                      iNode1 = iCnt;
                      iFreq1 = tree[iCnt].freq;
                    }
                  }
                }
                else
                  printf("Tried to build a tree, but something went wrong...");                
              }
            }

            /*2. If two elements found create new node*/
            /*  and add it to the end of array*/
            if (0 <= iNode1 && 0 <= iNode2  )
            {
              tree[tree_cnt].freq = iFreq1 + iFreq2;
              tree[tree_cnt].left = iNode1;
              tree[tree_cnt].right = iNode2;
              tree[iNode1].up = tree_cnt;
              tree[iNode2].up = tree_cnt;

              printf("Left Node \t[%d] Freq %d Left %d Right %d Up %d\n", iNode1, tree[iNode1].freq, tree[iNode1].left, tree[iNode1].right, tree[iNode1].up);         
              printf("Right Node \t[%d] Freq %d Left %d Right %d Up %d\n", iNode2, tree[iNode2].freq, tree[iNode2].left, tree[iNode2].right, tree[iNode2].up);         
              printf("Up node \t[%d] Freq %d Left %d Right %d Up %d\n", tree_cnt, tree[tree_cnt].freq, tree[tree_cnt].left, tree[tree_cnt].right, tree[tree_cnt].up);
              tree_cnt++;
            }

            /*Only one free node found - tree completed */
          } while (-1 != iNode2);

          printf("\nTree completed, total nodes %d...\n", tree_cnt);

          

          /*Generate code table*/
          for (iCnt = 0; iCnt < tree_cnt; iCnt++)
          {
            /*For each leave build code sequence*/
            if (0 == tree[iCnt].left)
            {
              uchar len = 0;
              uint node = iCnt;

              printf("Code \t[%d] Sequence ", tree[node].code);

              while(0 != tree[node].up)
              {
                code_tbl[tree[iCnt].code][len+1] = (node == tree[tree[node].up].left) ? 0 : 1;
                node = tree[node].up;
                len++;
                printf("%d", code_tbl[tree[iCnt].code][len]);
               /* if (255 == len) break;*/
              }

              code_tbl[tree[iCnt].code][0] = len;

              if (max_len < len) max_len = len;

              printf(" %d\n", code_tbl[tree[iCnt].code][0]);
              /*Bytes now stored in MSB->LSB order, but it would be more convinient*/
              /*to build bit stream if we put them in LSB->MSB order */  
              if (len >= 2)
              {
                uint j = 0;
                uchar temp = 0;
                for (j = 0; j < len/2; j++)
                {
                  temp = code_tbl[tree[iCnt].code][j+1];
                  code_tbl[tree[iCnt].code][j+1] = code_tbl[tree[iCnt].code][len - j];
                  code_tbl[tree[iCnt].code][len - j] = temp;
                }
              }
            }
          } /*Generate code table*/


          //hf_buf = malloc(sizeof(uchar) * HF_BUF_SIZE);

          rewind(f_in);

          FILE *f_out = fopen(argv[3], "w");

          uint hf_buf_offset = 0;
          uint wr_buf_offset = 0;
          uchar symbol = 0;
          uchar hf_code_len = 0;
          uchar *hf_code_ptr = NULL;
          int h = 0;

          /*Saving header and code table*/
          wr_buf[0] = 'h';
          wr_buf[1] = 'f';
          wr_buf[2] = 'x';
          wr_buf[3] = 0xFF^'h';
          wr_buf[4] = 0xFF^'f';
          wr_buf[5] = 0xFF^'x';

          wr_buf_offset = 6;

          for (iCnt = 0; iCnt < 256; iCnt++)
          {
            hf_code_len = code_tbl[iCnt][0];
            hf_code_ptr = &(code_tbl[iCnt][1]);

            wr_buf[wr_buf_offset] = iCnt;
            wr_buf_offset++;
            wr_buf[wr_buf_offset] = hf_code_len;
            wr_buf_offset++;
            if (hf_code_len > 0)
            {
              memcpy(wr_buf+wr_buf_offset, hf_code_ptr, hf_code_len);
              wr_buf_offset += hf_code_len;
            }
          }
          fwrite(wr_buf, 1, wr_buf_offset, f_out);

          do
          {
            bytes_read = fread(rd_buf, 1, RD_BUF_SIZE, f_in);

            for (iCnt = 0; iCnt < bytes_read; iCnt++)
            {
              symbol = rd_buf[iCnt];
              hf_code_len = code_tbl[symbol][0];
              hf_code_ptr = &(code_tbl[symbol][1]);
              //printf("Code %.2X length %d ", symbol, hf_code_len);

              if (hf_buf_offset + hf_code_len < HF_BUF_SIZE)
              {
                //printf("Append byte %.2X\n", symbol);
                memcpy(hf_buf + hf_buf_offset, hf_code_ptr, hf_code_len);
                hf_buf_offset += hf_code_len;
              }
              else
              {
                /*Move hf_buf contents to wr_buf*/
                /*No need to check buffer overflow cause wr_buf size*/
                /*depends on hf_buf size*/
                //printf("Flush %d bytes \n", hf_buf_offset);
                for (h = 0; h < hf_buf_offset; h += 8)
                {
                  wr_buf[h/8] = (hf_buf[h    ] * 0x01 + 
                                 hf_buf[h + 1] * 0x02 +
                                 hf_buf[h + 2] * 0x04 +
                                 hf_buf[h + 3] * 0x08 +
                                 hf_buf[h + 4] * 0x10 +
                                 hf_buf[h + 5] * 0x20 +
                                 hf_buf[h + 6] * 0x40 +
                                 hf_buf[h + 7] * 0x80) & 0xFF; 
                }
                //printf("Bytes left %d\n", hf_buf_offset);
                /*if 7 (or less) bytes left in hf_buf moving them to the zero index*/
                /*and updating hf_buf_offset accordingly*/
                if (h > hf_buf_offset)
                {
                  memmove(hf_buf, hf_buf+(h-8), hf_buf_offset-(h-8));
                  hf_buf_offset -= (h-8);
                }
                else 
                  hf_buf_offset = 0;

                fwrite(wr_buf, 1, h/8, f_out);

                //printf("Bytes left %d\n", hf_buf_offset);
              }
            }
          }
          while (0 == feof(f_in));

          /*Check if data left in hf_buf, append bits to full byte if necessary*/
          if (hf_buf_offset > 0)
          {
                  wr_buf[0] = (hf_buf[0    ] * 0x01 + 
                                 hf_buf[1] * 0x02 +
                                 hf_buf[2] * 0x04 +
                                 hf_buf[3] * 0x08 +
                                 hf_buf[4] * 0x10 +
                                 hf_buf[5] * 0x20 +
                                 hf_buf[6] * 0x40 +
                                 hf_buf[7] * 0x80) & 0xFF;            
                fwrite(wr_buf, 1, 1, f_out);
          }


          fclose(f_in);
          fclose(f_out);
            

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

      int bytes_read = 0;
      int iCnt = 0;

      FILE *f_in = fopen(argv[1], "r");
      bytes_read = fread(rd_buf, 1, 6, f_in);
      if (6 == bytes_read)
      {
        if ('h' == rd_buf[0] &&
            'f' == rd_buf[1] &&
            'x' == rd_buf[2] &&
            'h'^0xFF == rd_buf[3] &&
            'f'^0xFF == rd_buf[4] &&
            'x'^0xFF == rd_buf[5])
        {
          /*Reading code table*/
          for (int iCnt = 0; i < 256; iCnt++)
          {
            if (2 == fread(rd_buf, 1, 2, f_in))
            {
              hf_code_len = rd_buf[1];
              if (hf_code_len > 0)
                if(hf_code_len = fread(rd_buf, 1, hf_code_len, f_in))
                  memcpy(&(code_tbl[symbol][1]), rd_buf[0], hf_code_len);
            }            
          }
          
          do
          {
            bytes_read = fread(rd_buf, 1, RD_BUF_SIZE, f_in);
            fsize += bytes_read;

            for (iCnt = 0; iCnt < bytes_read; iCnt++)
            {
              freq_tbl[rd_buf[iCnt]] += 1;
            }
          }
          while (0 == feof(f_in));

        }
        else
        {
          printf("Wrong archive file %s\n", argv[1]);    
        }
      }
      else
      {
        printf("Error reading file %s\n", argv[1]);
      }

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


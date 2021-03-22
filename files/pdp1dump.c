#include <stdio.h>
#include <stdint.h>
#include <errno.h>

/*
 * FIO-DEC to ASCII
 * note: not all characters map to ascii (down arrow, implies, right arrow...)
 *
 * CONSISE  FIO-DEC  CHARACTER                 CONSISE  FIO-DEC  CHARACTER
 * CODE     CODE     LOWER  UPPER              CODE     CODE     LOWER  UPPER
 * 00        00      Tape Feed                 41       241      j      J
 * --       100      Delete                    42       242      k      K
 * 00       200      Space                     43        43      l      L
 * 01        01      1      "                  44       244      m      M
 * 02        02      2      '                  45        45      n      N
 * 03       203      3      ~ (not)            46       246      o      O
 * 04        04      4      (implies)          47       247      p      P
 * 05       205      5      (or)               50       250      q      Q
 * 06       206      6      ^ (and)            51        51      r      R
 * 07        07      7      < (less than)      54        54      -      +
 * 10        10      8      > (greater than)   55       255      )      ]
 * 11       211      9      <up arrow>         56       256      <non-spacing overstrike
 * --        --                                                   and vertical bar>
 * --        13      Stop Code                 
 * 20        20      0      <right arrrow>     57        57      (      [
 * 21       221      /      ?                  61        61      a      A
 * 22       222      s      S                  62        62      b      B
 * 23        23      t      T                  63       263      c      C
 * 24       224      u      U                  64        64      d      D
 * 25        25      v      V                  65       265      e      E
 * 26        26      w      W                  66       266      f      F
 * 27       227      x      X                  67        67      g      G
 * 30       230      y      Y                  70        70      h      H
 * 31        31      z      Z                  71       271      i      I
 * 33       233      ,      =                  72       272      Lower Case
 * 34        --      <black>                   73        73      .      X <multiply>
 * 35        --      <red>                     74       274      Upper Case
 * 36       236      tab                       75        75      Backspace
 * 40        40      <non-spacing middle dot
 *                    and underline>           77       277      Carriage Return
 *
 */

// Primitive FIO-DEC concise code to ASCII characters, for generating
// octal and text dump.
char cvt[] = { ' ', '"', 0x27, '~',  0 ,  0 , '^', '<',    /* 00 - 07 */
               '>', '^',  0 , '\n' ,  0 ,  0 ,  0 ,  0 ,     /* 10 - 17 */
               '.', '?', 'S', 'T', 'U', 'V', 'W', 'X',     /* 20 - 27 */
               'Y', 'Z',  0 , '=',  0 ,  0 , '\t',  0 ,    /* 30 - 37 */
               '.', 'J', 'K', 'L', 'M', 'N', 'O', 'P',     /* 40 - 47 */
               'Q', 'R',  0 ,  0 , '+', ']', '|', '[',     /* 50 - 57 */
               '.', 'A', 'B', 'C', 'D', 'E', 'F', 'G',     /* 60 - 67 */
               'H', 'I',  0 , 'x',  0 ,  0 ,  0 , '\n'     /* 70 - 77 */
        };

char cvtl[] ={ ' ', '1', '2', '3', '4', '5', '6', '7',     /* 00 - 07 */
               '8', '9',  0 , '\n',  0 ,  0 ,  0 ,  0 ,     /* 10 - 17 */
               '0', '/', 's', 't', 'u', 'v', 'w', 'x',     /* 20 - 27 */
               'y', 'z',  0 , ',',  0 ,  0 , '\t',  0 ,    /* 30 - 37 */
               '_', 'j', 'k', 'l', 'm', 'n', 'o', 'p',     /* 40 - 47 */
               'q', 'r',  0 ,  0 , '-', ')', '.', '(',     /* 50 - 57 */
                0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g',     /* 60 - 67 */
               'h', 'i',  0 , '.',  0 ,  0 ,  0 , '\n'     /* 70 - 77 */
        };

// Fancy FIO-DEC concise code to UTF-8 strings, for generating
// accurate representations of the actual printed files on a
// Flexowriter or Soroban.
char * ucvtu[64] = { " ", "\"", "'", "~", "\xE2\x8A\x83", "\xE2\x88\xA8", "\xE2\x88\xA7", "<",    /* 00 - 07 */
               ">", "^", 0, 0, 0, 0, 0, 0,     /* 10 - 17 */
               "\xE2\x86\x92", "?", "S", "T", "U", "V", "W", "X",     /* 20 - 27 */
               "Y", "Z",  0 , "=",  0 ,  0 , "\t",  0 ,    /* 30 - 37 */
               "_", "J", "K", "L", "M", "N", "O", "P",     /* 40 - 47 */
               "Q", "R",  0 ,  0 , "+", "]", "|", "[",     /* 50 - 57 */
               0, "A", "B", "C", "D", "E", "F", "G",     /* 60 - 67 */
               "H", "I",  0 , "\xc3\x97",  0 ,  "\b" ,  0 , "\n"     /* 70 - 77 */
        };

char * ucvtl[64] ={ " ", "1", "2", "3", "4", "5", "6", "7",     /* 00 - 07 */
               "8", "9",  0 , 0,  0 ,  0 ,  0 ,  0 ,     /* 10 - 17 */
               "0", "/", "s", "t", "u", "v", "w", "x",     /* 20 - 27 */
               "y", "z",  0 , ",",  0 ,  0 , "\t",  0 ,    /* 30 - 37 */
               "\xC2\xB7", "j", "k", "l", "m", "n", "o", "p",     /* 40 - 47 */
               "q", "r",  0 ,  0 , "-", ")", "\xE2\x80\xBE", "(",     /* 50 - 57 */
                0 , "a", "b", "c", "d", "e", "f", "g",     /* 60 - 67 */
               "h", "i",  0 , ".",  0 ,  "\b" ,  0 , "\n"     /* 70 - 77 */
        };

struct block {
  uint32_t words[256];
};

struct file {
  char *name;
  int starting_block;
};

// File table created by manual examination of the directory
struct file files[26] = {
		       { "billiards (start at 200)", 0012},
		       { "dispak insert", 0433},
		       { "dispak source", 0053},
		       { "dispak simulator", 0155},
		       { "dispak float insert 4-27-70", 0516},
		       { "dispak float source 4-27-70", 0715},
		       { "snowflake", 0364},
		       { "mp2", 0571},
		       { "t.s.idiot", 0624},
		       { "dots", 0332},
		       { "cube", 0074},
		       { "plotter memo justified 3-19", 0554},
		       { "plotter display total 20-1mpr", 0153},
		       { "plotter display total 11-11mpr", 0171},
		       { "newa spherical alph", 0406},
		       { "newa score alph", 0232},
		       { "newa 2.3 alph", 0037},
		       { "newa auto alph", 0304},
		       { "newa 1 alph", 0040},
		       { "newa 2 alph", 0325},
		       { "newa 3 alph", 0423},
		       { "newa 4 alph", 0602},
		       { "newa old alph", 0013},
		       { "newa dec b", 0146},
		       { "float source", 0103},
		       { "float insert", 0137}
};

/* One entry for each logical block on the tape. 
 * High digit is 0 for free, 4 for text file, and 5 for binary file.
 * Low three digits are logical block number of next block in file, or
 * 0 if last block in file.
 * (This is data from tape, but I already had it extracted.)
 */
uint32_t links [512] = {
  0237071, 0654336, 0020000, 0227034, 0000766, 0000000, 0000000, 0000000,
  0000000, 0000000, 0500014, 0400015, 0500016, 0400017, 0500020, 0400021,
  0500022, 0400023, 0500024, 0400025, 0500026, 0400027, 0500030, 0400031,
  0500032, 0400033, 0500034, 0400035, 0505036, 0400046, 0500000, 0400041,
  0400042, 0400043, 0400044, 0400045, 0400175, 0400047, 0400050, 0400051,
  0400052, 0400122, 0400054, 0400055, 0400056, 0400057, 0400060, 0400061,
  0400062, 0400063, 0400064, 0400065, 0400066, 0400067, 0400070, 0400071,
  0400072, 0400073, 0400075, 0400100, 0400076, 0400077, 0400105, 0400101,
  0400102, 0400000, 0400104, 0400106, 0400000, 0400107, 0400110, 0400111,
  0400112, 0400113, 0400114, 0400115, 0400116, 0401117, 0400120, 0400121,
  0400646, 0400123, 0400124, 0400125, 0400126, 0400127, 0400130, 0400131,
  0400132, 0400133, 0400134, 0400135, 0400136, 0400000, 0400140, 0400141,
  0400142, 0400000, 0400144, 0400145, 0400611, 0400147, 0400150, 0400151,
  0400152, 0400165, 0400154, 0400156, 0400166, 0400157, 0400160, 0400161,
  0400162, 0400163, 0400164, 0400475, 0400167, 0400213, 0400170, 0400200,
  0400172, 0400173, 0400174, 0400235, 0400176, 0400177, 0400201, 0400207,
  0400202, 0400203, 0400204, 0404205, 0400206, 0400000, 0400210, 0400211,
  0400212, 0400214, 0400215, 0400230, 0400216, 0400217, 0400220, 0400221,
  0400222, 0400223, 0400224, 0400225, 0400226, 0400227, 0400236, 0400231,
  0400000, 0400233, 0400234, 0400241, 0400246, 0400237, 0400240, 0400266,
  0400242, 0400243, 0400244, 0400245, 0400000, 0400247, 0400250, 0400251,
  0400252, 0400253, 0400254, 0400255, 0400256, 0400257, 0400260, 0400261,
  0400262, 0400263, 0400264, 0400265, 0400271, 0400267, 0400346, 0400312,
  0400272, 0400273, 0400274, 0400275, 0400276, 0400277, 0400300, 0400301,
  0400302, 0400303, 0400473, 0400306, 0400520, 0400307, 0400310, 0400311,
  0400404, 0400313, 0400314, 0400315, 0400316, 0400317, 0400320, 0400321,
  0400322, 0400323, 0400324, 0400451, 0400326, 0400327, 0400330, 0400331,
  0400333, 0400340, 0400334, 0400335, 0400336, 0400337, 0400342, 0400341,
  0400353, 0400343, 0400344, 0400345, 0400360, 0400347, 0400350, 0400351,
  0400352, 0400354, 0400375, 0400355, 0400356, 0400357, 0400361, 0400374,
  0400362, 0400363, 0400000, 0400365, 0400366, 0400367, 0403370, 0400000,
  0404372, 0000000, 0400000, 0000000, 0400376, 0400377, 0400400, 0400401,
  0400402, 0400403, 0400407, 0400405, 0400453, 0400410, 0400431, 0400411,
  0400412, 0400413, 0400414, 0400415, 0400416, 0400417, 0400420, 0400421,
  0400422, 0400000, 0400424, 0400425, 0400426, 0400427, 0400430, 0400450,
  0400432, 0400474, 0400434, 0400435, 0400436, 0400000, 0400440, 0400000,
  0400442, 0400437, 0400444, 0400441, 0400446, 0400443, 0400466, 0400445,
  0400452, 0400447, 0400454, 0400455, 0400456, 0400457, 0400460, 0400461,
  0400462, 0400463, 0400464, 0400465, 0400567, 0400467, 0400470, 0400471,
  0400472, 0400476, 0400530, 0400305, 0400500, 0400477, 0400504, 0400501,
  0400502, 0400503, 0400507, 0400505, 0400506, 0400000, 0400724, 0400511,
  0400000, 0400671, 0400510, 0400000, 0400512, 0400513, 0400514, 0400515,
  0400522, 0400517, 0400524, 0400521, 0400526, 0400523, 0400532, 0400525,
  0400540, 0400527, 0400534, 0400531, 0400536, 0400533, 0400546, 0400535,
  0400542, 0400537, 0400544, 0400541, 0400561, 0400543, 0400550, 0400545,
  0400552, 0400547, 0400562, 0400551, 0400556, 0400553, 0400560, 0400555,
  0400717, 0400563, 0400564, 0400565, 0400566, 0400570, 0400651, 0400575,
  0400572, 0400573, 0400574, 0400613, 0400576, 0400577, 0400600, 0400601,
  0400650, 0400603, 0400604, 0400605, 0400606, 0400607, 0400610, 0400000,
  0400612, 0400270, 0400614, 0400615, 0400616, 0400617, 0400620, 0400621,
  0400622, 0400623, 0400627, 0400625, 0400626, 0400000, 0400630, 0400631,
  0400632, 0400633, 0400634, 0400635, 0400000, 0400637, 0400143, 0400641,
  0400636, 0400643, 0400640, 0400645, 0400642, 0400647, 0400644, 0400000,
  0400652, 0400653, 0400654, 0400655, 0400656, 0400657, 0400660, 0400661,
  0400662, 0400663, 0400664, 0400665, 0400666, 0400667, 0400670, 0400000,
  0400672, 0400673, 0400674, 0400675, 0400676, 0400700, 0400000, 0400557,
  0400702, 0400677, 0400704, 0400701, 0400706, 0400703, 0400710, 0400705,
  0400712, 0400707, 0400714, 0400711, 0400716, 0400713, 0400720, 0400721,
  0400722, 0400723, 0400725, 0400726, 0400736, 0400727, 0400730, 0400731,
  0400732, 0400733, 0400734, 0400735, 0400000, 0400737, 0400740, 0400741,
  0400742, 0400743, 0400744, 0400745, 0400746, 0400747, 0400750, 0400751,
  0400752, 0400753, 0400754, 0400755, 0400756, 0400757, 0400760, 0400761,
  0400000, 0400763, 0000000, 0400765, 0000000, 0400767, 0000000, 0400771,
  0000000, 0400773, 0000000, 0400775, 0000000, 0400777, 0000000, 0400000
};

int main(int argc, char *argv[])
{
  struct block tape[512];
  int mapping[512];
  int uc = 0;
  size_t count;
  int blockno = 0;
  int pbn;

  for (pbn = 0; pbn < 512; pbn++) {
    count = fread(&tape[pbn].words[0], sizeof(uint32_t), 256, stdin);
    if (count != 256) {
      printf("READ ERROR!\n");
      return 1;
    }
  }

  printf("tape read\n");

  int lbn = 0;
  // Build the forward chain of blocks
  for (pbn = 1; pbn < 512; pbn += 2) {
    mapping[lbn++] = pbn;
  }
  for (pbn = 510; pbn >= 0; pbn -=2) {
    mapping[lbn++] = pbn;
  }

  printf("logical block table initialized\n");

#if 0
  for (lbn = 0; lbn < 512; lbn++) {
    struct block* bp = &tape[mapping[lbn]];
    printf("logical block %d. 0%03d (physical %d)\n", lbn, lbn, mapping[lbn]);

    int j;
    for (j = 0; j < 32; j++) {
      int k;
      for (k = 0; k < 8; k++) {
	uint32_t word = bp->words[(j * 8) + k];
	
	printf("%06o ", word & 0x0003ffff);

      }

      for (k = 0; k < 8; k++) {
	uint32_t word = bp->words[(j * 8) + k];
	uint8_t c[3];
	int i;

	c[0] = (word & 0770000) >> 12;
	c[1] = (word & 0007700) >> 6;
	c[2] = (word & 0000077);

	for (i = 0; i < 3; i++) {
	  if (c[i] == 013) { putchar('\\'); continue; } /* cr */
	  if (c[i] == 034) continue;			/* RED */
	  if (c[i] == 035) continue;			/* BLACK */
	  if (c[i] == 036) { putchar('#'); continue; } /* tab */
	  if (c[i] == 072) { uc = 0; continue; }    /* lower case */
	  if (c[i] == 074) { uc = 1; continue; }    /* upper case */
	  if (c[i] == 077) { putchar('\\'); continue; } /* cr */
                
	  if (uc) {
	    if (cvt[c[i]] == 0) { putchar('*'); continue; }
	    putchar(cvt[c[i]]);
	  } else {
	    if (cvtl[c[i]] == 0) { putchar('*'); continue; }
	    putchar(cvtl[c[i]]);
	  }
	}
      }
      printf("\n");
    }
    blockno++;
    printf("\n\n");
  }
#endif

  int f;
  for (f = 0; f < 26; f++) {
    char text_filename[100];	/* path to write ascii text file at */
    char binary_filename[100];	/* path to write binary file at  */
    int bn;			/* logical block number */

    printf("file %s\n", files[f].name);
    sprintf(text_filename, "text/%s", files[f].name);
    FILE *tf = fopen(text_filename, "w");
    sprintf(binary_filename, "binary/%s", files[f].name);
    FILE *bf = fopen(binary_filename, "w");
    uc = 0;			/* all files start in lower case */

    int last_center_dot = 0;	/* was last character center dot */

    int last_stop = 0;		/* was last word read all stop code 0131313? */

    for (bn = files[f].starting_block; bn != 0; bn = (links[bn] & 0777)) {
      printf("block %04o\n", bn);

      struct block* bp = &tape[mapping[bn]];

      /* Write the binary version */
      fwrite(bp->words, 256, 1, bf);

      /* Write the UTF-8 version */
      int w;

      for (w = 0; w < 256; w++) {
	uint32_t word = bp->words[w];
	uint8_t c[3];
	int i;

	c[0] = (word & 0770000) >> 12;
	c[1] = (word & 0007700) >> 6;
	c[2] = (word & 0000077);

	// Stop code processing is complicated in Expensive
	// Typewriter. They always come three to a word. But two words
	// of stop code in a row are end-of-file. So this little state
	// machine detects that, and emits the FF-CR if the last
	// 0131313 wasn't EOF, and ends the file if there were two in
	// a row without the FF-CR. 
	if (word == 0131313) {
	  if (last_stop) {
	    break;
	  }
	  last_stop = 1;
	} else {
	  if (last_stop) {
	    fputs("\f\n", tf);
	  }
	  last_stop = 0;
	}

	for (i = 0; i < 3; i++) {
	  // Ignore ribbon color, even Unicode doesn't do colors.
	  if (c[i] == 034) continue;			/* RED */
	  if (c[i] == 035) continue;			/* BLACK */

	  // Already handled stop code
	  if (c[i] == 013) continue;

	  // Detect case shifting
	  if (c[i] == 072) {uc = 0; continue;}    /* lower case */
	  if (c[i] == 074) {uc = 1; continue;}    /* upper case */

	  // CR handling is fancy in Expensive Typewriter. Every text
	  // line starts with a new PDP-1 memory word. So the last
	  // word is padded with extra CR's (077) to finish off the
	  // word, but only the first CR is actually printed out.  I
	  // suspect that when a page was in ET's memory, there was an
	  // array of pointers to each line, so it had to start on a
	  // word boundary.
	  if (c[i] == 077) {
	    if (((i == 0) && (c[1] == 077) && (c[2] == 077)) ||
		((i == 1) && (c[2] == 077)) ||
		(i == 2)) {
	      fputs("\n", tf);
	      break;
	    }
	  }

	  // State machine for processing center dot, as we want to
	  // map center-dot followed by period or comma as colon or
	  // semicolon.
	  if (!uc && (c[i] == 040)) {
	    /* defer center dot */
	    last_center_dot = 1;
	    continue;
	  }

	  // Now complete processing for cases where previous
	  // character was non-spacing.
	  if (last_center_dot) {
	    last_center_dot = 0;
	    if (c[i] == 033) {
	      /* turn center-dot comma into semicolon */
	      fputc(';', tf);
	      continue;
	    } else if (c[i] == 073) {
	      /* turn center-dot period into colon */
	      fputc(':', tf);
	      continue;
	    } else {
	      /* Go ahead and emit it overstriking next character */
	    }
	  }

	  // Finally print the mapped upper or lower case
	  // character. Use * (not valid FIO-DEC character) for
	  // violations.
	  if (uc) {
	    if (ucvtu[c[i]] == 0) {
	      fputc('*', tf);
	      continue;
	    }
	    fputs(ucvtu[c[i]], tf);
	  } else {
	    if (ucvtl[c[i]] == 0) {
	      fputc('*', tf);
	      continue;
	    }
	    fputs(ucvtl[c[i]], tf);
	  }
	}
      }
    }
    fclose(bf);
    fclose(tf);
  }


#if 0
  int l;
  for (l = 0; l < 512; l++) {
    uc = 0;
    char filename[10];
    sprintf(filename, "%03o.txt", l);
    FILE *bf = fopen(filename, "w");
    struct block* bp = &tape[mapping[l]];
    int w;
    int last_nonspacing = 0;
    int last_center_dot = 0;
    for (w = 0; w < 256; w++) {
      uint32_t word = bp->words[w];
      uint8_t c[3];
      int i;

      c[0] = (word & 0770000) >> 12;
      c[1] = (word & 0007700) >> 6;
      c[2] = (word & 0000077);

      for (i = 0; i < 3; i++) {
	if (c[i] == 034) continue;			/* RED */
	if (c[i] == 035) continue;			/* BLACK */
	if (c[i] == 013) {fputs("\f\n", bf); continue;}
	if (c[i] == 072) {uc = 0; continue;}    /* lower case */
	if (c[i] == 074) {uc = 1; continue;}    /* upper case */
	if (!uc && (c[i] == 040)) {
	  /* defer center dot */
	  last_center_dot = 1;
	  continue;
	}
	if (last_nonspacing) {  
	  last_nonspacing = 0;
	  /* eat space after non-spacing character */
	  if (c[i] == 000) continue;
	}
	if (last_center_dot) {
	  last_center_dot = 0;
	  if (c[i] == 033) {
	    /* turn center-dot comma into semicolon */
	    fputc(';', bf);
	    continue;
	  }
	  if (c[i] == 073) {
	    /* turn center-dot period into colon */
	    fputc(':', bf);
	    continue;
	  }
	}
	
	if(uc){
	  if(ucvtu[c[i]] == 0) { fputc('*', bf); continue;}
	  fputs(ucvtu[c[i]], bf);
	}
	else{
	  if(ucvtl[c[i]] == 0) { fputc('*', bf); continue;}
	  fputs(ucvtl[c[i]], bf);
	}
	/* Remember if this was non-spacing character */
	if ((c[i] == 040) || (c[i] == 056)) {
	  last_nonspacing = 1;
	}
      }
    }
    fclose(bf);
    printf("wrote %s\n", filename);
  }
#endif

  return 0;
}


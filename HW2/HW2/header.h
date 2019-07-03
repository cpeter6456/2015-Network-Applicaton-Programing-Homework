#include <stdio.h>
#include <stdlib.h>

#define Bound 512

#define S_BUFF_SIZE 256
#define BUFF_SIZE 10240 /* size of the tmp buffer */
#define DIE(x) perror(x),exit(1)


typedef struct fileinfo File_Info;
struct fileinfo {
    char name[S_BUFF_SIZE];
   	int  f_size;
   	char huf_name[S_BUFF_SIZE];
   	int  huf_f_size;
   	int table_size;

};

typedef struct ch_table the_ch_table;
struct ch_table {

	/**UNsigned IMPORTANT**/
	unsigned char ch;
	int num;
	char * arr_ptr;
	int fre;
};


int fixed_huf(File_Info * finfo_ptr,int type,char * huf_name);


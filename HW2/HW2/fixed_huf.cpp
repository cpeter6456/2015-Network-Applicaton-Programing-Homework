#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <algorithm>
#include <iostream>

#include <queue>
#include "header.h"
#define S_BUFF_SIZE 256
#define BUFF_SIZE 10240 /* size of the tmp buffer */
#define Bound 512

using namespace std;
string toBinary(unsigned int n,int bits);
int bits_num( char *arr);
#define SORT
#ifdef SORT
struct the_byte_fre
{
    int fre;
    unsigned char inchar;

     /**after two years i don't know what the fucking thing i write**/
    //the_byte_fre(int a = 0):fre(a){}
};
bool compare ( the_byte_fre a, the_byte_fre b )
{   
    return a.inchar < b.inchar;
}
#endif // SORT

int fixed_huf(File_Info * finfo_ptr,int type,char * huf_name){

	the_byte_fre byte_fre[Bound];
    unsigned char ch;

    int num = 0;


	/**fixed compress**/
	if(type==0){
        FILE *f_c_debug = fopen( "clientDebugoutput.txt", "w");

        FILE *file = fopen( (*finfo_ptr).name, "rb");
        if(!file) {
            puts("can not read file");
            return 1;
        }

        char fname[S_BUFF_SIZE];
        strcpy(fname,(*finfo_ptr).name);
        int f_len = strlen(fname),position;
        position=f_len;
        /**find '.'  position **/
        while(position>0){
            if(fname[position]=='.')
                break;
            position--;
        }
        /**from 0 ~ **/
        printf("position  at %d \n",position);
        if(position<=0){perror("position<=0");exit(1);}



		rewind(file);
        while(!feof(file)) {
            fread(&ch, sizeof(char), 1, file);
            if(feof(file))break;
            //printf("READ (!!)ch = %4x int =%d\n",ch,ch);

            int temp=0;
            while(temp<Bound){
                if(temp<num && byte_fre[temp].inchar ==ch){
                    byte_fre[temp].fre++;
                    break;
                }
                /**IN Here "num" is mean diffenent char **/
                if(temp==num){
                    byte_fre[temp].inchar=ch;
                    byte_fre[temp].fre=1;

                    num++;
                    break;
                }
                temp++;
            }

        }/**End of read file and know num etc...**/

        int t=1,t2=0;/**t2 is need bit number ex. 010 is 3 bits**/
        while(t<num){t=t*2;t2++;}
        printf("T2(bits) is %d",t2);
        stable_sort(byte_fre,byte_fre+num,compare);

        /**the_ch_table is define in header.h**/

		the_ch_table the_table[S_BUFF_SIZE];


        /**IN here , create Codeword ex 01100**/
        for(int t=0;t<num;t++){
        	//printf("'%c'[%d]  \tfre = %d\t\t",byte_fre[t].inchar,byte_fre[t].inchar,byte_fre[t].fre);
        	string r=toBinary(t,t2);
        	//cout<<r<<endl;


        	/**make table**/
        	the_table[t].ch = byte_fre[t].inchar;
        	the_table[t].num=t;
        	the_table[t].arr_ptr = new  char[r.size() + 1];
        	copy(r.begin(), r.end(), the_table[t].arr_ptr);
			the_table[t].arr_ptr[r.size()] = '\0'; // don't forget the terminating 0
			the_table[t].fre=byte_fre[t].fre;

			//printf("'%c'[%d]  %s\t\t\n",the_table[t].ch,the_table[t].ch,the_table[t].arr_ptr);
        }

        printf("WHY?\n");


        /**creat new fucking name ???-code.???**/
        char a[100],a2[100];
        strncpy(a,fname,position);
        a[position]='\0';t=0;
        for(t=0;t<(f_len-position);t++)
        	a2[t]=fname[position+t];
       	a2[t]='\0';
        string r2="",r3(a),r4(a2),r5="";
        //cout << "r3 is "<< r3<<endl;
        //cout << "r4 is "<< r4<<endl;
        r2=r3+"-code"+r4;
        r5="-code-table.txt";
        //cout << "r2 is "<< r2<<endl;
        char fname_out[100],table_out[100];
        strcpy(fname_out, r2.c_str());      
        strcpy(table_out, r5.c_str());

        strcpy((*finfo_ptr).huf_name,fname_out);

        FILE *fout=fopen(fname_out,"wb");
        FILE *fout2=fopen(table_out,"wb");
        deque< char> myDQ;
        char out;

        long int huf_len=0;
        t=0;
        fprintf(fout2, "%d\n%d\n",num,(*finfo_ptr).f_size);
        while(t<num){
            fprintf(f_c_debug,"%d -> %s fre = %d\n",the_table[t].ch,the_table[t].arr_ptr,the_table[t].fre);
            fprintf(fout2, "%c %s\n",the_table[t].ch,the_table[t].arr_ptr);
            //cout<< the_table[t].table_ch<<"->"<<the_table[t].binary <<endl;


            huf_len+=the_table[t].fre * strlen((const char *)the_table[t].arr_ptr );
            t++;
        }
        fclose(f_c_debug);
        (*finfo_ptr).table_size = ftell(fout2);
        fclose(fout2);
        int file_length = ftell(file);  


        rewind(file);
        while(!feof(file)) {
            fread(&ch, sizeof(char), 1, file);
            if(feof(file))break;
            t=0; char bin[50];
            //printf("READ (!!)ch = %4x int =%d\n",ch,ch);
            while(t<num){
                if(the_table[t].ch == ch){
                     //the_table[t].ch should UNsigned
                    strcpy( bin,(const char*)the_table[t].arr_ptr);break;
                }
                t++;
            }
            int bin_len=strlen(bin);t=0;
            while(t<bin_len){

                myDQ.push_back(bin[t]);
                t++;
            }

            while(myDQ.size()>=8){
                out = 0;

                for(t=0;t<8;t++){
                    out=out<<1;
                    if(myDQ.front()=='1')
                        out=out+1;
                    myDQ.pop_front();
                }
                fwrite(&out,sizeof(char),sizeof(out),fout);

            }


        }
        int DQ_len=myDQ.size();
        if(DQ_len>0 && DQ_len<8){
            for(t=0;t<8-DQ_len;t++){
                myDQ.push_back('0');
            }
            out=0;
            for(t=0;t<8;t++){
                out=out<<1;
                if(myDQ.front()=='1')
                    out=out+1;
                myDQ.pop_front();
            }
            fwrite(&out,sizeof(char),sizeof(out),fout);
        }
        (*finfo_ptr).huf_f_size = ftell(fout);

        printf("\n壓縮後的大小%ld",ftell(fout));
        if(huf_len>0)printf("\n壓縮率 %f",file_length*8.0/(ftell(fout)*8.0));
        else
            printf("壓縮率 0");
        fclose(fout);
        printf("THE END\n");


	}/**END of fixed compress**/
	/**fixed Decompress**/
	else{



        char fname[S_BUFF_SIZE];
        strcpy(fname,(*finfo_ptr).huf_name);
        int f_len = strlen(fname),position;
        position=f_len;
        /**find '.'  position **/
        while(position>0){
            if(fname[position]=='.')
                break;
            position--;
        }
        /**from 0 ~ **/
        printf("position  at %d \n",position);
        if(position<=0){perror("position<=0");exit(1);}



        /*
        if(position<6){
            printf(" -code name error position<6\n");
            exit(1);
        }
        int flag=1;
        if(fname[position-5]=='-'&&fname[position-4]=='c'&&fname[position-3]=='o'&&fname[position-2]=='d'&&fname[position-1]=='e')
            flag=0;
        if(flag==1){
            printf(" -code name error Can't find '-code'\n");
            exit(1);
        }*/
        FILE *ftable=fopen("-code-table.txt","rb");
        int t=0;char ccr,ccr2;
        the_ch_table the_table[S_BUFF_SIZE];
        for(int i=0;i<S_BUFF_SIZE;i++){
            the_table[i].fre=0;

        }

        int diff_num,original_size;
        fscanf(ftable,"%d%c",&diff_num,&ccr2);
        fscanf(ftable,"%d%c",&original_size,&ccr2);
        printf("diff num %d , original size %d\n",diff_num,original_size);

        while(diff_num>t){
            the_table[t].arr_ptr = new  char[19];
            fscanf(ftable,"%c%c%s%c",&(the_table[t].ch),&ccr,the_table[t].arr_ptr,&ccr2);

            
            t++;
        }
        int bits_number = bits_num(the_table[0].arr_ptr);
        printf("IT IS all %d bits\n",bits_number);



        //cout <<fname<<endl;
        FILE *fcode=fopen( fname,"rb");
        FILE *fout=fopen( (*finfo_ptr).name,"wb");
        printf("fname = %s\n",fname);
        if(fcode==NULL){perror("fcode");exit(1);}

        /**USE UNSIGNED CHAR IS VERY IMPORTANT**/
        //char ch2;
        int temp_fsize=0;
        deque<char> myDQ;
        //cout <<"IN HWERE"<<endl;
        string rt="";
        int wtf=0,readchar=0;
        FILE *f_s_debug = fopen( "serverDebugoutput.txt", "w");
        
        rewind(fcode);
        while(!feof(fcode)){
            wtf=0;
            //cout <<"IN HWERE"<<endl;
            fread(&wtf, sizeof(unsigned char), 1, fcode);
            readchar++;
            if(feof(fcode)||temp_fsize>=original_size){break;}
            //printf("Read wtf = !%d!\n",wtf);
            string r = toBinary((unsigned int)(wtf),8);
            
           // if(temp_fsize>7480)
               // cout <<"temp_fsize"<<temp_fsize<<"\t r(read from file bin) is "<< r<<endl;
            char temp_bin_ch[10];
            strcpy(temp_bin_ch,r.c_str());
            int t=0;
            /**read byte an convert to binar , to DQ**/
            while(t<8){

                myDQ.push_back(temp_bin_ch[t]);
                t++;
            }
            //char out;
            if(temp_fsize==7487){
                   // cout <<"7487(out) rt is "<< rt<<endl;
                }
            /** should myDQ.size()>0 , not > 8 => it will cause lost end char **/
            /** because it is work pre char  at next char input **/
            while(myDQ.size()>0){
                //cout <<"myDQ.front()"<< myDQ.front()<<endl;
                 if(temp_fsize==7487){
                    //cout <<"7487A rt is "<< rt<<"myDQ.size() is "<<myDQ.size()<<endl;
                }
                rt=rt+myDQ.front();
               
                myDQ.pop_front();
                 //cout <<"rt is "<< rt<<endl;
                if(temp_fsize==7487){
                    //cout <<"7487B rt is "<< rt<<"myDQ.size() is "<<myDQ.size()<<endl;
                }
                for(int i=0;i<diff_num;i++){
                    //if(feof(fcode)||temp_fsize>=original_size){printf("BREAK1\n"); break;}
                  
                    if(strncmp(the_table[i].arr_ptr,rt.c_str(),bits_number) == 0){
                       // cout <<"FIND ::rt is "<< rt<<endl;
                        the_table[i].fre++;

                        fprintf(fout,"%c",the_table[i].ch);rt="";
                        temp_fsize++;
                        //printf("temp_fsize = %d\n",temp_fsize );
                        if(feof(fcode)||temp_fsize>=original_size){break;}
                    }
                }
                if(feof(fcode)||temp_fsize>=original_size){ break;}
            }
            //cout <<"rt is "<< rt<<endl;

        }
        for(int i=0;i<diff_num;i++)
            fprintf(f_s_debug,"server %d -> %s fre = %d\n",the_table[i].ch,the_table[i].arr_ptr,the_table[i].fre);
        
        //printf("TTTemp_fsize IS %d\n",temp_fsize);
        //printf("readchar IS %d\n",readchar);
        fclose(fcode);
        fclose(fout);
        cout <<"END of fixed\n"<<endl;

        fclose(f_s_debug);

	}/**END of fixed Uncompress**/


	return 0;
}
string toBinary(unsigned int n,int bits)
{
       // cout << "N is "<<n<<endl;
    string r;
    if(n==0)r="0";
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}

    while((unsigned)bits>r.length())r="0"+r;
    // cout << "R is"<<r<<endl;
    return r;
}
int bits_num( char *arr){
    string r(arr);

    return r.size();
}

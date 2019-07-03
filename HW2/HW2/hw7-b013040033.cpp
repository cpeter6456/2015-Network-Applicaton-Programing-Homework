#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <queue>
#define Bound 300
/*
Author：雷皓博 (B013040033)
Date：2013/12/9
Purpose：習題 7 以Huffman 演算法製作一個檔案壓縮與解壓縮視窗軟體 (binary tree之應用)

*/
using namespace std;

#define SORT
#ifdef SORT
struct the_byte_fre
{
    int fre;
    unsigned char inchar;
    the_byte_fre(int a = 0):fre(a){}
};
bool compare ( the_byte_fre a, the_byte_fre b )
{   if(a.fre==b.fre)return (unsigned char)a.inchar < (unsigned char)b.inchar;
    return a.fre < b.fre;
}
#endif // SORT
struct node{
    int no;
    int node_fre,binary;
    unsigned char node_ch_array[Bound];
    node *father,*left,*right;
};
struct PQcompare{
    bool operator() ( node* a, node* b )
    {   if(a->node_fre==b->node_fre){

            if(strcmp((const char*)(a->node_ch_array),(const char*)(b->node_ch_array)) < 0)return 0;
            else return 1;

        }
        return a->node_fre > b->node_fre;
    }
};
struct table{
    unsigned char table_ch;
    unsigned char binary[50];
    int fre;

};
static int number,header_len,max_now,t_now;

table the_table[Bound];
node *globe_top,*globe_temp;
node *get_node(void);
node *combine_two_node(void);
void rec_find_binary(node *temp,char binary[],int num,int maxsize);
void rec_puts_binary(node *temp,int num,int maxsize,FILE *fout);
void rec_table_tree(node *temp);
priority_queue<node*,vector<node*>,PQcompare> PQ;

int need_char(int a);
char chtable[2048];
int main(int argc, char *argv[]){
    header_len=0;
     FILE *file = fopen(argv[1], "rb");
    if(!file) {
        puts("can not read file");
        return 1;
    }
    if(feof(file))printf("IS NULL \n");
    the_byte_fre byte_fre[Bound];
    unsigned char ch;
    char ar[1024];
    int cou = 0,num = 0,file_length=0;
    strcpy(ar,argv[1]);

    /**Here is uncompress huf file**/
   if(ar[strlen(ar)-3]=='h' && ar[strlen(ar)-2]=='u' && ar[strlen(ar)-1] == 'f'){   //need uncompress


            char ccr,pre[30];
            unsigned char ch2,ctemp=0,ctemp2=0;

            fscanf(file,"%s%c",pre,&ccr);

            int p=0;//printf("fuck2 ");
            while(1){
                    //printf("fuck3 ");
                fscanf(file,"%c",&chtable[p]);
            //printf("fuck4 ");
                if(chtable[p]=='0' )
                    p++;
                else if(chtable[p]=='1'){
                        //printf("fuck5 ");
                    p++;
                    fread(&chtable[p], sizeof(char), 1, file);
                    //printf("!!!%4x!!!\n",chtable[p]);
                    //fscanf(file,"%c",&chtable[p]);
                    p++;
                }
                else{

                    //printf("Break fuck");
                    break;
                }
            }
            chtable[p]='\0';
            p=p-1;
            int int_pre=atoi(pre);
            //printf("%d %d %s \n",int_pre,int_now,rate);
            int tt=0;
            //printf("len = %d\n",p);
            while(tt<p){
                //printf("%3x ",chtable[tt]);
                tt++;
            }
            node *top=NULL,*temp2=NULL;

            top=get_node();
            temp2=top;
            t_now=1;
            if(max_now>2)top->binary=0;
            else{
                    top->node_ch_array[0]=chtable[1];
                    top->binary=1;
            }
            max_now=p;
            //printf("%d\n",max_now);
            //rec_table_tree(top);
            //printf("%d",max_now);
            ar[strlen(ar)-3]='d' , ar[strlen(ar)-2]='e' ,ar[strlen(ar)-1] = 'c';

            FILE *fout2=fopen(ar,"wb");       //OUTPUT 的檔案都叫這名子 要自己改檔案格式
            while(t_now<max_now){               //USE WHILE AND TABLE TO BUILD FUCKING TREE
                //printf("t_now= %d\n",t_now);
                if(chtable[t_now]=='0'){
                    if( temp2->left == NULL){
                        temp2->left=get_node();
                        temp2->left->father=temp2;
                        temp2->left->binary=0;t_now++;
                        temp2=temp2->left;
                        //printf("0 a\n");
                    }
                    else if( temp2->right == NULL){
                        temp2->right=get_node();
                        temp2->right->father=temp2;
                        temp2->right->binary=0;t_now++;
                        temp2=temp2->right;
                        //printf("0 1b\n");
                    }
                    else{
                        //printf("0 1c\n");
                        temp2=temp2->father;

                    }
                }
                else if(chtable[t_now]=='1'){
                     if( temp2->left == NULL){
                       // printf("1 a\n");
                        temp2->left=get_node();
                        temp2->left->father=temp2;
                        temp2->left->binary=1;t_now++;
                        temp2->left->node_ch_array[0]=chtable[t_now];t_now++;
                    }
                    else if( temp2->right == NULL){
                           // printf("1 b\n");

                        temp2->right=get_node();
                        temp2->right->father=temp2;
                        temp2->right->binary=1;t_now++;
                        temp2->right->node_ch_array[0]=chtable[t_now];t_now++;
                    }
                    else if(temp2->left != NULL&&temp2->right != NULL){
                           // printf("1 c\n");
                        temp2=temp2->father;

                    }

                }


            }

            temp2=top;
            while(temp2!=NULL){
           // cout<< temp2->binary <<endl;
            temp2=temp2->left ;
            }
            temp2=top;
            int int_binary=0,loc_dir=-1;
            long long int pre_size=0;
            //printf("IN HERE\n");
            while(!feof(file)){

                if(int_pre<=pre_size)break;
                fread(&ch2, sizeof(char), 1, file);
                if(feof(file))break;
                ctemp=ch2;
                for(int n=0;n<8;n++){

                    if(ctemp>=128)
                        loc_dir=1;
                    else
                        loc_dir=0;
                    //printf("THE DIR %d\n",loc_dir);
                    ctemp=ctemp<<1;
                    if(loc_dir==0){
                        if(max_now>2)temp2=temp2->left;
                        //printf("%d\n",temp2->binary);
                        if(temp2->binary==1){

                            fprintf(fout2,"%c",temp2->node_ch_array[0]);
                            // printf("\nTHE\n");
                            temp2=top;
                            pre_size+=8;
                            if(int_pre<=pre_size)break;
                           // printf("THE33\n");
                        }
                    }
                    else{
                        temp2=temp2->right;
                       // printf("WHY %d\n",temp2==NULL);
                        if(temp2->binary==1){
                               // printf("THE22 \n");

                            fprintf(fout2,"%c",temp2->node_ch_array[0]);
                            temp2=top;
                            pre_size+=8;
                            if(int_pre<=pre_size)break;
                        }
                       // printf("THE\n");
                    }

                }
                //printf("\n");
                if(feof(file))break;
                //printf("%2x",ch2);
            }



   }
   else{
            rewind(file);
            while(!feof(file)) {
                fread(&ch, sizeof(char), 1, file);
                if(feof(file))break;
                //printf("%4x",ch);

                int temp=0;
                while(temp<Bound){
                    if(temp<num && byte_fre[temp].inchar ==ch){
                        byte_fre[temp].fre++;
                        break;
                    }

                    if(temp==num){
                        byte_fre[temp].inchar=ch;
                        byte_fre[temp].fre=1;

                        num++;
                        break;
                    }
                    temp++;
                }

            }
            stable_sort(byte_fre,byte_fre+num,compare);

            int t=0;

            while(t<num){
                //printf("%4x-> %d",byte_fre[t].inchar,byte_fre[t].fre );
                t++;
            }
            t=0;
            if(num==0){

            }
            t=0;
            while(t<num){                                  //num >0 will do
                node *temp=get_node();
                temp->node_fre=byte_fre[t].fre;

                strcpy((char *)temp->node_ch_array,"");
                temp->node_ch_array[0]=byte_fre[t].inchar;
                PQ.push(temp);
                t++;
            }

            t=0;
            printf("Have %d different char\n",num);

            t=0;
            while (t  < num-1 ){                        //num >1 will do
                PQ.push(combine_two_node());
                t++;
            }

            char binary2[50];
            t=0;
            while(t<50){
                binary2[t]='\0';
            t++;
            }
            number=0;
            //printf("FFFFFFFFFFF\n");
            if(num>1)
                rec_find_binary(PQ.top(),binary2,0,num);
            if(num==1){
                the_table[number].table_ch=PQ.top()->node_ch_array[0];
                the_table[number].binary[0]='0';the_table[number].binary[1]='\0';
                the_table[number].fre=PQ.top()->node_fre;
            }

            t=0;
            long int huf_len=0;
            while(t<num){
                printf("%2x -> %s\n",the_table[t].table_ch,the_table[t].binary);
                //cout<< the_table[t].table_ch<<"->"<<the_table[t].binary <<endl;


                huf_len+=the_table[t].fre * strlen((const char *)the_table[t].binary );
                t++;
            }
            file_length = ftell(file);      //原本要壓縮的檔案大小
            //cout<< file_length*8<<"bit" <<endl;
            //cout<< huf_len<< "bit"<<endl;
            //cout<< "要補"<<8-huf_len%8<<"個0" <<endl;
            //if(huf_len>0)cout<< "壓縮率"<< file_length*8.0/huf_len<<endl;
            //printf("HERE/n");
            rewind(file);

            ar[strlen(ar)-3]='h' , ar[strlen(ar)-2]='u' ,ar[strlen(ar)-1] = 'f';
            FILE *fout=fopen(ar,"wb");

            number=0;
            fprintf(fout,"%d ",file_length*8);

            //fprintf(fout,"%d ",huf_len);
            //if(huf_len>0)fprintf(fout,"%.3f ",file_length*8.0/huf_len);
            //else fprintf(fout,"0 ");

            if(huf_len>0){
                    rec_puts_binary(PQ.top(),0,num,fout);
                    fprintf(fout," ");
                    deque<char> myDQ;
                    char out;
                    while(!feof(file)) {
                        fread(&ch, sizeof(char), 1, file);
                        if(feof(file))break;
                        t=0;char bin[50];

                        while(t<num){
                            if(the_table[t].table_ch == ch){
                                strcpy( bin,(const char*)the_table[t].binary);break;
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
                    printf("\n壓縮後的大小%d",ftell(fout));
                    if(huf_len>0)printf("\n壓縮率 %f",file_length*8.0/(ftell(fout)*8.0));
                    else
                        printf("壓縮率 0");
                    fclose(fout);
                    //printf("THE END\n");

            }

   }

    fclose(file);
    return 0;
}
int need_char(int a){
    int t=0;
    while(a>0){
        a=a/10;
        t++;
    }
    return t;
}
static void use_binary_and_tree_find_char(int dir,FILE *fout){


}
void rec_puts_binary(node *temp,int num,int maxsize,FILE *fout){


    if( temp->left==NULL && temp->right==NULL ){

        fprintf(fout,"1%c",the_table[number].table_ch);
        header_len=header_len+16;
        number++;
    }
    else{
        fprintf(fout,"0");
        header_len=header_len+8;
    }
    if(number==maxsize)
        return;

    if(temp->left != NULL)
        rec_puts_binary(temp->left,num+1,maxsize,fout);
    if(temp->right != NULL)
        rec_puts_binary(temp->right,num+1,maxsize,fout);
}
void rec_find_binary(node *temp,char binary2[50],int num,int maxsize){
    if(temp->binary==1 && temp!=PQ.top())
        binary2[num-1]='1';
    else if( temp->binary==0 && temp!=PQ.top() )
        binary2[num-1]='0';
    int t=num;
    while(t < 50){
        binary2[t]='\0';
        t++;
    }
    if( temp->left==NULL && temp->right==NULL ){
        the_table[number].table_ch=temp->node_ch_array[0];
        strcpy( (char *)( the_table[number].binary ) ,(const char*) binary2 );
        the_table[number].fre=temp->node_fre;
        number++;
    }
    if(number==maxsize)
        return;

    if(temp->left != NULL)
        rec_find_binary(temp->left,binary2,num+1,maxsize);
    if(temp->right != NULL)
        rec_find_binary(temp->right,binary2,num+1,maxsize);

}
node *get_node(){
    node *temp;
    temp=(node*)malloc(sizeof(node));
    temp->binary=temp->node_fre=0;
    temp->father=temp->right=temp->left=NULL;
    int t=0;
    while(t<Bound){
        temp->node_ch_array[t]='\0';
    t++;
    }
    return temp;
}
node *combine_two_node(void){
    node * temp=get_node();
    node *a,*b;
    a=PQ.top();
    PQ.pop();

    b=PQ.top();
    PQ.pop();
    if(a->node_fre==b->node_fre && strcmp((const char*) a->node_ch_array,(const char*) b->node_ch_array)>0 ){
        node *tt=a;
        a=b;
        b=tt;
    }


    temp->node_fre=a->node_fre + b->node_fre;
    strcpy((char *) temp->node_ch_array,(const char*) a->node_ch_array);
    strcat((char *) temp->node_ch_array,(const char*) b->node_ch_array);
    temp->left=a;
    temp->right=b;
    a->binary=0;b->binary=1;



    return temp;
}

#define SIZE 10
#define max_filesystem 4
#define max_OpenFile 5
#define mkfs 1
#define use 2
#define cp 3
#include<iostream>
#include<string>
#include<fstream>
#include<unistd.h>
#include<wait.h>
#include<cstdlib>
#include<sstream>
#include<string.h>
using namespace std;
struct inode{
	string filename;
	int size,file_offset;
	char permission;
	void inodeinfo(void){
		cout<<"INODE DESCRIPTION:  "<<endl;
		cout<<"Filename :"<<filename<<" "<<"Permission: "<<permission<<" "<<"Size :"<<size<<" "<<"File offset :"<<file_offset<<endl;
	}
};
typedef struct inode Inode;
struct superblock{
        int bs,ts,free,occupied,start_offset,openfile;
        Inode I[max_OpenFile];
        string Drive;
        void metainfo(void){
		cout<<"SUPER BLOCK DESCRIPTION : "<<endl;
                cout<<"Drive :"<<Drive<<" Block size :"<<bs<<" Total size :"<<ts<<" Free :"<<free<<" Occupied :"<<occupied<<" Start offset: "<<start_offset<<" Total open file : "<<openfile<<endl;
		for(int i=0;i<openfile;i++){
			I[i].inodeinfo();
        	}
	}
};
typedef struct superblock Super;
int filesystem=0;
Super S[max_filesystem];
int check(string s){
	string temp="";
	for(int i=0;s[i]!=' ';i++){
		temp+=s[i];
	}
	if(temp=="mkfs")
		return 1;
	if(temp=="use")
		return 2;
	if(temp=="cp")
		return 3;
}
void display(char *c[]){
	for(int i=0;i<5;i++){
		cout<<c[i]<<endl;
	}
}
int parse(string s[],string str){
	int k=0;
	string temp="";
        for(int i=0;i<str.length();i++){
                if(str[i]!=' '){
                        temp+=str[i];
                }
                else{
                        s[k]=temp;
                        temp="";
                        k++;
                }
        }
        s[k]=temp;
	return k+1;
}

int mycreate(int drive,string name,char per,int size_file){
	Inode temp;
	temp.filename=name;
	temp.permission=per;
	temp.size=size_file;
	temp.file_offset=S[drive].start_offset;
	S[drive].start_offset+=size_file;
	S[drive].free-=1;
	S[drive].occupied+=1;
	S[drive].I[S[drive].openfile]=temp;
	S[drive].openfile++;
	S[drive].metainfo();
}
main(){
	int flag,pid;
	char *argv[SIZE];
	string s,name,block,size,cmd,str[SIZE];
	while(true){
		cout<<"myfs>";
		getline(cin,s);
		flag=check(s);
		if(flag==mkfs){
			name=s.substr(5,7);	
			block=s.substr(13,3);
			size=s.substr(17,2);
			cmd="dd if=/dev/zero of=";
			cmd+=name+" bs=";
			cmd+=block+" count=";
			int count=1000000*atoi((char*)(size.c_str()));
			count/=512;
			stringstream s1;
			s1<<count;
			cmd+=s1.str();
			int argc;
			argc=parse(str,cmd);
			for(int i=0;i<argc;i++){
				argv[i]=const_cast<char*>(str[i].c_str());
			}
			argv[argc]=NULL;
			if((pid=fork())==0){
				execvp(argv[0],argv);
				cout<<"CAN NOT EXECUTE"<<endl;
				exit(0);
			}
			else{
				wait(&pid);
			}
			S[filesystem].Drive=name;
			S[filesystem].bs=atoi((char*)(block.c_str()));
			S[filesystem].ts=1000000*atoi((char*)(size.c_str()));
			S[filesystem].free=(S[filesystem].ts/S[filesystem].bs);
			S[filesystem].occupied=0;
			S[filesystem].start_offset=sizeof(S[filesystem]);
			S[filesystem].metainfo();
			S[filesystem].openfile=0;
			filesystem++;
		}
// --------------------------------------- MKFS ----------------------------------------------------------------------
		if(flag==use){
			string dest=s.substr(15,2);
			string source=s.substr(4,7);
			char *argv2[]={(char*)"mv",(char*)(source.c_str()),(char*)(dest.c_str()),(char*)0};
			if((pid=fork())==0){
				execvp(argv2[0],argv2);
				cout<<"CAN NOT EXECUTE"<<endl;
				exit(0);
			}
			else{
				wait(&pid);
			}
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==source){
					S[i].Drive=dest;
					cout<<"Drive name has been changed to "<<S[i].Drive<<endl;
					break;
				}
			}
		}
// ---------------------------------------- USE ------------------------------------------------------------------------
		if(flag==cp){
			string source=s.substr(3,7);
			string d=s.substr(11,2);
			string dest=s.substr(13,8);
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==d){
					mycreate(i,dest,'r',10);
					cout<<i+1<<"th system partition super block has been updated"<<endl;
				}
			}
		}
// ------------------------------------ CP ------------------------------------------------------------------------------
	}
}				

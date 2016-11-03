#define SIZE 10
#define max_filesystem 4
#define max_OpenFile 5
#define MAX_FILE_SIZE 100
#define mkfs 1
#define use 2
#define cp 3
#define ls 4
#define rm 5
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
	if(temp=="ls")
		return 4;
	if(temp=="exit")
		exit(0);
	if(temp=="rm")
		return 5;
}
bool check2(string s){
	string temp="";
	for(int i=3;i<s.length();i++){
		if(s[i]!=' ')
			temp+=s[i];
		else{
			if(temp[1]==':')
				return true;
			else
				return false;
		}
	}
}
/*
void display(char *c[]){
	for(int i=0;i<5;i++){
		cout<<c[i]<<endl;
	}
}
*/
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

void mycreate(int drive,string name,char per){
	Inode temp;
	temp.filename=name;
	temp.permission=per;
	temp.size=0;
	temp.file_offset=S[drive].start_offset;
	S[drive].free-=1;
	S[drive].occupied+=1;
	S[drive].I[S[drive].openfile]=temp;
	S[drive].openfile++;
	S[drive].metainfo();
}
void display(char b[]){
        for(int i=0;b[i]!='\0';i++)
                cout<<b[i];
        cout<<endl;
}
void byte_write(int drive,char buff[],int inode_index){
	FILE *fd1=NULL;
	Inode temp=S[drive].I[inode_index];
	string name=S[drive].Drive;
	if((fd1=fopen((char*)(name.c_str()),"rw+"))!=NULL){
		cout<<"successfully open"<<endl;
	}
	else{
		cout<<"CAN NOT OPEN FOR WRITING"<<endl;
		exit(0);
	}
	int offset=S[drive].start_offset;
	cout<<"the Offset : "<<offset<<endl;
	if(fseek(fd1,offset,SEEK_CUR)!=0){
		cout<<"fseek failed"<<endl;
		exit(0);
	}
	int written_bytes;
	char buf[strlen(buff)];
	for(int i=0;buff[i]!='\0';i++)
		buf[i]=buff[i];
	written_bytes=fwrite(buf,sizeof(char),sizeof(buf),fd1);
	cout<<"written bytes : "<<written_bytes<<endl;
	temp.file_offset=S[drive].start_offset;
	S[drive].start_offset+=sizeof(buf);
	cout<<"Written Successfully "<<endl;
	temp.size=sizeof(buf);
	cout<<"SIZE OF THE FILE : "<<temp.size<<endl;
	S[drive].I[inode_index]=temp;	
	fclose(fd1);
}
void mywrite(string name,int drive,char buff[]){
	int length=strlen(buff);
	Inode temp;
	for(int i=0;i<S[drive].openfile;i++){
		temp=S[drive].I[i];
		if(temp.filename==name){
//			if(length<=temp.size){
				if(temp.permission=='w'||temp.permission=='a'){
					cout<<"CAN BE WRITTEN"<<endl;
					byte_write(drive,buff,i);
					display(buff);
				}
				else{
					cout<<"permission denied"<<endl;
				}
				break;
//			}
//			else{
//				cout<<"THERE IS NOT ENOUGHT SPACE ALLOCATED FOR THE FILE "<<length<<" ---- > "<<temp.size<<endl;
//				break;
//			}
		}
	}
}

void delete_file(int drive,string name){
	Inode temp;
	int start,Size,index;
	for(int i=0;i<S[drive].openfile;i++){
		temp=S[drive].I[i];
		if(temp.filename==name){
			index=i;
			start=temp.file_offset;
			Size=temp.size;
			cout<<"Start offset : "<<start<<" Size of the file : "<<name<<" : "<<Size<<endl;
			char buf[Size];
			memset(buf,0,sizeof(buf));
			cout<<"size of buff : "<<sizeof(buf)<<endl;
			FILE *fd=NULL;
			string d=S[drive].Drive;
			fd=fopen((char*)(d.c_str()),"rw+");
			if(fd==NULL){
				cout<<"CAN NOT OPEN FILE SYSTEM WHILE DELETING"<<endl;
				exit(0);
			}
			fseek(fd,start,SEEK_CUR);
			int byte=fwrite(buf,sizeof(char),sizeof(buf),fd);	
			cout<<byte<<" has been removed"<<endl;
			for(int j=i;j<S[drive].openfile-1;j++){
				S[drive].I[j]=S[drive].I[j+1];
			}
			S[drive].openfile--;
			S[drive].free++;
			S[drive].occupied--;
			cout<<"SUCCESFULLY UPDATED SYSTEM"<<endl;
			fclose(fd);
			break;
		}
	}
}
void myread(char buf[],int drive,string name){
	Inode temp;
	for(int i=0;i<S[drive].openfile;i++){
		temp=S[drive].I[i];
		if(temp.filename==name){
			int start=temp.file_offset;
//			int Size=temp.size;
//			buf=new char[Size];
			FILE *fd=NULL;
			string D=S[drive].Drive;
			fd=fopen((char*)(D.c_str()),"r");
			fseek(fd,start,SEEK_CUR);
			fread(buf,sizeof(char),sizeof(buf),fd);
			fclose(fd);
			cout<<"COPIED TEXT FROM SOURCE : ------------ "<<endl;
//			display(buf);
//			return buf;
		}
	}
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
			if(!check2(s)){
				string source=s.substr(3,7);
				string d=s.substr(11,2);
				string dest=s.substr(13,9);
				for(int i=0;i<filesystem;i++){
					if(S[i].Drive==d){
						mycreate(i,dest,'w');
						cout<<i+1<<"th system partition super block has been updated"<<endl;
						char buff[512];
						memset(buff,0,sizeof(buff));
						FILE *fd=NULL;
						fd=fopen((char*)(source.c_str()),"r");
						if(fd==NULL){
							cout<<"can not open source file"<<endl;
							exit(0);
						}
						int b=fread(buff,sizeof(char),sizeof(buff),fd);
//						cout<<b<<endl;
						fclose(fd);
						mywrite(dest,i,buff);
					}
				}
			}
			else{
//				cout<<"SECOND cp"<<endl;
				string source_drive=s.substr(3,2);
				string source_file=s.substr(5,9);
				string dest_drive=s.substr(15,2);
				string dest_file=s.substr(17,9);
//				cout<<"source drive : "<<source_drive<<"\n Souce_File : "<<source_file<<"\nDest Drive : "<<dest_drive<<"\ndest_file : "<<dest_file<<endl;
				for(int i=0;i<filesystem;i++){
					if(S[i].Drive==source_drive){
						for(int j=0;i<S[j].openfile;j++){
							Inode temp=S[i].I[j];
							if(temp.filename==name){
								int Size_file=temp.size;
								break;
							}
						}
						char buffer[SIZE];
						myread(buffer,i,source_file);
						display(buffer);
						for(int k=0;k<filesystem;k++){
							if(S[k].Drive==dest_drive){
								mycreate(k,dest_file,'w');
								mywrite(dest_file,k,buffer);
								break;
							}
						}
						break;
					}
				}
			}
		}
// ------------------------------------ CP ------------------------------------------------------------------------------
		
		if(flag==ls){
			string D=s.substr(3,2);
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==D){
					for(int j=0;j<S[i].openfile;j++){
						Inode temp=S[i].I[j];
						temp.inodeinfo();
					}
				}
			}
		}
		if(flag==rm){
			string target_drive=s.substr(3,2);
			string target_file=s.substr(5,9);
			cout<<target_drive<<" "<<target_file<<endl;
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==target_drive){
					delete_file(i,target_file);
					break;
				}
			}
		}
	}
}				

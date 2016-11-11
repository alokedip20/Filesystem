#define SIZE 10
#define max_filesystem 4
#define max_OpenFile 5
#define MAX_FILE_SIZE 100
#define mkfs 1
#define use 2
#define cp 3
#define ls 4
#define rm 5
#define info 6
#define mv 8
#include<iostream>
#include<string>
#include<fstream>
#include<unistd.h>
#include<wait.h>
#include<cstdlib>
#include<sstream>
#include<string.h>
#include<cmath>
using namespace std;
char *valid_dir_name[]={(char*)"C:",(char*)"D:",(char*)"E:",(char*)"F:"};
struct inode{
	char filename[15];
	int size,file_offset;
	char permission;
};
typedef struct inode Inode;
void inodeinfo(Inode I){
        cout<<"INODE DESCRIPTION : "<<endl;
        cout<<"File name : "<<I.filename<<endl;
        cout<<"File Size : "<<I.size<<endl;
        cout<<"File Offset : "<<I.file_offset<<endl;
        cout<<"File Permission : "<<I.permission<<endl;
}
struct superblock{
        int bs,ts,free,occupied,start_offset,openfile;
        Inode I[max_OpenFile];
        char Drive[8];
        void metainfo(void){
		cout<<"SUPER BLOCK DESCRIPTION : "<<endl;
                cout<<"Drive :"<<Drive<<" Block size :"<<bs<<" Total size :"<<ts<<" Free :"<<free<<" Occupied :"<<occupied<<" Start offset: "<<start_offset<<" Total open file : "<<openfile<<endl;
		for(int i=0;i<openfile;i++){
			inodeinfo(I[i]);
        	}
	}
};
typedef struct superblock Super;
int filesystem=0;
Super S[max_filesystem];
void store(Super S[],int drive_index){
	FILE *fd=NULL;
	string D(S[drive_index].Drive);
	fd=fopen((char*)(D.c_str()),"r+");
	if(fd==NULL){
		cout<<"CAN NOT INITIALIZE SUPER BLOCK"<<endl;
		exit(0);
	}
	fseek(fd,0,SEEK_SET);
	cout<<sizeof(S[drive_index].bs)<<endl;
	int w=fwrite(&S[drive_index].bs,sizeof(S[drive_index].bs),1,fd);
	fwrite(&S[drive_index].ts,sizeof(S[drive_index].ts),1,fd);
	fwrite(&S[drive_index].free,sizeof(S[drive_index].free),1,fd);
	fwrite(&S[drive_index].occupied,sizeof(S[drive_index].occupied),1,fd);
	fwrite(&S[drive_index].start_offset,sizeof(S[drive_index].start_offset),1,fd);
	fwrite(&S[drive_index].openfile,sizeof(S[drive_index].openfile),1,fd);
	fwrite(&S[drive_index].Drive,2,1,fd);
	fwrite(S[drive_index].I,sizeof(Inode),S[drive_index].openfile,fd);
	if(drive_index==0){
		fwrite(&filesystem,sizeof(filesystem),1,fd);
	}	
	cout<<" size of drive : ----------------- "<<sizeof(S[drive_index].Drive)<<endl;
	cout<<" size os inode : ----------------- "<<sizeof(Inode)<<endl;
	cout<<"INFORMATION THAT ARE BEING STORED : "<<endl;
	cout<<"Drive : "<<S[drive_index].Drive<<endl;
	cout<<"Block Size : "<<S[drive_index].bs<<endl;
	cout<<"Total Size : "<<S[drive_index].ts<<endl;
	cout<<"Free : "<<S[drive_index].free<<endl;
	cout<<"Occupied : "<<S[drive_index].occupied<<endl;
	cout<<"Start Offset : "<<S[drive_index].start_offset<<endl;
	cout<<"No of Open File : "<<S[drive_index].openfile<<endl;
	cout<<"Total Partition Created so Far : "<<filesystem<<endl;
	fclose(fd);
}
void initialize(Super S[],char *drive,int drive_index){
	FILE *fd=NULL;
        fd=fopen(drive,"r");
        if(fd==NULL){
                cout<<"CAN NOT INITIALIZE SUPER BLOCK"<<endl;
                exit(0);
        }
        fseek(fd,0,SEEK_SET);
        cout<<sizeof(S[drive_index].bs)<<endl;
        int w=fread(&S[drive_index].bs,sizeof(S[drive_index].bs),1,fd);
        fread(&S[drive_index].ts,sizeof(S[drive_index].ts),1,fd);
        fread(&S[drive_index].free,sizeof(S[drive_index].free),1,fd);
        fread(&S[drive_index].occupied,sizeof(S[drive_index].occupied),1,fd);
        fread(&S[drive_index].start_offset,sizeof(S[drive_index].start_offset),1,fd);
        fread(&S[drive_index].openfile,sizeof(S[drive_index].openfile),1,fd);
        fread(&S[drive_index].Drive,2,1,fd);
        fread(S[drive_index].I,sizeof(Inode),S[drive_index].openfile,fd);
	if(drive_index==0){
		fread(&filesystem,sizeof(filesystem),1,fd);
	}
	cout<<"READING FROM SYSTEM :"<<endl;
        cout<<"Block Size : "<<S[drive_index].bs<<endl;
        cout<<"Total Size : "<<S[drive_index].ts<<endl;
        cout<<"Free : "<<S[drive_index].free<<endl;
        cout<<"Occupied : "<<S[drive_index].occupied<<endl;
        cout<<"Start Offset : "<<S[drive_index].start_offset<<endl;
        cout<<"No of Open File : "<<S[drive_index].openfile<<endl;
	cout<<"Drive : "<<S[drive_index].Drive<<endl;
	cout<<"Total Partition : "<<filesystem<<endl;
        fclose(fd);
}
int check(string s){
	string temp="";
	for(int i=0;s[i]!=' '&&i<s.length();i++){
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
		return 9;
	if(temp=="rm")
		return 5;
	if(temp=="info")
		return 6;
	if(temp=="clear")
		return 7;
	if(temp=="mv")
		return 8;
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
bool check3(string s){
//	string temp=s.substr(15,2);
	string temp,A[2];
	int X=0;
	for(int j=3;j<s.length();j++){
		if(s[j]!=' ')
			A[X]+=s[j];
		else
			X++;
	}
	temp=A[1].substr(0,2);
	cout<<temp<<endl;
	for(int i=0;i<filesystem;i++)
		if(S[i].Drive==temp)
			return true;
	return false;
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

void mycreate(int drive,string name,char per){
	if(S[drive].openfile<max_OpenFile-1){
		Inode temp;
		strcpy(temp.filename,(char*)(name.c_str()));
		temp.permission=per;
		temp.size=0;
		temp.file_offset=S[drive].start_offset;
		S[drive].I[S[drive].openfile]=temp;
		S[drive].openfile++;
		S[drive].metainfo();
	}
	else{
		cout<<"THE DISK IS CAPABLE OF STORING ONLY "<<max_OpenFile<<" BUT FILE LIMIT EXCEED"<<endl;
		for(int i=0;i<filesystem;i++){
			store(S,i);
		}
		exit(0);
	}
}
void display(char b[],int size){
        for(int i=0;i<size;i++)
                cout<<b[i];
        cout<<endl;
}
void byte_write(int drive,char buff[],int data_size,int inode_index){
	FILE *fd1=NULL;
	Inode temp=S[drive].I[inode_index];
	string name=S[drive].Drive;
	if((fd1=fopen((char*)(name.c_str()),"rw+"))!=NULL){
		cout<<"successfully open"<<endl;
	}
	else{
		cout<<"CAN NOT OPEN FOR WRITING"<<endl;
		for(int i=0;i<filesystem;i++){
			store(S,i);
		}
		exit(0);
	}
	int offset=S[drive].start_offset;
	cout<<"the Offset : "<<offset<<endl;
	if(fseek(fd1,offset,SEEK_SET)!=0){
		cout<<"fseek failed"<<endl;
		exit(0);
	}
	int written_bytes;
	written_bytes=fwrite(buff,sizeof(char),data_size,fd1);
	cout<<"written bytes : "<<written_bytes<<endl;
	temp.file_offset=S[drive].start_offset;
	S[drive].start_offset+=data_size;
	cout<<"Written Successfully "<<endl;
	temp.size=data_size;
	cout<<"SIZE OF THE FILE : "<<temp.size<<endl;
	S[drive].I[inode_index]=temp;	
	fclose(fd1);
}
void mywrite(string name,int drive,int data_size,char buff[]){
	Inode temp;
	for(int i=0;i<S[drive].openfile;i++){
		temp=S[drive].I[i];
		if(temp.filename==name){
				if(temp.permission=='w'||temp.permission=='a'){
					cout<<"CAN BE WRITTEN"<<endl;
					cout<<" TOTAL BYTE TO BE WRITTEN : "<<data_size<<"\n\n\n";
					byte_write(drive,buff,data_size,i);
				}
				else{
					cout<<"permission denied"<<endl;
				}
				break;
		}
	}
	S[drive].free-=ceil((double)data_size/S[drive].bs);
	S[drive].occupied+=ceil((double)data_size/S[drive].bs);
}

void delete_file(int drive,string name){
	Inode temp;
	int start,Size,FILE_SIZE,index,start_ref;
	for(int i=0;i<S[drive].openfile;i++){
		temp=S[drive].I[i];
		if(temp.filename==name){
			index=i;
			start_ref=temp.file_offset;
			Size=FILE_SIZE=temp.size;
			cout<<"Start offset from where it will be written : "<<start_ref<<endl;
			cout<<"Size of the file -- "<<name<<" =  "<<Size<<endl;
			char buf[Size];
			memset(buf,0,sizeof(buf));
			FILE *fd=NULL;
			string d=S[drive].Drive;
			fd=fopen((char*)(d.c_str()),"rw+");
			if(fd==NULL){
				cout<<"CAN NOT OPEN FILE SYSTEM WHILE DELETING"<<endl;
				exit(0);
			}
			fseek(fd,start_ref,SEEK_SET);
			int byte=fwrite(buf,sizeof(char),sizeof(buf),fd);
			fclose(fd);
			int start;	
			cout<<byte<<" HAS BEEN REMOVED NOW GOING TO SHIFT ALL FILES"<<endl;
			fd=fopen((char*)(d.c_str()),"rw+");
			for(int j=i;j<S[drive].openfile-1;j++){
				S[drive].I[j]=S[drive].I[j+1];
				temp=S[drive].I[j];
				start=temp.file_offset;
				cout<<"\n\n STARTING OFFSET OF THE FILE THAT TO BE SHIFTED : "<<start<<endl;
				Size=temp.size;
				cout<<" TOTAL BYTES TO BE SHIFTED : "<<Size<<endl;
				char Buf[Size],replace[Size];
				fseek(fd,start,SEEK_SET);
				int Read=fread(Buf,sizeof(char),sizeof(Buf),fd);
				memset(replace,0,sizeof(replace));
				fseek(fd,start,SEEK_SET);
				fwrite(replace,sizeof(char),sizeof(replace),fd);
				cout<<"SEEKED BUFFER : -------------------- "<<Read<<" ";
				fseek(fd,start_ref,SEEK_SET);
				cout<<"THE NEXT FILE WILL BE STARTED FROM START_REF : "<<start_ref<<endl;
				fwrite(Buf,sizeof(char),sizeof(Buf),fd);
				temp.file_offset=start_ref;
				S[drive].I[j]=temp;
				start_ref+=Size;	
			}
			S[drive].openfile--;
			S[drive].free+=ceil((double)FILE_SIZE/S[drive].bs);
			S[drive].occupied-=ceil((double)FILE_SIZE/S[drive].bs);
			S[drive].start_offset=start_ref;
			cout<<"SUCCESFULLY UPDATED SYSTEM"<<endl;
			fclose(fd);
			break;
		}
	}
}
void myread(char buf[],int drive,int data_size,string name){
	Inode temp;
	for(int i=0;i<S[drive].openfile;i++){
		temp=S[drive].I[i];
		if(temp.filename==name){
			int start=temp.file_offset;
			FILE *fd=NULL;
			string D=S[drive].Drive;
			fd=fopen((char*)(D.c_str()),"r");
			fseek(fd,start,SEEK_CUR);
			int b=fread(buf,sizeof(char),data_size,fd);
			fclose(fd);
			cout<<" COPIED TEXT FROM SOURCE : ------------ READ --------> "<<b<<endl;
		}
	}
}
void mount(char *dir[]){
	FILE *fd=NULL;
	int flag=0;
	for(int i=0;i<max_filesystem;i++){
		string drive=dir[i];
		fd=fopen((char*)(drive.c_str()),"r");
		if(fd!=NULL){
			flag++;
			initialize(S,(char*)(drive.c_str()),i);
			cout<<"SUCCESSFULLY MOUNTED : "<<drive<<endl;
			fclose(fd);
		}
	}
	cout<<"-------------------- MOUNTED : ----------------------------"<<flag<<endl;
}
ifstream::pos_type FileSize(const char* filename){
	ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg(); 
}
main(){
	mount(valid_dir_name);
	int flag,pid;
	char *argv[SIZE];
	string s,name,block,size,cmd,str[SIZE];
	while(true){
		cout<<"myfs>";
		getline(cin,s);
		flag=check(s);
// ------------------------------------------- mkfs ------------------------------------------------------------
		if(flag==mkfs){
/*
			name=s.substr(5,7);	
			block=s.substr(13,4);

			if(block[block.length()-1]==' '){
				block=s.substr(13,3);
				size=s.substr(17,2);
			}
			else
				size=s.substr(18,2);
*/
			string A[3];
			int X=0;
			for(int j=5;j<s.length();j++){
				if(s[j]!=' ')
					A[X]+=s[j];
				else
					X++;
			}
			name=A[0];
			block=A[1];
			size=A[2].substr(0,A[2].length()-2);
			cout<<"BLOCK  : "<<block<<endl;
			cmd="dd if=/dev/zero of=";
			cmd+=name+" bs="+block+" count=";
//			cmd+=block+" count=";
			int count=1000000*atoi((char*)(size.c_str()));
			count/=atoi((char*)(block.c_str()));
			cout<<"COUNT : "<<count<<endl;
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
			strcpy(S[filesystem].Drive,(char*)(name.c_str()));
			S[filesystem].bs=atoi((char*)(block.c_str()));
			S[filesystem].ts=1000000*atoi((char*)(size.c_str()));
			S[filesystem].free=ceil((double)S[filesystem].ts/S[filesystem].bs);
			S[filesystem].occupied=0;
			S[filesystem].start_offset=sizeof(S[filesystem]);
			S[filesystem].metainfo();
			S[filesystem].openfile=0;
			filesystem++;
		}
// --------------------------------------- use ----------------------------------------------------------------------
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
					strcpy(S[i].Drive,(char*)(dest.c_str()));
					cout<<"Drive name has been changed to "<<S[i].Drive<<endl;
					break;
				}
			}
		}
// ---------------------------------------- cp ------------------------------------------------------------------------
		if(flag==cp){
			if(!check2(s)){
				cout<<"cp osfile3 C:dest "<<endl;
				string A[3];
				int X=0;
				for(int j=3;j<s.length();j++){
					if(s[j]!=' '){
						A[X]+=s[j];
					}
					else{
						X++;
					}
				}
				string source=A[0];	
				string d=A[1].substr(0,2);
				string dest=A[1].substr(2,A[1].length()-1);
				for(int i=0;i<filesystem;i++){
					if(S[i].Drive==d){
						mycreate(i,dest,'w');
						int actual_size=FileSize((char*)(source.c_str()));
						int no_of_block=ceil((double)actual_size/S[i].bs);
						char BUFF[actual_size];
						memset(BUFF,0,sizeof(BUFF));
						FILE *fd=NULL;
                                                fd=fopen((char*)(source.c_str()),"r");
                                                if(fd==NULL){
                                                        cout<<"can not open source file"<<endl;
                                                        exit(0);
                                                }
						string s1="";
						char buff[S[i].bs];
						int Start=0;
						int R=0;
						
						for(int pass=0;pass<no_of_block;pass++){
							fseek(fd,Start,SEEK_SET);
							int b=fread(buff,sizeof(char),sizeof(buff),fd);
							for(int P=0;P<b;P++)
								BUFF[R++]=buff[P];
							
							Start+=b;
						}
						fclose(fd);
						mywrite(dest,i,actual_size,BUFF);
						cout<<"BLOCK : "<<no_of_block<<endl;
						cout<<i+1<<"th system partition super block has been updated"<<endl;
					}
				}
			}
			else{
				if(check3(s)){
					cout<<"cp C:source D:dest "<<endl;
					string A[4];
					int X=0;
					for(int j=3;j<s.length();j++){
						if(s[j]!=' '){
							A[X]+=s[j];
						}
						else
							X++;
					}
					string source_drive=A[0].substr(0,2);
					string source_file=A[0].substr(2,A[0].length()-1);
					string dest_drive=A[1].substr(0,2);
					string dest_file=A[1].substr(2,A[1].length()-1);
					int Size_file;
					for(int i=0;i<filesystem;i++){
						if(S[i].Drive==source_drive){
							for(int j=0;j<S[i].openfile;j++){
								Inode temp=S[i].I[j];
								if(temp.filename==source_file){
									Size_file=temp.size;
										break;
								}
							}
							cout<<"ACTUAL SIZE OF THE FILE : "<<Size_file<<"\n\n";
							char buffer[Size_file];
							for(int x=0;x<Size_file;x++)
								buffer[x]='.';
							cout<<"ALLOCATED SIZE : "<<sizeof(buffer)<<"\n\n\n";
							myread(buffer,i,Size_file,source_file);
							for(int k=0;k<filesystem;k++){
								if(S[k].Drive==dest_drive){
									mycreate(k,dest_file,'w');
									mywrite(dest_file,k,Size_file,buffer);
									break;
								}
							}
							break;
						}
					}	
				}
				else{	
					cout<<"cp C:source testfile "<<endl;
					string A[2];
					int X=0;
					for(int j=3;j<s.length();j++){
						if(s[j]!=' ')
							A[X]+=s[j];
						else
							X++;
					}
					string source_drive=A[0].substr(0,2);
					string source_file=A[0].substr(2,A[0].length()-1);
					string dest_file=A[1];
					cout<<source_drive<<"/"<<source_file<<" -----> "<<dest_file<<endl;
					int source_drive_index,dest_drive_index;
					for(int i=0;i<filesystem;i++){
						if(S[i].Drive==source_drive)
							source_drive_index=i;		
					}
					for(int i=0;i<S[source_drive_index].openfile;i++){
						Inode temp=S[source_drive_index].I[i];
						if(temp.filename==source_file){
							char FinalBuffer[temp.size];
							myread(FinalBuffer,source_drive_index,temp.size,source_file);
							FILE* FD=fopen((char*)(dest_file.c_str()),"w");
							fwrite(FinalBuffer,sizeof(char),temp.size,FD);
							fclose(FD);
							break;
						}
					}
				}
			}
		}
// ------------------------------------ ls ------------------------------------------------------------------------------
		
		if(flag==ls){
			string D=s.substr(3,2);
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==D){
					for(int j=0;j<S[i].openfile;j++){
						Inode temp=S[i].I[j];
						inodeinfo(temp);
					}
				}
			}
		}
// -------------------------------------- rm  ---------------------------------------------------------------------------
		if(flag==rm){
			string A[1];
			int X=0;
			for(int j=3;j<s.length();j++){
				if(s[j]!=' ')
					A[X]+=s[j];
				else
					X++;
			}
			string target_drive=A[0].substr(0,2);
			string target_file=A[0].substr(2,A[0].length()-1);
			cout<<target_drive<<" "<<target_file<<endl;
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==target_drive){
					delete_file(i,target_file);
					break;
				}
			}
		}
// ----------------------------------- info ---------------------------------------------------------------------------
		if(flag==info){
			cout<<"THE REQUIRED INFO : "<<endl;
			for(int m=0;m<filesystem;m++)
				S[m].metainfo();
		}
// ---------------------------------- clear ---------------------------------------------------------------------
		if(flag==7){
			char *cmd[]={(char*)"clear",0};
			int p;
			if((p=fork())==0)
				execvp(cmd[0],cmd);
			else
				wait(&p);
		}
// ------------------------------------------- mv ----------------------------------------------------------------
		if(flag==mv){
			cout<<"MOVE OPERATION WILL BE DONE"<<endl;
			string A[4];
                        int X=0;
                        for(int j=3;j<s.length();j++){
                        if(s[j]!=' '){
                        	A[X]+=s[j];
                        }
                        else
                        	X++;
                        }
                        string source_drive=A[0].substr(0,2);
                       	string source_file=A[0].substr(2,A[0].length()-1);
                        string dest_drive=A[1].substr(0,2);
                       	string dest_file=A[1].substr(2,A[1].length()-1);
			int source_drive_index,dest_drive_index;
			for(int i=0;i<filesystem;i++){
				if(S[i].Drive==source_drive){
					source_drive_index=i;
				}
				if(S[i].Drive==dest_drive){
					dest_drive_index=i;
				}
			}
			int TotalSize;
			for(int i=0;i<S[source_drive_index].openfile;i++){
				Inode temp=S[source_drive_index].I[i];
				if(temp.filename==source_file){
					TotalSize=temp.size;
					char Buffer[TotalSize];
					myread(Buffer,source_drive_index,TotalSize,source_file);
					delete_file(source_drive_index,source_file);
					mycreate(dest_drive_index,dest_file,'w');
					mywrite(dest_file,dest_drive_index,TotalSize,Buffer);
					break;
				}
			}
		}
// ----------------------------------------------- exit ------------------------------------------------------------
		if(flag==9){
			for(int i=0;i<filesystem;i++){
				store(S,i);
			}
			exit(0);
		}
	}
}				

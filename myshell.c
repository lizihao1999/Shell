#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

char error_message[30] = "An error has occurred\n"; 

void myPrint(char *msg)
{
  write(STDOUT_FILENO, msg, strlen(msg));
  fflush(stdout);
}


void fexit() {
	exit(0);
}

void fcd(char* s) {
	if (s==NULL) {
		const char* t=getenv("HOME");
		int f=chdir(t);
		if (f==-1) {
			myPrint(error_message);
		}
	}	else {
		int f=chdir((const char*) s);
		if (f==-1) {
			myPrint(error_message);
		}
	} 
}

void fpwd() {
	char* buf=malloc(sizeof(char)*200);
	char* a=getcwd(buf,200);
	if (a==NULL) {
		myPrint(error_message);
	} else {
		myPrint(a);
		myPrint("\n");
	}
	free(buf);
}

char** insert_back(char** s, int p, char* i) {
  
  if (strcmp(i,"")==0) {
    
    char** r=malloc(sizeof(char*)*p);
    for (int j=1 ; j<p ; j++) {
      r[j-1]=strdup(s[j]);	
    }
    
    r[p-1]=NULL;
    return r;
  } else {
    char** r=malloc(sizeof(char*)*(p+1));
    for (int j=1 ; j<p ; j++) {
      r[j-1]=strdup(s[j]);
    }
    r[p-1]=strdup(i);
    r[p]=NULL;
    return r;
  }
}

void fexecvp(char** s) {
  execvp(s[0],s);
}





void fredirect(char** s, int x, int y, int z) {
	if (y==0) {
		return;
	}
	if (y==1) {
		char* a=strdup(s[x]);
		a[z]='\0';
    	
    	char* b=strdup(s[x]);
    	char* c=b+z+1;

    	if ((strcmp(c,"")==0) && (s[x+1]==NULL)) {
			
			myPrint(error_message);
			return;
		} else if ((strcmp(c,"")!=0) && (s[x+1]!=NULL)) {
        	myPrint(error_message);
			return;
		} else if ((s[x+1]!=NULL) && (s[x+2]!=NULL)) {
			myPrint(error_message);
			return;
		} else if (strcmp(c,"")==0) {
			c=strdup(s[x+1]);
		  	
    	} 
	
		char** toex=insert_back(s, x, a);
		if ((strcmp(toex[0],"pwd")==0) || (strcmp(toex[0],"cd")==0) || (strcmp(toex[0],"exit")==0)) {
			myPrint(error_message);
			return;
		}  
		mode_t mode=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      	int fd;
      	fd=open((const char*) c, O_RDONLY, mode);
      	if (fd!=-1) {
        	myPrint(error_message);
        	close(fd);
        	return;
      	}
		fd=open((const char*) c, O_RDONLY | O_CREAT | O_WRONLY, mode);
      	if (fd==-1) {
        	myPrint(error_message);
        	close(fd);
        	return;
      	}
		pid_t child_pid;
		child_pid=fork();
		if (child_pid==0) {
        	dup2(fd, 1);
        	fexecvp(toex);
        	myPrint(error_message);
        	exit(0);
    	} else {
      		wait(0);
    	} 
		close(fd);	
	}   else if (y==2) {
    	char* a=strdup(s[x]);
		a[z]='\0';
    	char* b=strdup(s[x]);
    	char* c=b+z+2;
    	if ((strcmp(c,"")==0) && (s[x+1]==NULL)) {
			myPrint(error_message);
			return;
		} else if ((strcmp(c,"")!=0) && (s[x+1]!=NULL)) {
			
      		myPrint(error_message);
			return;
		} else if ((s[x+1]!=NULL) && (s[x+2]!=NULL)) {
			myPrint(error_message);
			return;
		} else if (strcmp(c,"")==0) {
			c=strdup(s[x+1]);
    	} 
		char** toex=insert_back(s, x, a);
		if ((strcmp(toex[0],"pwd")==0) || (strcmp(toex[0],"cd")==0) || (strcmp(toex[0],"exit")==0)) {
			myPrint(error_message);
			return;
		}  
		

		mode_t mode=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      	int fd1;
      	fd1=open((const char*) c, O_RDWR | O_CREAT, mode);
		if (fd1==-1) {
        	myPrint(error_message);
        	close(fd1);
        	return;
      	}
		char* read_buff=malloc(sizeof(char)*2000);
     	int flag=read(fd1,read_buff,2000);  			
		close(fd1);
		int fd;
		fd=open((const char*) c, O_RDWR | O_CREAT | O_TRUNC, mode);
		pid_t child_pid;
		child_pid=fork();
		if (child_pid==0) {
     		
		  	pid_t c_pid;
		  	c_pid=fork();
      		dup2(fd, 1);
      		if (c_pid==0) {
      			fexecvp(toex);
      			myPrint(error_message);
      			exit(0);
      		} else {
      			wait(0);
      		}
      		if (flag > -1) {
      			
      			myPrint(read_buff);
      		}
      		exit(0);
    	}	else {
    		wait(0);
    	}   
		
		close(fd);    
	}
}

					



int main(int argc, char *argv[]) {
  	if (argc==1) {
    	char* cmd_buff=malloc(sizeof(char)*514);
    	char *pinput;
        
    	while (1) {        
      		myPrint("myshell> ");
     		pinput = fgets(cmd_buff, 514, stdin);
            
      		char* use = strdup(pinput);
      		int flag=0;
      		for (int i=0; i<514; i++) {
        		if (cmd_buff[i]=='\n') {                
          			flag=1;
            		break;
        		}
        		if (cmd_buff[i]=='\0') {
            		break;
        		}
      		}
      		if (flag==0) {
				int len=513;
		    	char tem='1';
		    	while ((len==513) && tem!='\n') {
			    	len=strlen(pinput);
			    	tem=pinput[len-1];
			    	myPrint(pinput);
          			if ((tem=='\n') || (len!=513)) {
				    	break;
			    	}
			    	pinput = fgets(cmd_buff, 514, stdin);
		    	}
		    	myPrint(error_message);
      		}
            
      		if (flag==1) {
        		char*** put=malloc(sizeof(char**)*512);
        		for (int i=0; i<512; i++) {
          			put[i]=malloc(sizeof(char*)*512);
          			for (int j=0; j<512; j++) {
            			put[i][j]=malloc(sizeof(char)*512);
 						put[i][j]=NULL;
          			}    
        		}
        		char* pch=malloc(sizeof(char)*512);
        		pch=strtok(use, ";");
     		   if (pch!=NULL) {
          			put[0][0]=strdup(pch);
     			}	
    		    int c=0;
        		while (pch!=NULL) {
          			c=c+1;
          			pch=strtok(NULL, ";");
            
          			if  (pch!=NULL) {
            			put[c][0]=strdup(pch);
                    	
          			}        
        		}
                    
        		for (int i=0; i<=c; i++) {
          			int d=1;
         		 	if (put[i][0]==NULL) {
                    	
            			break;
        			}
         			pch=strtok(put[i][0], " \n\t");
          			if (pch!=NULL) {
            			put[i][d]=strdup(pch);
                    	
          			}
          			while (pch!=NULL) {
            			d=d+1;
              			pch=strtok(NULL, " \n\t");
              			if (pch!=NULL) {
                			put[i][d]=strdup(pch);
            			}
          			}
                    
          			if (put[i][1]==NULL) {
                    continue;
                }

                if (strcmp(put[i][1],"exit")==0) {
            			if (put[i][2]!=NULL) {
              				myPrint(error_message);
                    		continue;
            			} else {
              				fexit();
            			}
          			}	else if (strcmp(put[i][1],"cd")==0) {
                    	
            			if (put[i][3]!=NULL) {
                    		
              				myPrint(error_message);
                			continue;
						} else {
						
							fcd(put[i][2]);
							continue;
						}
					} 	else if (strcmp(put[i][1],"pwd")==0) {
            			if (put[i][2]!=NULL) {
              				myPrint(error_message);
              				continue;
            			} else {
              				fpwd();
              				continue;	
            			}		
          			}

          			int j=1;
          			int x=0;
          			int y=0;
          			int z=0;
          			while (put[i][j]!=NULL) {
            			int len=strlen(put[i][j]);
            			for (int k=0; k<len; k++) {
              				if (put[i][j][k]=='>') {
                				x=j;	
                				z=k;
                				if (put[i][j][k+1]=='+') {
                  					y=2;
                				} else {
                  					y=1;
         						}	
        						break;              
              				}
            			}
                   		if (y!=0) {
                   			break;
                   		}
            			j=j+1;
          			}
					fredirect(put[i],x,y,z);
					if (y==0) {
						pid_t child_pid;
						child_pid=fork();
						if (child_pid==0) {
        					fexecvp(put[i]+1);
        					myPrint(error_message);
        					exit(0);
    					} else {
      						wait(0);
    					} 
    				}	

        		}
    		}
		}
	} else if (argc==2)  {
		FILE* fd3;
		fd3=fopen(argv[1], "r");
		if (fd3==NULL) {
			myPrint(error_message);

		}
		char* cmd_buff=malloc(sizeof(char)*2000);
		while (fgets(cmd_buff, 2000, fd3) != NULL) {
            char* use = strdup(cmd_buff);
      		char* use1=strdup(cmd_buff);
      		char* check=malloc(sizeof(char)*2000);
      		check=strtok(use1," \n\t");
      		if (check!=NULL) {
      			myPrint(use);
      		}
      		int flag=0;
      		for (int i=0; i<514; i++) {
        		if (cmd_buff[i]=='\n') {                
          			flag=1;
            		break;
        		}
        		if (cmd_buff[i]=='\0') {
            		break;
        		}
      		}
      		if (flag==0) {
				myPrint(error_message);
      			continue;
      		}
            
      		if (flag==1) {
        		char*** put=malloc(sizeof(char**)*512);
        		for (int i=0; i<512; i++) {
          			put[i]=malloc(sizeof(char*)*512);
          			for (int j=0; j<512; j++) {
            			put[i][j]=malloc(sizeof(char)*512);
 						put[i][j]=NULL;
          			}    
        		}
        		char* pch=malloc(sizeof(char)*512);
        		pch=strtok(use, ";");
     		   if (pch!=NULL) {
          			put[0][0]=strdup(pch);
     			}	
    		    int c=0;
        		while (pch!=NULL) {
          			c=c+1;
          			pch=strtok(NULL, ";");
            
          			if  (pch!=NULL) {
            			put[c][0]=strdup(pch);
                    	
          			}        
        		}
                    
        		for (int i=0; i<=c; i++) {
          			int d=1;
         		 	if (put[i][0]==NULL) {
                    	
            			break;
        			}
         			pch=strtok(put[i][0], " \n\t");
          			if (pch!=NULL) {
            			put[i][d]=strdup(pch);
                    	
          			}
          			while (pch!=NULL) {
            			d=d+1;
              			pch=strtok(NULL, " \n\t");
              			if (pch!=NULL) {
                			put[i][d]=strdup(pch);
            			}
          			}
                    
                    
          			if (put[i][1]==NULL) {
          				continue;
          			}

          			if (strcmp(put[i][1],"exit")==0) {
            			if (put[i][2]!=NULL) {
              				myPrint(error_message);
                    		continue;
            			} else {
              				fexit();
            			}
          			}	else if (strcmp(put[i][1],"cd")==0) {
                    	
            			if (put[i][3]!=NULL) {
                    		
              				myPrint(error_message);
                			continue;
						} else {
						
							fcd(put[i][2]);
							continue;
						}
					} 	else if (strcmp(put[i][1],"pwd")==0) {
            			if (put[i][2]!=NULL) {
              				myPrint(error_message);
              				continue;
            			} else {
              				fpwd();
              				continue;	
            			}		
          			}

          			int j=1;
          			int x=0;
          			int y=0;
          			int z=0;
          			while (put[i][j]!=NULL) {
            			int len=strlen(put[i][j]);
            			for (int k=0; k<len; k++) {
              				if (put[i][j][k]=='>') {
                				x=j;	
                				z=k;
                				if (put[i][j][k+1]=='+') {
                  					y=2;
                				} else {
                  					y=1;
         						}	
        						break;              
              				}
            			}
                   		if (y!=0) {
                   			break;
                   		}
            			j=j+1;
          			}
					fredirect(put[i],x,y,z);
					if (y==0) {
						pid_t child_pid;
						child_pid=fork();
						if (child_pid==0) {
        					fexecvp(put[i]+1);
        					myPrint(error_message);
        					exit(0);
    					} else {
      						wait(0);
    					} 
    				}		

        		}
    		}
    	}	
		fclose(fd3);
	} else {
		myPrint(error_message);
	}
}                   
              

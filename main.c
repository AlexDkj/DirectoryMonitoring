#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "queue.h"
#include "list.h"

int flag = 0;

/* handler for parent */
void handler_sigint(int signum){

  flag = 1;

}

/* handler for childs when cathed signal SIGINT */
void handler_int(int signum){

  //printf("Ok\n");
  exit(0);

}


int main(int argc, char** argv){

  que* queue;
  char fifo1[10] = "../fifo";
  char* tk = " ",cwdp[100];
  char tp[10] = " ";
  char tem[200];
  char buff[100] = " ";
  char buff1[4098] = " ";
  char wd[200] = " ";
  int pe[2],fd,fd2,fd3;
  int status;
  pid_t pid,pid1;
  fd_set rfds;
  struct timeval tv;
  l* list;
  d* temp;

  /* Get filepath from argv or the default */
  if(argc < 3){
    /* default */
    getcwd(wd,200);
    strncpy(cwdp,wd,strlen(wd));
  }
  else{
    strncpy(wd,argv[2],strlen(argv[2]));
    wd[strlen(argv[2])] = '\0';
    strncpy(cwdp,wd,strlen(wd));
  }

  /* Create file for outputs .out files */
  if(mkdir("../out",0755) == -1){
    if(errno != EEXIST){
      perror("Error failed to create directory\n");
      return 1;
    }
  }

  if(pipe(pe) == -1){
    perror("Error pipe failed to create");
    return 1;
  }

  if((pid = fork()) == -1){
    perror("Error fork failed to work");
    return 1;
  }

  if(pid != 0){
      close(pe[1]);
      dup2(pe[0],0);

      /* init queue with info for processes */
      queue = queue_init(queue);

      queue = malloc(sizeof(que));
      queue->sizeofqueue = 1;
      queue->front = NULL;

      signal(SIGINT,handler_sigint);

      /* read when comes data to pipe and keep only important operations in monitored directory */
      while(read(pe[0], buff, 100) > 0){

        if(strncmp(buff+strlen(wd), "/ MOVED_TO", 10) == 0 || strncmp(buff+strlen(wd),"/ CREATE", 8) == 0){

	  strcpy(buff, buff+strlen(wd)+11);
	  //printf("%s",buff);

          /* if queue who keeps proccess ids has free proccess then give file to work with
             else create new proccess with new named pipe */
	  if((temp = queue_search(queue->front)) != NULL){

	    if((fd = open(temp->fifoname, O_RDWR)) < 0){
              perror("Error fifo file failed to open");
              return 1;
            }

            /* give job to the process who found free */
	    write(fd, buff, strlen(buff));
	    kill((long)temp->pid,SIGCONT);
	  }
	  else{

            /* create fifo like as fifo1,fifo2... when we don't found process free */
            strncpy(fifo1+7,"",1);
	    sprintf(tp,"%d",queue->sizeofqueue);
	    strncat(fifo1,tp,atoi(tp));

	    if(mkfifo(fifo1, 0666) == -1){
              if(errno != EEXIST){
                perror("Error fifo file failed to create");
                return 1;
              }
            }

	    if((fd = open(fifo1, O_RDWR)) < 0){
              perror("Error fifo file failed to open");
              return 1;
            }

	    write(fd, buff, strlen(buff));

	    strncpy(tp," ",1);

            if((pid1 = fork()) == -1){
              perror("Error fork failed to work");
              return 1;
	    }

	    if(pid1 != 0){

              /* wait for all worker processes until sigchld signal occur when worker get signal sigstop */
	      int w = waitpid(-1, &status, WUNTRACED);

	      queue->front = queue_insert(queue->front,queue,w,fifo1,1);
	      queue->sizeofqueue++;
              /* when queue->front->flag is 1 proccess are avaliable to get job else not */
              //queue->front->flag = 0;
	    }
            if(pid1 == 0){

	      if((fd2 = open(fifo1, O_RDWR)) < 0){
                perror("Error fifo file failed to open");
                return 1;
              }

	      strncpy(buff," ",strlen(buff));

	      list = list_init(list);

	      while(1){

                /* read from named pipe who get filename and do the job with this file */
                if(read(fd2, buff, 100) > 0){

                  signal(SIGINT,handler_int);

		  strcpy(tem, wd);
                  strncat(tem,"/",1);
		  //printf("%s\n",tem);
		  
                  if((fd3 = open(strncat(tem ,buff, strlen(buff)-1), O_RDWR)) < 0){
                    perror("Error file failed to open");
                    return 1;
                  }


                  /* read from file who get from named pipe of worker process */
	          while(read(fd3, buff1, 4098) > 0){
		    
                    tk = strtok(buff1," \n");

                    while(tk != NULL){
		      
		      tk[strlen(tk)] = '\0';

		      if(strncmp(tk, "http://www.", 11) == 0){
                        tk = tk + 11;

                        if(tk[strlen(tk)-1] == '/')
                          tk[strlen(tk)-1] = '\0';

                        list = list_insert(list,tk);
                      }
	              else if(strncmp(tk, "http://", 7) == 0){
	                tk = tk + 7;

                        if(tk[strlen(tk)-1] == '/')
                          tk[strlen(tk)-1] = '\0';

                        list = list_insert(list,tk);
                      }

                      tk = strtok(NULL," \n");
		    }

                    list_print(list,tem,strlen(wd));
		    strncpy(buff1," ",100);

                  }

		  strncpy(buff," ",strlen(buff));
		  close(fd3);
		  free_list(list);
		  list = list_init(list);
		  
		}
		raise(SIGSTOP);
	      }
              close(fd2);
	    }
	  }
	}

	strncpy(buff," ",100);
	strncpy(buff1," ",100);

      }

      /* free all resources when catched the signal SIGINT for parent and terminate childs */
      if(flag == 1){
        /* free queue with info of worker processes */
	free_queue(queue->front);
	free(queue);
	close(pe[0]);
        close(pe[1]);
	close(fd);
        exit(0);
      }
      
  }
  else{
    close(pe[0]);
    dup2(pe[1],1);
    close(pe[1]);
    execlp("inotifywait", "inotifywait", wd, "-m", NULL);

  }

  return 0;

}

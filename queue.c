#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>
#include <signal.h>

que*  queue_init(que* q){

  q = NULL;

  return q;
  
}

/* Insert proccesses infos in queue */
d* queue_insert(d* dat, que* q, long i, char *fname, int f){

  if(dat == NULL){
    dat = malloc(sizeof(d));
    dat->pid = i;
    dat->fifoname = malloc(strlen(fname));
    strncpy(dat->fifoname,fname,strlen(fname));
    dat->flag = f;
    
    dat->next = NULL;
  }
  else{
    dat->next = queue_insert(dat->next,q,i,fname,f);
  }
  
  return dat;
}

/* Search if any proccess child are free to get job */
d* queue_search(d* dat){

  d* temp;

  temp = dat;

  while(temp != NULL){
    if(temp->flag == 0)
      return temp;
    temp = temp->next;
  }

  return temp;
}

/* void queue_print(d* dat){

  d* temp;

  temp = dat;
  
  while(temp != NULL){
    printf("%ld\n",(long)temp->pid);
    temp = temp->next;
  }

  temp = NULL;
  
} */

void free_queue(d* dat){

  d* temp;

  while(dat != NULL){

    temp = dat->next;

    kill(dat->pid,SIGINT);
    free(dat->fifoname);
    free(dat);

    dat = temp;
  }

  temp = NULL;
}

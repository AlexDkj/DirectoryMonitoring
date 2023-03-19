#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "list.h"

l* list_init(l* list){

  list = NULL;

  return list;
}

/* Insert data to list who keeps urls and how many times found in file of child who call */
l* list_insert(l* list, char* word){

  if(list == NULL){
    list = (l*)malloc(sizeof(l));
    list->word = (char*)malloc(strlen(word));
    strncpy(list->word,word,strlen(word));
    list->sumofword = 1;

    list->next = NULL;
  }
  else{
    if(strncmp(list->word,word,strlen(word)) == 0){
       list->sumofword += 1;
       return list;
    }
    list->next = list_insert(list->next,word);
  }

  return list;
}

/* Write output to .out files in out folder */
void list_print(l* list, char* filename,int offsetnum){

  int fd;
  l* ls;

  ls = list;

  char wd[200];

  getcwd(wd,200);

  char temp[100] = "../out";

  strncat(temp,filename+offsetnum,strlen(filename+offsetnum));
  strncat(temp,".out",4);
  //printf("%s\n",temp);

  if((fd = open(temp, O_RDWR | O_CREAT, 0644)) < 0){
     perror("Error file failed to create");
     return;
  }

  while(ls != NULL){
    if(strncmp(ls->word, " ", 1) != 0){
      //printf("%s %d\n",ls->word,ls->sumofword);
      write(fd, ls->word, strlen(ls->word));
      write(fd, " ", 1);
      sprintf(temp,"%d",ls->sumofword);
      write(fd, temp, strlen(temp));
      write(fd, "\n", 1);
    }
    ls = ls ->next;
  }

  ls = NULL;

  close(fd);

}

void free_list(l* dat){

  l* temp;
  
  while(dat != NULL){

    temp = dat->next;

    free(dat->word);
    free(dat);

    dat = temp;
  }

  temp = NULL;
  
}

typedef struct data{
   long pid;
   char* fifoname;
   int flag;
   struct data* next;
}d;

typedef struct queue{
  int sizeofqueue;
  d* front;
  d* rear;
}que;

que*  queue_init(que* );
d* queue_insert(d* ,que* q ,long ,char*, int );
d* queue_search(d* );
void queue_print(d* );
void free_queue(d* );

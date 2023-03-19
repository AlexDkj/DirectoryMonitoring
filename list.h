typedef struct list{
  char* word;
  int sumofword;
  struct list* next;
}l;

l* list_init(l* );
l* list_insert(l* ,char* );
void list_print(l* ,char* ,int );
void free_list(l* );

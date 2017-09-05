#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP -1; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* set_wp(char* args) {
  bool flag;
  int value=expr(args,&flag);
  WP* wp=new_wp();
  if (flag) {
    wp->begin=value;
    strcpy(wp->exp,args);
    return wp;
  }
  return 0;
}

bool del_wp(char* args) {
  WP* p=head;
  int no=atoi(args);
  for(;p;p=p->next) {
    if (p->NO == no) {
      free_wp(p);
      return true;
    }
  }
  return false;
}

WP* new_wp() {
  WP* new=free_;
  if(new) {
    free_=free_->next;
    new->next=head;
    head=new;
    return new;
  }
  else {
    printf("No available watchpoint\n");
    assert(0);
    return 0;
  }
}

int free_wp(WP* wp) {
  WP* p;
  if (wp == head) {
    head=head->next;
    wp->next=free_;
  }
  else {
    for(p=head;p;p=p->next) {
      if (p->next==wp) {
        p->next=wp->next;
        wp->next=free_;
      }
    }
  }
  free_=wp;
  return 0;
}
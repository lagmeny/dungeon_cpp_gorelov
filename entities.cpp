#include "entities.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
void hero_init(Hero *h) {
    if(!h)return;
    *h=Hero {
        1,1,100,100,10,0,1,0
    } ;
}
void hero_hit(int *hp,int damage) {
    if(!hp||damage<0)return;
    *hp=damage>=*hp?0:*hp-damage;
}
static const char *names[]= {
    "poisoned","blind","invisible","petrified","burning","sleeping"
} ;
unsigned flag_by_name(const char *s) {
    if(!s)return 0;
    for(unsigned i=0;i<6;i++)if(strcmp(s,names[i])==0)return 1u<<i;
    return 0;
}
void hero_status(const Hero *h) {
    if(!h)return;
    printf("HP: %d/%d (%.1f%%) Attack: %d Gold: %u Depth: %u Position: %d,%d\n",h->hp,h->max_hp,100.0*(double)h->hp/(double)h->max_hp,h->attack,h->gold,h->depth,h->row,h->col);
    printf("States:");
    bool any=false;
    for(unsigned i=0;i<6;i++)if(h->flags&(1u<<i)) {
        printf(" %s",names[i]);
        any=true;
    }
    if(!any)printf(" none");
    putchar('\n');
}
bool monster_add(Monsters *m,Monster v) {
    if(!m)return false;
    if(m->count==m->capacity) {
        if(m->capacity>SIZE_MAX/2/sizeof(Monster))return false;
        size_t next=m->capacity?m->capacity*2:2;
        if(next>SIZE_MAX/sizeof(Monster))return false;
        Monster *p=static_cast<Monster *>(realloc(m->items,next*sizeof(*p)));
        if(!p)return false;
        m->items=p;
        m->capacity=next;
    }
    m->items[m->count++]=v;
    return true;
}
bool monster_remove(Monsters *m,size_t index) {
    if(!m||index>=m->count)return false;
    for(size_t i=index+1;i<m->count;i++)m->items[i-1]=m->items[i];
    m->count--;
    return true;
}
void monsters_free(Monsters *m) {
    if(!m)return;
    free(m->items);
    m->items=NULL;
    m->count=m->capacity=0;
}

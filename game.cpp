#include "game.hpp"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE_SIZE 256
#define MAX_WORDS 8
static size_t split(char *s,char **words,size_t max) {
    size_t n=0;
    while(*s) {
        while(*s&&isspace((unsigned char)*s))s++;
        if(!*s)break;
        if(n==max)return max+1;
        words[n++]=s;
        while(*s&&!isspace((unsigned char)*s)) {
            *s=(char)tolower((unsigned char)*s);
            s++;
        }
        if(*s)*s++='\0';
    }
    return n;
}
static bool parse_number(const char *s,unsigned *value) {
    if(!s||!*s||!value)return false;
    unsigned n=0;
    for(;*s;s++) {
        if(*s<'0'||*s>'9')return false;
        unsigned digit=(unsigned)(*s-'0');
        if(n>(UINT_MAX-digit)/10)return false;
        n=n*10+digit;
    }
    *value=n;
    return true;
}
GameError game_init(Game *g,size_t h,size_t w,const char *level) {
    if(!g)return GAME_BAD_SIZE;
    *g=Game {
    } ;
    hero_init(&g->hero);
    g->running=true;
    GameError e=level?map_load(&g->map,level):map_create(&g->map,h,w);
    if(e!=GAME_OK)return e;
    if(level) {
        for(size_t r=0;r<g->map.height;r++)for(size_t c=0;c<g->map.width;c++)if(map_get(&g->map,r,c)=='@') {
            g->hero.row=(int)r;
            g->hero.col=(int)c;
            *map_cell(&g->map,r,c)='.';
        }
    } else {
        *map_cell(&g->map,1,2)='$';
        *map_cell(&g->map,g->map.height-2,g->map.width-2)='E';
        if(g->map.height>5&&g->map.width>5)*map_cell(&g->map,2,3)='#';
    }
    return GAME_OK;
}
void game_free(Game *g) {
    if(!g)return;
    monsters_free(&g->monsters);
    map_free(&g->map);
}
/* Coordinates are changed through pointers to Hero's fields; no global state is modified. */
bool game_move(Game *g,int dr,int dc) {
    if(!g)return false;
    int nr=g->hero.row+dr,nc=g->hero.col+dc;
    char type;
    bool passable;
    if(!map_info(&g->map,nr,nc,&type,&passable)||!passable)return false;
    g->hero.row=nr;
    g->hero.col=nc;
    g->turns++;
    if(type=='$') {
        g->hero.gold++;
        *map_cell(&g->map,(size_t)nr,(size_t)nc)='.';
        puts("Gold collected!");
    }
    if(type=='E')puts("Exit reached!");
    return true;
}
static void help(void) {
    puts("north/south/east/west (n/s/e/w), go north, look, status, hit N, set STATE, clear STATE, toggle STATE, check, use N, addmonster, delmonster N, monsters, addresses, help, quit");
}
void game_command(Game *g,char *line) {
    if(!g||!line)return;
    char *w[MAX_WORDS];
    size_t n=split(line,w,MAX_WORDS);
    if(n==0)return;
    if(n>MAX_WORDS) {
        puts("Too many words");
        return;
    }
    const char *cmd=w[0];
    if(strcmp(cmd,"go")==0) {
        if(n!=2) {
            puts("Usage: go DIRECTION");
            return;
        }
        cmd=w[1];
    }
    int dr=0,dc=0;
    bool move=true;
    if(!strcmp(cmd,"north")||!strcmp(cmd,"n"))dr=-1;
    else if(!strcmp(cmd,"south")||!strcmp(cmd,"s"))dr=1;
    else if(!strcmp(cmd,"east")||!strcmp(cmd,"e"))dc=1;
    else if(!strcmp(cmd,"west")||!strcmp(cmd,"w"))dc=-1;
    else move=false;
    if(move) {
        puts(game_move(g,dr,dc)?"Moved":"Blocked");
        return;
    }
    if(!strcmp(cmd,"quit")) {
        g->running=false;
        puts("Goodbye");
    } else if(!strcmp(cmd,"look"))map_draw(&g->map,g->hero.row,g->hero.col);
    else if(!strcmp(cmd,"help"))help();
    else if(!strcmp(cmd,"status"))hero_status(&g->hero);
    else if(!strcmp(cmd,"check"))puts(map_reachable(&g->map,g->hero.row,g->hero.col)?"Exit reachable":"Exit unreachable");
    else if(!strcmp(cmd,"hit")) {
        unsigned amount;
        if(n!=2||!parse_number(w[1],&amount)||amount>INT_MAX) {
            puts("Invalid damage");
            return;
        }
        hero_hit(&g->hero.hp,(int)amount);
        hero_status(&g->hero);
    } else if(!strcmp(cmd,"set")||!strcmp(cmd,"clear")||!strcmp(cmd,"toggle")) {
        unsigned flag=n==2?flag_by_name(w[1]):0;
        if(!flag) {
            puts("Invalid state");
            return;
        }
        if(!strcmp(cmd,"set"))g->hero.flags|=flag;
        else if(!strcmp(cmd,"clear"))g->hero.flags&=~flag;
        else g->hero.flags^=flag;
        hero_status(&g->hero);
    } else if(!strcmp(cmd,"use")) {
        unsigned item;
        if(n!=2||!parse_number(w[1],&item)) {
            puts("Invalid item number");
            return;
        }
        printf("Item %u is not in inventory\n",item);
    } else if(!strcmp(cmd,"addmonster")) {
        Monster m= {
            g->hero.row,g->hero.col,10
        } ;
        puts(monster_add(&g->monsters,m)?"Monster added":"Out of memory");
    } else if(!strcmp(cmd,"delmonster")) {
        unsigned i;
        if(n!=2||!parse_number(w[1],&i)||!monster_remove(&g->monsters,i))puts("Invalid monster index");
        else puts("Monster removed");
    } else if(!strcmp(cmd,"monsters"))printf("Monsters: %zu Capacity: %zu\n",g->monsters.count,g->monsters.capacity);
    else if(!strcmp(cmd,"addresses"))printf("Hero: %p HP: %p Map: %p First cell: %p\n",(void*)&g->hero,(void*)&g->hero.hp,(void*)g->map.cells,(void*)map_cell(&g->map,0,0));
    else puts("Unknown command. Type help.");
}
int game_run(int argc,char **argv) {
    size_t h=8,w=12;
    const char *level=NULL;
    if(argc==3&&strcmp(argv[1],"--level")==0)level=argv[2];
    else if(argc==3) {
        unsigned a,b;
        if(!parse_number(argv[1],&a)||!parse_number(argv[2],&b)) {
            puts("Invalid dimensions");
            return 1;
        }
        h=a;
        w=b;
    } else if(argc==2) {
        level=argv[1];
    } else if(argc!=1) {
        puts("Usage: game [height width] OR game [level-file]");
        return 1;
    }
    Game game {
    } ;
    GameError error=game_init(&game,h,w,level);
    if(error!=GAME_OK) {
        printf("Error: %s\n",game_error_string(error));
        return 1;
    }
    puts("Welcome to the dungeon!");
    map_draw(&game.map,game.hero.row,game.hero.col);
    char line[LINE_SIZE];
    while(game.running) {
        printf("[%u] Enter command > ",game.turns);
        fflush(stdout);
        if(!fgets(line,sizeof(line),stdin)) {
            puts("Input closed");
            break;
        }
        if(!strchr(line,'\n')&&!feof(stdin)) {
            int ch;
            while((ch=getchar())!='\n'&&ch!=EOF) {
            }
            puts("Command too long");
            continue;
        }
        game_command(&game,line);
    }
    game_free(&game);
    return 0;
}

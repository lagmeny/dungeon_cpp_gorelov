#include "map.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define MAX_DIM 200
GameError map_create(Map *m,size_t h,size_t w) {
    if(!m)return GAME_BAD_SIZE;
    if(h<3||w<3||h>MAX_DIM||w>MAX_DIM||h>SIZE_MAX/w)return GAME_BAD_SIZE;
    char *p=static_cast<char *>(malloc(h*w));
    if(!p)return GAME_NO_MEMORY;
    m->cells=p;
    m->height=h;
    m->width=w;
    for(size_t r=0;r<h;r++)for(size_t c=0;c<w;c++)p[r*w+c]=(r==0||c==0||r==h-1||c==w-1)?'#':'.';
    return GAME_OK;
}
void map_free(Map *m) {
    if(!m)return;
    free(m->cells);
    m->cells=NULL;
    m->height=m->width=0;
}
char *map_cell(Map *m,size_t r,size_t c) {
    return m&&m->cells&&r<m->height&&c<m->width?&m->cells[r*m->width+c]:NULL;
}
char map_get(const Map *m,size_t r,size_t c) {
    return m&&m->cells&&r<m->height&&c<m->width?m->cells[r*m->width+c]:'\0';
}
bool map_info(const Map *m,int r,int c,char *type,bool *passable) {
    if(!m||!type||!passable||r<0||c<0||(size_t)r>=m->height||(size_t)c>=m->width)return false;
    *type=map_get(m,(size_t)r,(size_t)c);
    *passable=*type!='#';
    return true;
}
void map_draw(const Map *m,int hr,int hc) {
    for(size_t r=0;r<m->height;r++) {
        for(size_t c=0;c<m->width;c++)putchar((int)r==hr&&(int)c==hc?'@':map_get(m,r,c));
        putchar('\n');
    }
}
/* Each recursive call checks bounds before indexing. The separate visited buffer is reset for each search. */
static bool dfs(const Map *m,int r,int c,unsigned char *seen) {
    if(r<0||c<0||(size_t)r>=m->height||(size_t)c>=m->width)return false;
    size_t pos=(size_t)r*m->width+(size_t)c;
    if(seen[pos]||m->cells[pos]=='#')return false;
    seen[pos]=1;
    if(m->cells[pos]=='E')return true;
    return dfs(m,r-1,c,seen)||dfs(m,r+1,c,seen)||dfs(m,r,c-1,seen)||dfs(m,r,c+1,seen);
}
bool map_reachable(const Map *m,int r,int c) {
    if(!m||!m->cells)return false;
    unsigned char *seen=static_cast<unsigned char *>(calloc(m->height*m->width,1));
    if(!seen)return false;
    bool found=dfs(m,r,c,seen);
    free(seen);
    return found;
}
GameError map_load(Map *out,const char *path) {
    if(!out||!path)return GAME_BAD_FORMAT;
    FILE *f=fopen(path,"rb");
    if(!f)return GAME_FILE_NOT_FOUND;
    char line[MAX_DIM+3];
    size_t width=0,height=0,exits=0,starts=0;
    char *data=NULL;
    GameError error=GAME_OK;
    while(fgets(line,sizeof(line),f)) {
        size_t len=strcspn(line,"\r\n");
        if(len==0||len>MAX_DIM||(width&&len!=width)||height==MAX_DIM) {
            error=GAME_BAD_FORMAT;
            break;
        }
        if(!width)width=len;
        if(line[len]=='\r'&&line[len+1]!='\n') {
            error=GAME_BAD_FORMAT;
            break;
        }
        char *next=static_cast<char *>(realloc(data,(height+1)*width));
        if(!next) {
            error=GAME_NO_MEMORY;
            break;
        }
        data=next;
        for(size_t i=0;i<width;i++) {
            char ch=line[i];
            if(ch!='#'&&ch!='.'&&ch!='$'&&ch!='E'&&ch!='@') {
                error=GAME_BAD_FORMAT;
                break;
            }
            if(ch=='E')exits++;
            if(ch=='@')starts++;
            data[height*width+i]=ch;
        }
        if(error!=GAME_OK)break;
        height++;
    }
    if(ferror(f))error=GAME_BAD_FORMAT;
    if(fclose(f)!=0&&error==GAME_OK)error=GAME_BAD_FORMAT;
    if(error==GAME_OK&&(height<3||width<3||exits!=1||starts!=1))error=GAME_BAD_FORMAT;
    if(error==GAME_OK) {
        for(size_t r=0;r<height;r++)for(size_t c=0;c<width;c++)if((r==0||c==0||r==height-1||c==width-1)&&data[r*width+c]!='#')error=GAME_BAD_FORMAT;
    }
    if(error!=GAME_OK) {
        free(data);
        return error;
    }
    out->cells=data;
    out->height=height;
    out->width=width;
    return GAME_OK;
}
const char *game_error_string(GameError e) {
    switch(e) {
        case GAME_OK:return "success";
        case GAME_FILE_NOT_FOUND:return "file not found";
        case GAME_BAD_FORMAT:return "damaged level format";
        case GAME_NO_MEMORY:return "out of memory";
        case GAME_BAD_SIZE:return "invalid map size";
    }
    return "unknown error";
}

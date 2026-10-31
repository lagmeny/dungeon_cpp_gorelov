#ifndef ENTITIES_H
#define ENTITIES_H
#include <stdbool.h>
#include <stddef.h>
#define POISONED (1u << 0)
#define BLIND (1u << 1)
#define INVISIBLE (1u << 2)
#define PETRIFIED (1u << 3)
#define BURNING (1u << 4)
#define SLEEPING (1u << 5)
struct Hero { int row, col, hp, max_hp, attack; unsigned gold, depth, flags; };
struct Monster { int row, col, hp; };
struct Monsters { Monster *items; size_t count, capacity; };
void hero_init(Hero *hero);
void hero_hit(int *hp,int damage);
void hero_status(const Hero *hero);
unsigned flag_by_name(const char *name);
bool monster_add(Monsters *list,Monster value);
bool monster_remove(Monsters *list,size_t index);
void monsters_free(Monsters *list);
#endif

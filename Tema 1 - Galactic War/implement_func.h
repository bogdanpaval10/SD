// Copyright 2021 Paval Bogdan-Costin

#ifndef IMPLEMENT_FUNC_H_
#define IMPLEMENT_FUNC_H_

#define MAX_LEN_CHAR 30

typedef struct node_t node_t;
struct node_t
{
    void *data;
    node_t *prev, *next;
};

typedef struct list_t list_t;
struct list_t
{
    node_t* head;
    unsigned int data_size;
    unsigned int size;
};

typedef struct planets_t planets_t;
struct planets_t
{
    char name[MAX_LEN_CHAR];
    list_t *shields;
    int nr_kills;
};

list_t* create_list(unsigned int data_size);

void add_node(list_t *list, unsigned int n, const void* data);

void add(list_t *plist, char name_p[], int poz, int nr);

void dll_free(list_t** pp_list);

void upg(list_t *plist, int poz_planet, int poz_shield, int val);

void expp(list_t *plist, int poz_planet, int val);

void remove_node(list_t* list, unsigned int n);

void blh(list_t *plist, int poz, int ok);

void rmv(list_t *plist, int poz_planet, int poz_shield);

void col(list_t *plist, int poz_planet1, int poz_planet2);

void rot(list_t *plist, int poz, char c, int units);

void shw(list_t *plist, int poz);

void frees(list_t **plist);

#endif  // IMPLEMENT_FUNC_H_

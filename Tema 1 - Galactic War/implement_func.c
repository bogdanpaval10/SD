// Copyright 2021 Paval Bogdan-Costin

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "implement_func.h"

list_t* create_list(unsigned int data_size)
{
    list_t *list = malloc(sizeof(list_t));
    if (list == NULL) {
        fprintf(stderr, "eroare");
        exit(1);
    }
    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;
    return list;
}

void add_node(list_t *list, unsigned int n, const void* data)
{
    node_t *newn = malloc(sizeof(node_t));
    if (list == NULL)
        exit(1);
    if (newn == NULL) {
        fprintf(stderr, "eroare");
        exit(1);
    }
    if ((int)n < 0) {
        fprintf(stderr, "eroare");
        exit(1);
    }
    newn->data = malloc(list->data_size);
    if (newn->data == NULL) {
        fprintf(stderr, "eroare");
        exit(1);
    }
    memcpy(newn->data, data, list->data_size);
    if (list->head == NULL) {  // adaugare in lista goala
        newn->next = newn;
        newn->prev = newn;
        list->head = newn;
    }
    if (n == 0 || list->size == 0) {  // adaugare pe prima pozitie
        newn->next = list->head;
        newn->prev = list->head->prev;
        list->head->prev->next = newn;
        list->head->prev = newn;
        list->head = newn;
    }
    if (n == list->size && n != 0) {  // adaugare la final
        newn->next = list->head;
        newn->prev = list->head->prev;
        if (list->head == NULL) {
            list->head = list->head->prev = newn;
            newn->next = newn;
            newn->prev = newn;
        } else {
            list->head->prev->next = newn;
            list->head->prev = newn;
        }
    }
    if (n < list->size && n > 0) {  // adaugare in interior
        unsigned int i = 0;
        node_t* p = list->head;
        while (i < n - 1) {
            p = p->next;
            i++;
        }
        p->next->prev = newn;
        newn->next = p->next;
        newn->prev = p;
        p->next = newn;
    }
    list->size++;
}

void add(list_t *plist, char name_p[], int poz, int nr)
{
    int i, val = 1;
    if (plist == NULL)
        exit(1);
    if (poz > (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    planets_t *planet = malloc(sizeof(planets_t));  // creare planeta
    if (planet == NULL) {
        fprintf(stderr, "eroare");
        exit(1);
    }
    sscanf(name_p, "%s", planet->name);
    planet->shields = create_list(sizeof(int));
    planet->nr_kills = 0;
    for (i = 0; i < nr; i++)  // adaugare scuturi
        add_node(planet->shields, i, &val);
    add_node(plist, poz, planet);  // adaugare planeta in galaxie
    printf("The planet %s has joined the galaxy.\n", planet->name);
    free(planet);
}

void
dll_free(list_t** pp_list)  // dezalocare lista circulara dublu inlantuita
{
    node_t *pprev, *curr;
    pprev = (*pp_list)->head->prev;
	while (pprev != (*pp_list)->head) {
        curr = pprev;
        pprev = pprev->prev;
        free(curr->data);
		free(curr);
    }
    if (pprev == (*pp_list)->head) {
        free(pprev->data);
        free(pprev);
    }
	free(*pp_list);
	*pp_list = NULL;
}


void upg(list_t *plist, int poz_planet, int poz_shield, int val)
{
    if (poz_planet >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    node_t *curr = plist->head;
    int i = 0, j = 0;
    do {
        if (i == poz_planet) {  // cautare planeta in galaxie
            if (poz_shield >= (int)((planets_t*)curr->data)->shields->size) {
                printf("Shield out of bounds!\n");
                return;
            }
            node_t *curr2 = ((planets_t*)curr->data)->shields->head;
            j = 0;
            do {
                if (j == poz_shield) {  // cautare pozitie shield
                    *(int*)curr2->data = *(int*)curr2->data + val;
                    return;
                }
                j++;
                curr2 = curr2->next;
            }while (curr2 != ((planets_t*)curr->data)->shields->head &&
            j < (int)((planets_t*)curr->data)->shields->size);
        }
        i++;
        curr = curr->next;
    } while (curr != plist->head && i < (int)plist->size);
}

void expp(list_t *plist, int poz_planet, int val)
{
    if (poz_planet >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    node_t *curr = plist->head;
    int i = 0;
    do {
        if (i == poz_planet) {  // cautare planeta in galaxie
            add_node(((planets_t*)curr->data)->shields,
            ((planets_t*)curr->data)->shields->size, &val);
            break;  // adaugare nod la final
        }
        i++;
        curr = curr->next;
    }while (curr != plist->head && i < (int)plist->size);
}

void remove_node(list_t* list, unsigned int n)
{
    if (list == NULL || list->head == NULL)
        return;
    if ((int)n < 0) {
        fprintf(stderr, "eroare");
        exit(1);
    }
    if (n == 0) {  // eliminare primul nod
        node_t *del = list->head;
        list->head->next->prev = list->head->prev;
        list->head->prev->next = list->head->next;
        list->head = del->next;
        free(del->data);
        free(del);
        list->size--;
    }
    if (n >= list->size - 1 && n != 0) {  // eliminare ultimul nod
        node_t *del = list->head->prev;
        list->head->prev = del->prev;
        del->prev->next = list->head;
        free(del->data);
        free(del);
        list->size--;
    }
    if (n > 0 && n < list->size - 1) {  // eliminare nod din interior
        unsigned int i = 0;
        node_t *p = list->head;
        node_t *del;
        while (i < n - 1) {
            p = p->next;
            i++;
        }
        del = p->next;
        del->next->prev = p;
        p->next = del->next;
        free(del->data);
        free(del);
        list->size--;
    }
}

void blh(list_t *plist, int poz, int ok)
{
    if (poz >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    if (plist == NULL || plist->head == NULL)
        return;
    node_t *curr = plist->head;
    int i;
    for (i = 0; i < poz; i++)  // cautare planeta in galaxie
        curr = curr->next;
    list_t *p = ((planets_t*)curr->data)->shields;
    dll_free(&p);
    char name[MAX_LEN_CHAR];
    sscanf(((planets_t*)curr->data)->name, "%s", name);
    int size = plist->size;
    remove_node(plist, poz);  // eliminare planeta
    if (size == 1)
        plist->head = NULL;
    if (ok == 0)
        printf("The planet %s has been eaten by the vortex.\n", name);
}

void rmv(list_t *plist, int poz_planet, int poz_shield)
{
    if (poz_planet >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    node_t *curr = plist->head;
    int i, nr = 0;
    for (i = 0; i < poz_planet; i++)  // cautare planeta in galaxie
        curr = curr->next;
    if (poz_shield >= (int)((planets_t*)curr->data)->shields->size) {
        printf("Shield out of bounds!\n");
        return;
    }
    node_t *curr2 = ((planets_t*)curr->data)->shields->head;
    do {
        nr++;
        curr2 = curr2->next;
    }while (curr2 != ((planets_t*)curr->data)->shields->head);
    if (nr <= 4)
        printf("A planet cannot have less than 4 shields!\n");
    else  // eliminare scut
        remove_node(((planets_t*)curr->data)->shields, poz_shield);
}

void col(list_t *plist, int poz_planet1, int poz_planet2)
{
    if (poz_planet1 >= (int)plist->size || poz_planet2 >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    node_t *planet2, *planet1 = plist->head;
    int i, ok = 0;
    for (i = 0; i < poz_planet1; i++)  // cautare planete
        planet1 = planet1->next;
    planet2 = planet1->next;
    node_t *curr1 = ((planets_t*)planet1->data)->shields->head;
    for (i = 0; i < (int)((planets_t*)planet1->data)->shields->size / 4; i++)
        curr1 = curr1->next;
    if (*(int*)curr1->data == 0) {  // verificare punct coliziune planeta 1
        printf("The planet %s has imploded.\n",
        ((planets_t*)planet1->data)->name);
        blh(plist, poz_planet1, 1);  // eliminare planeta
        ((planets_t*)planet2->data)->nr_kills++;
        ok = 1;
    } else {
        *(int*)curr1->data = *(int*)curr1->data - 1;
    }
    node_t *curr2 = ((planets_t*)planet2->data)->shields->head;
    for (i = 0; i < 3 * ((int)((planets_t*)planet2->data)->shields->size / 4);
    i++)
        curr2 = curr2->next;
    if (*(int*)curr2->data == 0) {  // verificare punct coliziune planeta 2
        printf("The planet %s has imploded.\n",
        ((planets_t*)planet2->data)->name);
        if (ok == 1)
            poz_planet2--;
        blh(plist, poz_planet2, 1);  // eliminare planeta
        if (ok == 0)
            ((planets_t*)planet1->data)->nr_kills++;
    } else {
        *(int*)curr2->data = *(int*)curr2->data - 1;
    }
}

void rot(list_t *plist, int poz, char c, int units)
{
    if (poz >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    if (c != 'c' && c != 't') {
        printf("Not a valid direction!\n");
        return;
    }
    node_t *curr = plist->head;
    int i;
    for (i = 0; i < poz; i++)  // cautare planeta in galaxie
        curr = curr->next;
    if (units == (int)((planets_t*)curr->data)->shields->size)
        return;
    if (units > (int)((planets_t*)curr->data)->shields->size)
        units = units % ((planets_t*)curr->data)->shields->size;
    node_t *curr2 = ((planets_t*)curr->data)->shields->head;
    for (i = 0; i < units; i++) {  // cautare noul head in functie de unitati
        if (c == 'c')
            curr2 = curr2->prev;
        else
            curr2 = curr2->next;
    }
    ((planets_t*)curr->data)->shields->head = curr2;
}

void shw(list_t *plist, int poz)
{
    if (poz >= (int)plist->size) {
        printf("Planet out of bounds!\n");
        return;
    }
    if (plist == NULL || plist->head == NULL)
        return;
    node_t *curr2, *curr = plist->head;
    int i;
    for (i = 0; i < poz; i++)  // cautare planeta in galaxie
        curr = curr->next;
    printf("NAME: %s\n", ((planets_t*)curr->data)->name);
    if (plist->size == 2) {  // afisare informatii planeta
        node_t *curr2 = curr->next;
        printf("CLOSEST: %s\n", ((planets_t*)curr2->data)->name);
    } else {
        if (plist->size == 1) {
            printf("CLOSEST: none\n");
        } else {
            curr2 = curr->prev;
            printf("CLOSEST: %s and ", ((planets_t*)curr2->data)->name);
            curr2 = curr->next;
            printf("%s\n", ((planets_t*)curr2->data)->name);
        }
    }
    curr2 = ((planets_t*)curr->data)->shields->head;
    printf("SHIELDS: ");
    do {
        printf("%d ", *(int*)curr2->data);
        curr2 = curr2->next;
    }while (curr2 != ((planets_t*)curr->data)->shields->head);
    printf("\n");
    printf("KILLED: %d\n", ((planets_t*)curr->data)->nr_kills);
}

void frees(list_t **plist)  // dezalocare memorie lista de planete
{
	if ((*plist) == NULL || (*plist)->head == NULL) {
        free(*plist);
        (*plist)->head = NULL;
        return;
	}
	node_t *pprev, *curr;
    pprev = (*plist)->head->prev;
    while (pprev != (*plist)->head) {
        curr = pprev;
        pprev = pprev->prev;
        list_t *p =((planets_t*)curr->data)->shields;
        dll_free(&p);  // dezalocare lista scuturi
        free(curr->data);  // dezalocare data
		free(curr);  // dezalocare nod
    }
    if (pprev == (*plist)->head) {
        list_t *p = ((planets_t*)pprev->data)->shields;
        dll_free(&p);
        free(pprev->data);
        free(pprev);
    }
	free(*plist);  // dezalocare lista planete
	*plist = NULL;
}

/* Copyright 2021 <Paval Bogdan-Costin> */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#include "server.h"
#define HMAX 10000


server_memory* init_server_memory() {  // creare, alocare memorie pt un server
	server_memory *server = malloc(sizeof(server_memory));
	int i;
	if (server == NULL) {
		fprintf(stderr, "server malloc failed");
		return NULL;
	}
	server->hmax = HMAX;
	server->buckets = malloc(server->hmax * sizeof(linked_list_t*));
	if (server->buckets == NULL) {
		fprintf(stderr, "server malloc failed");
		return NULL;
	}
	for(i = 0; i < (int)server->hmax; i++)
		server->buckets[i] = ll_create(sizeof(struct info));
	server->size = 0;
	return server;
}

unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

void server_store(server_memory* server, char* key, char* value) {
	int index = hash_function_string(key) % server->hmax;
	ll_node_t *nnod, *curr_nod = server->buckets[index]->head;
	nnod = malloc(sizeof(ll_node_t));  // alocare memorie pt noul element
	DIE(nnod == NULL, "eroare malloc");
	nnod->data = malloc(sizeof(struct info));
	DIE(nnod->data == NULL, "eroare malloc");
	((struct info*)nnod->data)->key = malloc(strlen(key) + 1);
	if(((struct info*)nnod->data)->key == NULL) {
        fprintf(stderr, "eroare malloc");
        exit(1);
    }  // completare camp key
	memcpy(((struct info*)nnod->data)->key, key, strlen(key) + 1);
	((struct info*)nnod->data)->value = malloc(strlen(value) + 1);
	if(((struct info*)nnod->data)->value == NULL) {
        fprintf(stderr, "eroare malloc");
        exit(1);
    }  // completare camp value
	memcpy(((struct info*)nnod->data)->value, value, strlen(value) + 1);
	// cautare loc de inserare
	while(curr_nod != NULL &&
	compare_function_strings(((struct info*)curr_nod->data)->key, key) != 0) {
		curr_nod = curr_nod->next;
	}  // se adauga noul element printre cele deja existente
	if (curr_nod == NULL) {
		nnod->next = server->buckets[index]->head;
		server->buckets[index]->head = nnod;
		server->size++;
	} else {
		memcpy(((struct info*)curr_nod->data)->value,
		value, strlen(value) + 1);
		free(((struct info*)nnod->data)->value);
		free(((struct info*)nnod->data)->key);
		free(nnod->data);
		free(nnod);
	}
}

void server_remove(server_memory* server, char* key) {
	int poz = 0, index = hash_function_string(key) % server->hmax;
	ll_node_t *curr_nod = server->buckets[index]->head;
	while(curr_nod != NULL &&
	compare_function_strings(((struct info*)curr_nod->data)->key, key) != 0) {
		curr_nod = curr_nod->next;
		poz++;
	}  // se cauta pozitia elementul ce trebuie eliminat si se sterge
	if (curr_nod != NULL) {
		ll_remove_nth_node(server->buckets[index], poz);
		free(((struct info*)curr_nod->data)->value);
		free(((struct info*)curr_nod->data)->key);
		free((struct info*)curr_nod->data);
		free(curr_nod);
	}
}

char* server_retrieve(server_memory* server, char* key) {
	int index = hash_function_string(key) % server->hmax;
	ll_node_t *curr_nod = server->buckets[index]->head;
	while(curr_nod != NULL &&
	compare_function_strings(((struct info*)curr_nod->data)->key, key) != 0)
		curr_nod = curr_nod->next;  // cauta elementul cerut in baza cheii
	if (curr_nod != NULL)  // returneaza valoarea
		return ((struct info*)curr_nod->data)->value;
	return NULL;
}


void free_server_memory(server_memory* server) {
	int poz = 0;
	ll_node_t *pprev, *curr;  // se dazaloca fiecare camp alocat pt server
	while (poz < (int)server->hmax) {
		curr = server->buckets[poz]->head;
		pprev = server->buckets[poz]->head;
		while (curr != NULL) {
			pprev = curr;
        	curr = curr->next;
        	free(((struct info*)pprev->data)->value);
			free(((struct info*)pprev->data)->key);
			free((struct info*)pprev->data);
			free(pprev);
    	}
		free(server->buckets[poz]);
		poz++;
	}
	free(server->buckets);
	free(server);
}

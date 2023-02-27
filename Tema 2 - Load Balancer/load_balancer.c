/* Copyright 2021 <Paval Bogdan-Costin> */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "utils.h"
#define MAX_SERVERS 100000
#define MAX_STRING_SIZE 500

typedef struct server_info server_info;

struct server_info {
	unsigned int label;  // eticheta serverului
    int id;  // id-ul serverului
};

struct load_balancer {
	server_memory **servers;  // un array de servere
    server_info *hash_ring;  // un array de perechi (label, id)
    int nr_servers;  // numarul de servere din sistem
    int nr_hr;  // numarul de elemente din hash_ring
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}


load_balancer* init_load_balancer() {  // creare si alocare memorie
	load_balancer *main = malloc(sizeof(load_balancer));
    if (main == NULL) {
		fprintf(stderr, "main malloc failed");
        return NULL;
    }
    main->nr_servers = 0;
    main->servers = calloc(MAX_SERVERS, sizeof(server_memory*));
    if (main->servers == NULL) {
		fprintf(stderr, "main malloc failed");
        return NULL;
    }
    main->hash_ring = calloc(3 * MAX_SERVERS, sizeof(server_info*));
    main->nr_hr = 0;
    return main;
}


void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
	int i;
    for (i = 0; i < main->nr_hr; i++)  // cauta serverul pe care trebuie stocat
        if (main->hash_ring[i].label > hash_function_key(key))  // obiectul
            break;
    if (i == main->nr_hr)
        i = 0;
    *server_id = (int)main->hash_ring[i].id;  // se adauga pe serverul potrivit
    server_store(main->servers[main->hash_ring[i].id], key, value);
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	int i;
    for (i = 0; i < main->nr_hr; i++)  // cauta serverul pe care este stocat
        if (server_retrieve(main->servers[main->hash_ring[i].id], key) != NULL)
            break;
    if (i >= main->nr_hr)
	    return NULL;
    *server_id = (int)main->hash_ring[i].id;  // se returneaza valoarea
    return server_retrieve(main->servers[main->hash_ring[i].id], key);
}


void loader_add_server(load_balancer* main, int server_id) {
	main->servers[server_id] = init_server_memory();  // se creeaza serverul
    int i, j;
    for (i = 0; i < 3; i++) {  // se adauga fiecare replica pe hash_ring
        unsigned int label = i * MAX_SERVERS + server_id;
        int ok = 0;
        if (main->nr_servers == 0 && main->nr_hr == 0) {  // primul server
            main->hash_ring[0].label =
            (unsigned int)hash_function_servers(&label);
            main->hash_ring[0].id = server_id;
            main->nr_hr++;
            ok = 1;
        }
        for (j = 0; j < main->nr_hr && ok == 0; j++) {
            if (hash_function_servers(&label) < main->hash_ring[j].label) {
                int k;  // trebuie adaugat inainte de serverul de pe pozitia j
                for (k = main->nr_hr; k > j; k--) {
                    main->hash_ring[k].id = main->hash_ring[k - 1].id;
                    main->hash_ring[k].label = main->hash_ring[k - 1].label;
                }
                main->hash_ring[j].label =
                (unsigned int)hash_function_servers(&label);
                main->hash_ring[j].id = server_id;
                main->nr_hr++;
                ok = 1;
            }
            if (hash_function_servers(&label) == main->hash_ring[j].label &&
            server_id < main->hash_ring[j].id) {
                int k;  // trebuie adaugat inainte de serverul de pe pozitia j
                for (k = main->nr_hr; k > j; k--) {
                    main->hash_ring[k].id = main->hash_ring[k - 1].id;
                    main->hash_ring[k].label = main->hash_ring[k - 1].label;
                }
                main->hash_ring[j].label =
                (unsigned int)hash_function_servers(&label);
                main->hash_ring[j].id = server_id;
                main->nr_hr++;
                ok = 1;
            }
            if (hash_function_servers(&label) == main->hash_ring[j].label &&
            server_id > main->hash_ring[j].id) {
                int k;  // trebuie adaugat dupa serverul de pe pozitia j
                j++;
                for (k = main->nr_hr; k > j; k--) {
                    main->hash_ring[k].id = main->hash_ring[k - 1].id;
                    main->hash_ring[k].label = main->hash_ring[k - 1].label;
                }
                main->hash_ring[j].label =
                (unsigned int)hash_function_servers(&label);
                main->hash_ring[j].id = server_id;
                main->nr_hr++;
                ok = 1;
            }
        }
        if (ok == 0) {  // se adauga la final
            main->hash_ring[main->nr_hr].label =
            (unsigned int)hash_function_servers(&label);
            main->hash_ring[main->nr_hr].id = server_id;
            main->nr_hr++;
        }
    }
    main->nr_servers++;  // se rebalanseaza sistemul de servere
    for (i = 0; i < main->nr_hr && main->nr_servers > 1; i++)
        if (main->hash_ring[i].id == server_id) {
            int poz = 0, start;
            if (i == main->nr_hr - 1)
                start = -1;
            else
                start = i;
            if (i == main->nr_hr - 2 && main->hash_ring[i + 1].id == server_id)
                start = -1;
            if (i == main->nr_hr - 3 && main->hash_ring[i + 1].id == server_id
            && main->hash_ring[i + 2].id == server_id)
                start = -1;
            for (j = start + 1; j < main->nr_hr; j++)
                if (main->hash_ring[j].id != server_id)
                    break;
            for (j = start + 1; j < main->nr_hr; j++)
                if (main->hash_ring[j].id != server_id)
                    break;
            ll_node_t *curr;
            server_memory *copy_old = main->servers[main->hash_ring[j].id];
            server_memory *copy_new = main->servers[main->hash_ring[i].id];
            while (poz < (int)copy_old->hmax) {
                curr = copy_old->buckets[poz]->head;
                while (curr != NULL) {  // daca este cazul se muta obiecte
                    ll_node_t *currnext = curr->next;  // pe serverul adaugat
                    char key[MAX_STRING_SIZE], value[MAX_STRING_SIZE];
                    memcpy(key, ((struct info*)curr->data)->key,
                    strlen(((struct info*)curr->data)->key) + 1);
                    memcpy(value, ((struct info*)curr->data)->value,
                    strlen(((struct info*)curr->data)->value) + 1);
                    if (server_retrieve(copy_new, key) == NULL) {
                        loader_store(main, key, value, &start);
                        if (server_retrieve(copy_new, key) != NULL) {
                            server_remove(copy_old, key);
                        }  // se sterge de pe vechiul server
                    }
                    curr = currnext;
                }
                poz++;
            }
        }
}


void loader_remove_server(load_balancer* main, int server_id) {
	int i, j, k, poz = 0;
    server_memory *copy = main->servers[server_id];
    for (i = 0; i < main->nr_hr; i++)  // eliminarea replicilor serverului
        if (main->hash_ring[i].id == server_id) {
            for (j = i; j < main->nr_hr - 1; j++) {
                main->hash_ring[j].label = main->hash_ring[j + 1].label;
                main->hash_ring[j].id = main->hash_ring[j + 1].id;
            }
            i--;
            main->nr_hr = main->nr_hr - 1;
        }
    ll_node_t *curr;  // distribuirea obiectelor de pe acel server
    while (poz < (int)copy->hmax) {
        curr = copy->buckets[poz]->head;
        while (curr != NULL) {
            loader_store(main, ((struct info*)curr->data)->key,
            ((struct info*)curr->data)->value, &k);
            curr = curr->next;
            }
        poz++;
    }
    main->nr_servers--;
}

void free_load_balancer(load_balancer* main) {
    int i;  // se dazaloca fiecare camp alocat pt main
    for (i = 0; i < MAX_SERVERS; i++)
        if (main->servers[i] != NULL)
            free_server_memory(main->servers[i]);
    free(main->servers);
    free(main->hash_ring);
    free(main);
}

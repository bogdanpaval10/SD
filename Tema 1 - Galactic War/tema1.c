// Copyright 2021 Paval Bogdan-Costin

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "implement_func.h"
#define MAX_LEN_CHAR 30
#define NMAX 200


int check_command(list_t *list)
{
    char command[MAX_LEN_CHAR], line[NMAX];
    fgets(line, NMAX, stdin);  // citeste linia comenzii
    sscanf(line, "%s", command);
    if (strcmp(command, "ADD") == 0) {  // daca este comanda ADD
        char name_p[MAX_LEN_CHAR];
		int poz, nr_shields;
		if (sscanf(line, "%s %s %d %d",
        command, name_p, &poz, &nr_shields) == 4) {
            add(list, name_p, poz, nr_shields);
        } else {
            return -1;
        }
    }
    if (strcmp(command, "BLH") == 0) {  // daca este comanda BLH
        int poz;
        if (sscanf(line, "%s %d", command, &poz) == 2)
            blh(list, poz, 0);
        else
            return -1;
    }
    if (strcmp(command, "UPG") == 0) {  // daca este comanda UPG
        int poz_planet, poz_shield, val;
        if (sscanf(line, "%s %d %d %d",
        command, &poz_planet, &poz_shield, &val) == 4)
            upg(list, poz_planet, poz_shield, val);
        else
            return -1;
    }
	if (strcmp(command, "EXP") == 0) {  // daca este comanda EXP
	    int poz_planet, val;
        if (sscanf(line, "%s %d %d", command, &poz_planet, &val) == 3)
            expp(list, poz_planet, val);
        else
            return -1;
    }
	if (strcmp(command, "RMV") == 0) {  // daca este comanda RMV
        int poz_planet, poz_shield;
        if (sscanf(line, "%s %d %d", command, &poz_planet, &poz_shield) == 3)
            rmv(list, poz_planet, poz_shield);
        else
            return -1;
    }
	if (strcmp(command, "COL") == 0) {  // daca este comanda COL
        int poz_shield1, poz_shield2;
        if (sscanf(line, "%s %d %d", command, &poz_shield1, &poz_shield2) == 3)
            col(list, poz_shield1, poz_shield2);
        else
            return -1;
    }
	if (strcmp(command, "ROT") == 0) {  // daca este comanda ROT
        int poz, units;
        char c;
        if (sscanf(line, "%s %d %c %d", command, &poz, &c,  &units) == 4)
            rot(list, poz, c,  units);
        else
            return -1;
	}
    if (strcmp(command, "SHW") == 0) {  // daca este comanda SHW
        int poz;
        if (sscanf(line, "%s %d", command, &poz) == 2)
            shw(list, poz);
        else
            return -1;
    }
    return 0;
}

int main()
{
    int m, i;
    list_t *list = create_list(sizeof(planets_t));
    char c;
    scanf("%d%c", &m, &c);
    for (i = 0; i < m; i++)
        check_command(list);
    if (list != NULL)
        frees(&list);
    return 0;
}

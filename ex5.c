/*
Name: Sajed Isa
ID: 325949089
Exercise: EX5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= STRUCTS ================= */

typedef struct Episode {
    char *name;
    char length;              / format XX:XX:XX */
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

/* ================= GLOBALS ================= */

TVShow ***database = NULL;
int dbSize = 0;

/* ================= UTIL ================= */

char *readLine(void) {
    char *line = NULL;
    size_t size = 0;
    getline(&line, &size, stdin);
    line[strcspn(line, "\n")] = '\0';
    return line;
}

int validLength(const char *s) {
    return strlen(s) == 8 &&
           s[2] == ':' && s[5] == ':' &&
           s[0] >= '0' && s[0] <= '9' &&
           s[1] >= '0' && s[1] <= '9' &&
           s[3] >= '0' && s[3] <= '9' &&
           s[4] >= '0' && s[4] <= '9' &&
           s[6] >= '0' && s[6] <= '9' &&
           s[7] >= '0' && s[7] <= '9';
}

/* ================= FIND ================= */

TVShow *findShow(const char *name) {
    for (int i = 0; i < dbSize; i++)
        for (int j = 0; j < dbSize; j++)
            if (database[i][j] && strcmp(database[i][j]->name, name) == 0)
                return database[i][j];
    return NULL;
}

Season *findSeason(TVShow *s, const char *name) {
    for (Season *c = s->seasons; c; c = c->next)
        if (strcmp(c->name, name) == 0)
            return c;
    return NULL;
}

/* ================= ADD ================= */

void addTVShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    if (findShow(name)) {
        printf("Show already exists.\n");
        free(name);
        return;
    }

    TVShow *s = malloc(sizeof(TVShow));
    s->name = name;
    s->seasons = NULL;

    TVShow **newDB = realloc(database, (dbSize + 1) * sizeof(TVShow *));
    database = newDB;

    database[dbSize] = malloc((dbSize + 1) * sizeof(TVShow *));
    for (int i = 0; i <= dbSize; i++)
        database[dbSize][i] = NULL;

    database[dbSize][dbSize] = s;
    dbSize++;
}

void addSeason(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    free(showName);

    if (!s) {
        printf("Show not found.\n");
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = readLine();

    if (findSeason(s, seasonName)) {
        printf("Season already exists.\n");
        free(seasonName);
        return;
    }

    printf("Enter the position:\n");
    int pos;
    scanf("%d", &pos);
    getchar();

    Season *newS = malloc(sizeof(Season));
    newS->name = seasonName;
    newS->episodes = NULL;
    newS->next = NULL;

    if (pos <= 0 || !s->seasons) {
        newS->next = s->seasons;
        s->seasons = newS;
        return;
    }

    Season *cur = s->seasons;
    for (int i = 1; cur->next && i < pos; i++)
        cur = cur->next;

    newS->next = cur->next;
    cur->next = newS;
}

void addEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    free(showName);

    if (!s) {
        printf("Show not found.\n");
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *se = findSeason(s, seasonName);
    free(seasonName);

    if (!se) {
        printf("Season not found.\n");
        return;
    }

    printf("Enter the name of the episode:\n");
    char *epName = readLine();

    printf("Enter the length (xx:xx:xx):\n");
    char *len = readLine();
    while (!validLength(len)) {
        printf("Invalid length, enter again:\n");
        free(len);
        len = readLine();
    }

    printf("Enter the position:\n");
    int pos;
    scanf("%d", &pos);
    getchar();

    Episode *e = malloc(sizeof(Episode));
    e->name = epName;
    e->length = len;
    e->next = NULL;

    if (pos <= 0 || !se->episodes) {
        e->next = se->episodes;
        se->episodes = e;
        return;
    }

    Episode *cur = se->episodes;
    for (int i = 1; cur->next && i < pos; i++)
        cur = cur->next;

    e->next = cur->next;
    cur->next = e;
}

/* ================= CLEANUP ================= */

void cleanup(void) {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            TVShow *s = database[i][j];
            if (!s) continue;

            Season *se = s->seasons;
            while (se) {
                Episode *e = se->episodes;
                while (e) {
                    Episode *et = e;
                    e = e->next;
                    free(et->name);
                    free(et->length);
                    free(et);
                }
                Season *st = se;
                se = se->next;
                free(st->name);
                free(st);
            }
            free(s->name);
            free(s);
        }
        free(database[i]);
    }
    free(database);
}

/* ================= MAIN ================= */

int main(void) {
    int choice;
    while (1) {
        printf("Choose an option:\n1. Add\n2. Delete\n3. Print\n4. Exit\n");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            printf("Choose an option:\n1. Add a TV show\n2. Add a season\n3. Add an episode\n");
            scanf("%d", &choice);
            getchar();
            if (choice == 1) addTVShow();
            else if (choice == 2) addSeason();
            else if (choice == 3) addEpisode();
        }
        else if (choice == 4) {
            cleanup();
            break;
        }
    }
    return 0;
}

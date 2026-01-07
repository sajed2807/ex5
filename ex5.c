/*
-----------------------------------------
Name: Sajed Isa
ID: 325949089
Exercise: EX5
-----------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===================== Structs ===================== */

typedef struct Episode {
    char *name;
    char *length;
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

/* ===================== Globals ===================== */

TVShow ***database = NULL;
int dbSize = 0;

/* ===================== Utils ===================== */

void *safeMalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return p;
}

char *readLine() {
    int c, size = 32, len = 0;
    char *s = safeMalloc(size);
    while ((c = getchar()) != '\n' && c != EOF) {
        if (len + 1 >= size) {
            size *= 2;
            s = realloc(s, size);
        }
        s[len++] = (char)c;
    }
    s[len] = '\0';
    return s;
}

int readInt() {
    char *s = readLine();
    int x = atoi(s);
    free(s);
    return x;
}

/* ===================== Database ===================== */

int showCount() {
    if (!database) return 0;
    int c = 0;
    for (int i = 0; i < dbSize * dbSize; i++) {
        if (database[i / dbSize][i % dbSize]) c++;
        else break;
    }
    return c;
}

TVShow *getShow(int i) {
    return database[i / dbSize][i % dbSize];
}

void setShow(int i, TVShow *s) {
    database[i / dbSize][i % dbSize] = s;
}

int findShow(char *name) {
    int n = showCount();
    for (int i = 0; i < n; i++)
        if (strcmp(getShow(i)->name, name) == 0)
            return i;
    return -1;
}

void resizeDB(int newSize) {
    TVShow **newDB = safeMalloc(newSize * sizeof(TVShow *));
    for (int i = 0; i < newSize; i++) {
        newDB[i] = safeMalloc(newSize * sizeof(TVShow *));
        for (int j = 0; j < newSize; j++)
            newDB[i][j] = NULL;
    }

    int count = showCount();
    for (int i = 0; i < count; i++)
        newDB[i / newSize][i % newSize] = getShow(i);

    if (database) {
        for (int i = 0; i < dbSize; i++)
            free(database[i]);
        free(database);
    }

    database = newDB;
    dbSize = newSize;
}

/* ===================== Linked Lists ===================== */

Season *findSeason(TVShow *s, char *name) {
    for (Season *p = s->seasons; p; p = p->next)
        if (strcmp(p->name, name) == 0)
            return p;
    return NULL;
}

Episode *findEpisode(Season *s, char *name) {
    for (Episode *e = s->episodes; e; e = e->next)
        if (strcmp(e->name, name) == 0)
            return e;
    return NULL;
}

/* ===================== Add ===================== */

void addShow() {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    if (findShow(name) != -1) {
        printf("Show already exists.\n");
        free(name);
        return;
    }

    if (dbSize == 0)
        resizeDB(1);
    else if (showCount() == dbSize * dbSize)
        resizeDB(dbSize + 1);

    TVShow *s = safeMalloc(sizeof(TVShow));
    s->name = name;
    s->seasons = NULL;

    int count = showCount();
    int pos = 0;
    while (pos < count && strcmp(getShow(pos)->name, name) < 0)
        pos++;

    for (int i = count; i > pos; i--)
        setShow(i, getShow(i - 1));

    setShow(pos, s);
}

void addSeason() {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    int idx = findShow(showName);

    if (idx == -1) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = readLine();

    printf("Enter the position:\n");
    int pos = readInt();

    Season *s = safeMalloc(sizeof(Season));
    s->name = seasonName;
    s->episodes = NULL;
    s->next = NULL;

    TVShow *show = getShow(idx);
    if (pos == 0 || !show->seasons) {
        s->next = show->seasons;
        show->seasons = s;
    } else {
        Season *p = show->seasons;
        for (int i = 0; i < pos - 1 && p->next; i++)
            p = p->next;
        s->next = p->next;
        p->next = s;
    }

    free(showName);
}

void addEpisode() {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    int idx = findShow(showName);

    if (idx == -1) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *season = findSeason(getShow(idx), seasonName);

    if (!season) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }

    printf("Enter the name of the episode:\n");
    char *epName = readLine();

    printf("Enter the length:\n");
    char *len = readLine();

    printf("Enter the position:\n");
    int pos = readInt();

    Episode *e = safeMalloc(sizeof(Episode));
    e->name = epName;
    e->length = len;
    e->next = NULL;

    if (pos == 0 || !season->episodes) {
        e->next = season->episodes;
        season->episodes = e;
    } else {
        Episode *p = season->episodes;
        for (int i = 0; i < pos - 1 && p->next; i++)
            p = p->next;
        e->next = p->next;
        p->next = e;
    }

    free(showName);
    free(seasonName);
}

/* ===================== Menus ===================== */

void addMenu() {
    printf("Choose an option:\n");
    printf("1. Add TV show\n");
    printf("2. Add season\n");
    printf("3. Add episode\n");

    int c = readInt();
    if (c == 1) addShow();
    else if (c == 2) addSeason();
    else if (c == 3) addEpisode();
}

/* ===================== Print ===================== */

void printArray() {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j])
                printf("[%s] ", database[i][j]->name);
            else
                printf("[NULL] ");
        }
        printf("\n");
    }
}

/* ===================== Main ===================== */

int main() {
    while (1) {
        printf("Choose an option:\n");
        printf("1. Add\n");
        printf("2. Print\n");
        printf("3. Exit\n");

        int c = readInt();
        if (c == 1) addMenu();
        else if (c == 2) printArray();
        else if (c == 3) break;
    }

    printf("GOOD LUCK!\n");
    return 0;
}


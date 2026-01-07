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

/* ===================== Utilities ===================== */

void *safeMalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return p;
}

char *readLine() {
    int c;
    int size = 32, len = 0;
    char *str = safeMalloc(size);

    while ((c = getchar()) != '\n' && c != EOF) {
        if (len + 1 >= size) {
            size *= 2;
            str = realloc(str, size);
        }
        str[len++] = (char)c;
    }
    str[len] = '\0';
    return str;
}

int readInt() {
    char *s = readLine();
    int x = atoi(s);
    free(s);
    return x;
}

/* ===================== Database Helpers ===================== */

int showCount() {
    if (!database) return 0;
    int count = 0;
    for (int i = 0; i < dbSize * dbSize; i++) {
        if (database[i / dbSize][i % dbSize])
            count++;
        else
            break;
    }
    return count;
}

TVShow *getShow(int index) {
    return database[index / dbSize][index % dbSize];
}

void setShow(int index, TVShow *show) {
    database[index / dbSize][index % dbSize] = show;
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

Season *findSeason(TVShow *show, char *name) {
    for (Season *s = show->seasons; s; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

Episode *findEpisode(Season *season, char *name) {
    for (Episode *e = season->episodes; e; e = e->next)
        if (strcmp(e->name, name) == 0)
            return e;
    return NULL;
}

/* ===================== Add Functions ===================== */

void addShow() {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    if (findShow(name) != -1) {
        printf("Show already exists.\n");
        free(name);
        return;
    }

    int count = showCount();
    if (count == dbSize * dbSize)
        resizeDB(dbSize + 1);
    if (dbSize == 0)
        resizeDB(1);

    TVShow *show = safeMalloc(sizeof(TVShow));
    show->name = name;
    show->seasons = NULL;

    int pos = 0;
    while (pos < count && strcmp(getShow(pos)->name, name) < 0)
        pos++;

    for (int i = count; i > pos; i--)
        setShow(i, getShow(i - 1));

    setShow(pos, show);
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
        Season *curr = show->seasons;
        for (int i = 0; i < pos - 1 && curr->next; i++)
            curr = curr->next;
        s->next = curr->next;
        curr->next = s;
    }
    free(showName);
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
        printf("1. Add\n2. Print\n3. Exit\n");

        int choice = readInt();
        if (choice == 1)
            addShow();
        else if (choice == 2)
            printArray();
        else if (choice == 3)
            break;
    }

    printf("GOOD LUCK!\n");
    return 0;
}

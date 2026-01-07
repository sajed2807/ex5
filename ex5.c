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

/* database is a pointer to a 2D array of TVShow pointers */
TVShow ***database = NULL;
int dbSize = 0;

static void *safeMalloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return p;
}

static char *readLine(void) {
    int c;
    int cap = 32, len = 0;
    char *s = safeMalloc((size_t)cap);

    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            s = realloc(s, (size_t)cap);
            if (!s) {
                printf("Memory allocation failed\n");
                exit(1);
            }
        }
        s[len++] = (char)c;
    }
    s[len] = '\0';
    return s;
}

static int readInt(void) {
    char *s = readLine();
    int x = atoi(s);
    free(s);
    return x;
}

static int showCount(void) {
    if (!database || dbSize == 0) return 0;
    int c = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) c++;
            else return c;
        }
    }
    return c;
}

static TVShow *getShow(int i) {
    return database[i / dbSize][i % dbSize];
}

static void setShow(int i, TVShow *s) {
    database[i / dbSize][i % dbSize] = s;
}

static int findShow(const char *name) {
    int n = showCount();
    for (int i = 0; i < n; i++) {
        if (strcmp(getShow(i)->name, name) == 0) return i;
    }
    return -1;
}

/* Fixed pointer levels and casting in resizeDB */
static void resizeDB(int newSize) {
    /* Allocate array of rows (TVShow **) */
    TVShow **newDB = (TVShow *)safeMalloc((size_t)newSize * sizeof(TVShow *));

    for (int i = 0; i < newSize; i++) {
        /* Allocate each row (array of TVShow *) */
        newDB[i] = (TVShow **)safeMalloc((size_t)newSize * sizeof(TVShow *));
        for (int j = 0; j < newSize; j++) {
            newDB[i][j] = NULL;
        }
    }

    int count = showCount();
    for (int i = 0; i < count; i++) {
        newDB[i / newSize][i % newSize] = getShow(i);
    }

    /* Free old database structure */
    if (database) {
        for (int i = 0; i < dbSize; i++) {
            free(database[i]);
        }
        free(database);
    }

    database = newDB;
    dbSize = newSize;
}

static Season *findSeason(TVShow *s, const char *name) {
    for (Season *p = s->seasons; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

void addShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    if (findShow(name) != -1) {
        printf("Show already exists.\n");
        free(name);
        return;
    }

    int currentCount = showCount();
    if (dbSize == 0) resizeDB(1);
    else if (currentCount == dbSize * dbSize) resizeDB(dbSize + 1);

    TVShow *s = (TVShow *)safeMalloc(sizeof(TVShow));
    s->name = name;
    s->seasons = NULL;

    int count = showCount();
    int pos = 0;
    while (pos < count && strcmp(getShow(pos)->name, name) < 0) pos++;

    for (int i = count; i > pos; i--) {
        setShow(i, getShow(i - 1));
    }
    setShow(pos, s);
}

void addSeason(void) {
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

    Season *s = (Season *)safeMalloc(sizeof(Season));
    s->name = seasonName;
    s->episodes = NULL;
    s->next = NULL;

    TVShow *show = getShow(idx);
    if (pos <= 1 || show->seasons == NULL) {
        s->next = show->seasons;
        show->seasons = s;
    } else {
        Season *p = show->seasons;
        for (int i = 0; i < pos - 2 && p->next; i++) p = p->next;
        s->next = p->next;
        p->next = s;
    }

    free(showName);
}

void addEpisode(void) {
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

    Episode *e = (Episode *)safeMalloc(sizeof(Episode));
    e->name = epName;
    e->length = len;
    e->next = NULL;

    if (pos <= 1 || season->episodes == NULL) {
        e->next = season->episodes;
        season->episodes = e;
    } else {
        Episode *p = season->episodes;
        for (int i = 0; i < pos - 2 && p->next; i++) p = p->next;
        e->next = p->next;
        p->next = e;
    }

    free(showName);
    free(seasonName);
}

void addMenu(void) {
    printf("Choose an option:\n");
    printf("1. Add TV show\n");
    printf("2. Add season\n");
    printf("3. Add episode\n");

    int c = readInt();
    if (c == 1) addShow();
    else if (c == 2) addSeason();
    else if (c == 3) addEpisode();
}

void printArray(void) {
    if (dbSize == 0 || database == NULL) return;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j]) printf("[%s] ", database[i][j]->name);
            else printf("[NULL] ");
        }
        printf("\n");
    }
}

int main(void) {
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

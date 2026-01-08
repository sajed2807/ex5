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
#include <errno.h>

/* ---------- Structs ---------- */

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

/* ---------- Globals ---------- */

static TVShow ***database = NULL;
static int dbSize = 0;

/* ---------- Helpers ---------- */

static void *safeMalloc(size_t n) {
    if (n == 0) n = 1;

    void *p = malloc(n);
    if (!p) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return p;
}

static char *safeStrdup(const char *s) {
    size_t len = strlen(s);
    char *out = safeMalloc(len + 1);
    memcpy(out, s, len + 1);
    return out;
}

static char *readLine(void) {
    char buffer[1024];

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        char *empty = safeMalloc(1);
        empty[0] = '\0';
        return empty;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
    return safeStrdup(buffer);
}

static int readInt(void) {
    char line[128];

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) {
            return -1;
        }

        errno = 0;
        char *endptr = NULL;
        long v = strtol(line, &endptr, 10);

        while (endptr && (*endptr == ' ' || *endptr == '\t')) endptr++;

        if (errno == 0 && endptr && (*endptr == '\0' || *endptr == '\n')) {
            return (int)v;
        }

        printf("Invalid input, try again:\n");
    }
}

static int countShows(void) {
    if (!database || dbSize == 0) return 0;

    int count = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j]) count++;
        }
    }
    return count;
}

/* ---------- Database Resize ---------- */

static void resizeDatabase(int newSize) {
    TVShow ***newDB = safeMalloc((size_t)newSize * sizeof(*newDB));

    for (int i = 0; i < newSize; i++) {
        newDB[i] = safeMalloc((size_t)newSize * sizeof(*newDB[i]));
        for (int j = 0; j < newSize; j++) {
            newDB[i][j] = NULL;
        }
    }

    if (database) {
        int idx = 0;
        for (int i = 0; i < dbSize; i++) {
            for (int j = 0; j < dbSize; j++) {
                if (database[i][j]) {
                    newDB[idx / newSize][idx % newSize] = database[i][j];
                    idx++;
                }
            }
            free(database[i]);
        }
        free(database);
    }

    database = newDB;
    dbSize = newSize;
}

/* ---------- Free ---------- */

static void freeEpisodes(Episode *e) {
    while (e) {
        Episode *tmp = e;
        e = e->next;
        free(tmp->name);
        free(tmp->length);
        free(tmp);
    }
}

static void freeSeasons(Season *s) {
    while (s) {
        Season *tmp = s;
        s = s->next;
        freeEpisodes(tmp->episodes);
        free(tmp->name);
        free(tmp);
    }
}

/* ---------- Find / Collect ---------- */

static TVShow *findShowByName(const char *name) {
    if (!database) return NULL;

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] &&
                strcmp(database[i][j]->name, name) == 0) {
                return database[i][j];
            }
        }
    }
    return NULL;
}

static TVShow **collectShows(int *outN) {
    int n = countShows();
    *outN = n;

    if (n == 0) return NULL;

    TVShow **arr = safeMalloc((size_t)n * sizeof(*arr));
    int idx = 0;

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j]) {
                arr[idx++] = database[i][j];
            }
        }
    }
    return arr;
}

static void refillDatabasePacked(TVShow **arr, int n) {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            int k = i * dbSize + j;
            database[i][j] = (k < n) ? arr[k] : NULL;
        }
    }
}

/* ---------- Core ---------- */

void addShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    if (name[0] == '\0' || findShowByName(name)) {
        printf("Invalid or duplicate show.\n");
        free(name);
        return;
    }

    int count = countShows();
    if (dbSize == 0) resizeDatabase(1);
    else if (count == dbSize * dbSize) resizeDatabase(dbSize + 1);

    TVShow *newShow = safeMalloc(sizeof(*newShow));
    newShow->name = name;
    newShow->seasons = NULL;

    int n = 0;
    TVShow **old = collectShows(&n);
    TVShow **sorted = safeMalloc((size_t)(n + 1) * sizeof(*sorted));

    int i = 0, j = 0;
    int inserted = 0;

    while (i < n) {
        if (!inserted &&
            strcmp(newShow->name, old[i]->name) < 0) {
            sorted[j++] = newShow;
            inserted = 1;
        } else {
            sorted[j++] = old[i++];
        }
    }
    if (!inserted) sorted[j++] = newShow;

    refillDatabasePacked(sorted, j);

    free(old);
    free(sorted);
}

void addSeason(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();

    TVShow *show = findShowByName(showName);
    if (!show) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *name = readLine();

    for (Season *s = show->seasons; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            printf("Season already exists.\n");
            free(showName);
            free(name);
            return;
        }
    }

    Season *s = safeMalloc(sizeof(*s));
    s->name = name;
    s->episodes = NULL;
    s->next = show->seasons;
    show->seasons = s;

    free(showName);
}

void addEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *show = findShowByName(showName);

    if (!show) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *s = show->seasons;

    while (s && strcmp(s->name, seasonName) != 0) s = s->next;
    if (!s) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }

    printf("Enter episode name:\n");
    char *eName = readLine();
    printf("Enter episode length:\n");
    char *len = readLine();

    Episode *e = safeMalloc(sizeof(*e));
    e->name = eName;
    e->length = len;
    e->next = s->episodes;
    s->episodes = e;

    free(showName);
    free(seasonName);
}

void printDatabase(void) {
    if (!database) {
        printf("(empty)\n");
        return;
    }

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j]) {
                printf("Show: %s\n", database[i][j]->name);
            }
        }
    }
}

/* ---------- Cleanup ---------- */

static void cleanup(void) {
    if (!database) return;

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j]) {
                freeSeasons(database[i][j]->seasons);
                free(database[i][j]->name);
                free(database[i][j]);
            }
        }
        free(database[i]);
    }
    free(database);
}

int main(void) {
    while (1) {
        printf("1. Add TV Show\n"
               "2. Add Season\n"
               "3. Add Episode\n"
               "4. Print Database\n"
               "5. Exit\n");

        int c = readInt();
        if (c == 1) addShow();
        else if (c == 2) addSeason();
        else if (c == 3) addEpisode();
        else if (c == 4) printDatabase();
        else if (c == 5) break;
    }

    cleanup();
    return 0;
}

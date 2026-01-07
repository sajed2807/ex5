
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
    /* malloc(0) is implementation-defined; avoid it */
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

/* Read a full line safely (without trailing '\n'). Returns heap string. */
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

/* Read an integer choice safely using fgets + strtol. */
static int readInt(void) {
    char line[128];

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) {
            return -1; /* EOF */
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
    if (dbSize == 0 || database == NULL) return 0;

    int count = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) count++;
        }
    }
    return count;
}

/* Create a new newSize x newSize matrix and pack old shows row-major */
static void resizeDatabase(int newSize) {
    TVShow **newDB = safeMalloc((size_t)newSize * sizeof(*newDB)); / TVShow*** */

    for (int i = 0; i < newSize; i++) {
        newDB[i] = safeMalloc((size_t)newSize * sizeof(newDB[i])); / TVShow**  */
        for (int j = 0; j < newSize; j++) {
            newDB[i][j] = NULL;                                     /* TVShow*   */
        }
    }

    if (database != NULL) {
        int count = 0;
        for (int i = 0; i < dbSize; i++) {
            for (int j = 0; j < dbSize; j++) {
                if (database[i][j] != NULL) {
                    newDB[count / newSize][count % newSize] = database[i][j];
                    count++;
                }
            }
            free(database[i]);
        }
        free(database);
    }

    database = newDB;
    dbSize = newSize;
}

/* ---------- Free linked-lists ---------- */

static void freeEpisodes(Episode *head) {
    while (head) {
        Episode *temp = head;
        head = head->next;
        free(temp->name);
        free(temp->length);
        free(temp);
    }
}

static void freeSeasons(Season *head) {
    while (head) {
        Season *temp = head;
        head = head->next;
        freeEpisodes(temp->episodes);
        free(temp->name);
        free(temp);
    }
}

/* ---------- Find / Collect / Refill ---------- */

static TVShow *findShowByName(const char *name) {
    if (!database) return NULL;

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] && strcmp(database[i][j]->name, name) == 0) {
                return database[i][j];
            }
        }
    }
    return NULL;
}

/* Pack all existing shows into an array (size = countShows). */
static TVShow **collectShows(int *outN) {
    int n = countShows();
    *outN = n;

    if (n == 0) {
        return NULL;
    }

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

/* ---------- Core Logic ---------- */

void addShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    if (name[0] == '\0') {
        printf("Invalid show name.\n");
        free(name);
        return;
    }

    if (findShowByName(name)) {
        printf("Show already exists.\n");
        free(name);
        return;
    }

    int currentCount = countShows();
    if (dbSize == 0) {
        resizeDatabase(1);
    } else if (currentCount == dbSize * dbSize) {
        resizeDatabase(dbSize + 1);
    }

    TVShow *newShow = safeMalloc(sizeof(*newShow));
    newShow->name = name;
    newShow->seasons = NULL;

    int n = 0;
    TVShow *tempList = collectShows(&n); / may be NULL if n==0 */

    TVShow **newList = safeMalloc((size_t)(n + 1) * sizeof(*newList));

    int inserted = 0;
    int a = 0, b = 0;

    while (a < n) {
        if (!inserted && strcmp(newShow->name, tempList[a]->name) < 0) {
            newList[b++] = newShow;
            inserted = 1;
        } else {
            newList[b++] = tempList[a++];
        }
    }
    if (!inserted) {
        newList[b++] = newShow;
    }

    refillDatabasePacked(newList, b);

    free(tempList);
    free(newList);
}

void addSeason(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();

    TVShow *target = findShowByName(showName);
    if (!target) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *sName = readLine();

    if (sName[0] == '\0') {
        printf("Invalid season name.\n");
        free(showName);
        free(sName);
        return;
    }

    /* prevent duplicate season names */
    for (Season *scan = target->seasons; scan; scan = scan->next) {
        if (strcmp(scan->name, sName) == 0) {
            printf("Season already exists.\n");
            free(showName);
            free(sName);
            return;
        }
    }

    Season *newS = safeMalloc(sizeof(*newS));
    newS->name = sName;
    newS->episodes = NULL;
    newS->next = NULL;

    if (!target->seasons) {
        target->seasons = newS;
    } else {
        Season *curr = target->seasons;
        while (curr->next) curr = curr->next;
        curr->next = newS;
    }

    free(showName);
}

void addEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();

    TVShow *target = findShowByName(showName);
    if (!target) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *sName = readLine();

    Season *currS = target->seasons;
    while (currS && strcmp(currS->name, sName) != 0) {
        currS = currS->next;
    }

    if (!currS) {
        printf("Season not found.\n");
        free(showName);
        free(sName);
        return;
    }

    printf("Enter the name of the episode:\n");
    char *eName = readLine();

    printf("Enter the length (hh:mm:ss):\n");
    char *len = readLine();

    if (eName[0] == '\0' || len[0] == '\0') {
        printf("Invalid episode data.\n");
        free(showName);
        free(sName);
        free(eName);
        free(len);
        return;
    }

    /* prevent duplicate episode names */
    for (Episode *scan = currS->episodes; scan; scan = scan->next) {
        if (strcmp(scan->name, eName) == 0) {
            printf("Episode already exists.\n");
            free(showName);
            free(sName);
            free(eName);
            free(len);
            return;
        }
    }

    Episode *newE = safeMalloc(sizeof(*newE));
    newE->name = eName;
    newE->length = len;
    newE->next = NULL;

    if (!currS->episodes) {
        currS->episodes = newE;
    } else {
        Episode *currE = currS->episodes;
        while (currE->next) currE = currE->next;
        currE->next = newE;
    }

    free(showName);
    free(sName);
}

void printDatabase(void) {
    if (!database || dbSize == 0) {
        printf("(empty)\n");
        return;
    }

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j]) {
                printf("Show: %s\n", database[i][j]->name);
                Season *s = database[i][j]->seasons;
                int sIdx = 1;
                while (s) {
                    printf("  Season %d: %s\n", sIdx++, s->name);
                    Episode *e = s->episodes;
                    while (e) {
                        printf("    Episode: %s (%s)\n", e->name, e->length);
                        e = e->next;
                    }
                    s = s->next;
                }
            }
        }
    }
}

/* ---------- Cleanup ---------- */

static void cleanup(void) {
    if (database != NULL) {
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
}

/* ---------- Main ---------- */

int main(void) {
    while (1) {
        printf("1. Add TV Show\n"
               "2. Add Season\n"
               "3. Add Episode\n"
               "4. Print Database\n"
               "5. Exit\n");

        int choice = readInt();
        if (choice == -1) break;

        if (choice == 1) addShow();
        else if (choice == 2) addSeason();
        else if (choice == 3) addEpisode();
        else if (choice == 4) printDatabase();
        else if (choice == 5) break;
        else printf("Invalid choice.\n");
    }

    cleanup();
    printf("GOOD LUCK!\n");
    return 0;
}

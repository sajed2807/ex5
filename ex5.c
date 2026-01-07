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

TVShow ***database = NULL;
int dbSize = 0;

/* --- Helper Functions --- */

static void *safeMalloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return p;
}

static char *readLine(void) {
    char buffer[1024];
    scanf(" %[^\n]", buffer);
    char *s = safeMalloc(strlen(buffer) + 1);
    strcpy(s, buffer);
    return s;
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

/* ✅ FIXED: no casts + no unused variables */
static void resizeDatabase(int newSize) {
    TVShow **newDB = safeMalloc((size_t)newSize * sizeof(*newDB));          / TVShow*** */

    for (int i = 0; i < newSize; i++) {
        newDB[i] = safeMalloc((size_t)newSize * sizeof(newDB[i]));          / TVShow**  */
        for (int j = 0; j < newSize; j++) {
            newDB[i][j] = NULL;                                              /* TVShow*   */
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

/* --- Linked List Management --- */

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

/* --- Core Logic --- */

void addShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();

    /* Check if exists */
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database != NULL && database[i][j] && strcmp(database[i][j]->name, name) == 0) {
                printf("Show already exists.\n");
                free(name);
                return;
            }
        }
    }

    int currentCount = countShows();
    if (dbSize == 0) resizeDatabase(1);
    else if (currentCount == dbSize * dbSize) resizeDatabase(dbSize + 1);

    TVShow *newShow = safeMalloc(sizeof(TVShow));
    newShow->name = name;
    newShow->seasons = NULL;

    /* Insert sorted */
    TVShow *tempList[100];
    int n = countShows();
    int idx = 0;

    for (int i = 0; i < dbSize; i++)
        for (int j = 0; j < dbSize; j++)
            if (database[i][j]) tempList[idx++] = database[i][j];

    int pos = 0;
    while (pos < n && strcmp(tempList[pos]->name, name) < 0) pos++;

    for (int i = n; i > pos; i--) tempList[i] = tempList[i - 1];
    tempList[pos] = newShow;
    n++;

    /* Clear and refill database packed row-major */
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            int k = i * dbSize + j;
            database[i][j] = (k < n) ? tempList[k] : NULL;
        }
    }
}

void addSeason(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();

    TVShow *target = NULL;
    for (int i = 0; i < dbSize; i++)
        for (int j = 0; j < dbSize; j++)
            if (database[i][j] && strcmp(database[i][j]->name, showName) == 0) target = database[i][j];

    if (!target) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *sName = readLine();

    Season *newS = safeMalloc(sizeof(Season));
    newS->name = sName;
    newS->episodes = NULL;
    newS->next = NULL;

    if (!target->seasons) target->seasons = newS;
    else {
        Season *curr = target->seasons;
        while (curr->next) curr = curr->next;
        curr->next = newS;
    }

    free(showName);
}

void addEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();

    TVShow *target = NULL;
    for (int i = 0; i < dbSize; i++)
        for (int j = 0; j < dbSize; j++)
            if (database[i][j] && strcmp(database[i][j]->name, showName) == 0) target = database[i][j];

    if (!target) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *sName = readLine();

    Season *currS = target->seasons;
    while (currS && strcmp(currS->name, sName) != 0) currS = currS->next;

    if (!currS) {
        printf("Season not found.\n");
        free(showName);
        free(sName);
        return;
    }

    Episode *newE = safeMalloc(sizeof(Episode));

    printf("Enter the name of the episode:\n");
    newE->name = readLine();

    printf("Enter the length (hh:mm:ss):\n");
    newE->length = readLine();

    newE->next = NULL;

    if (!currS->episodes) currS->episodes = newE;
    else {
        Episode *currE = currS->episodes;
        while (currE->next) currE = currE->next;
        currE->next = newE;
    }

    free(showName);
    free(sName);
}

void printDatabase(void) {
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

int main(void) {
    int choice;

    while (1) {
        printf("1. Add TV Show\n2. Add Season\n3. Add Episode\n4. Print Database\n5. Exit\n");
        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) addShow();
        else if (choice == 2) addSeason();
        else if (choice == 3) addEpisode();
        else if (choice == 4) printDatabase();
        else if (choice == 5) break;
    }

    /* Cleanup */
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

    printf("GOOD LUCK!\n");
    return 0;
}

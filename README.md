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

static TVShow **shows = NULL;
static int showCount = 0;

/* ---------- Helpers ---------- */

static void *safeMalloc(size_t n) {
    void *p = malloc(n);
    if (!p) exit(1);
    return p;
}

static char *readLine(void) {
    char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        char *e = safeMalloc(1);
        e[0] = '\0';
        return e;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *out = safeMalloc(strlen(buffer) + 1);
    strcpy(out, buffer);
    return out;
}

static int readInt(void) {
    char line[128];
    if (!fgets(line, sizeof(line), stdin)) return -1;
    return atoi(line);
}

/* ---------- Find ---------- */

static TVShow *findShow(const char *name) {
    for (int i = 0; i < showCount; i++) {
        if (strcmp(shows[i]->name, name) == 0)
            return shows[i];
    }
    return NULL;
}

/* ---------- Add Show ---------- */

void addShow(void) {
    char *name = readLine();
    if (name[0] == '\0' || findShow(name)) {
        free(name);
        return;
    }

    TVShow *s = safeMalloc(sizeof(TVShow));
    s->name = name;
    s->seasons = NULL;

    shows = realloc(shows, (showCount + 1) * sizeof(TVShow *));
    shows[showCount++] = s;
}

/* ---------- Add Season ---------- */

void addSeason(void) {
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        free(showName);
        return;
    }

    char *seasonName = readLine();
    Season *scan = s->seasons;
    while (scan) {
        if (strcmp(scan->name, seasonName) == 0) {
            free(showName);
            free(seasonName);
            return;
        }
        scan = scan->next;
    }

    Season *newS = safeMalloc(sizeof(Season));
    newS->name = seasonName;
    newS->episodes = NULL;
    newS->next = s->seasons;
    s->seasons = newS;

    free(showName);
}

/* ---------- Add Episode ---------- */

void addEpisode(void) {
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        free(showName);
        return;
    }

    char *seasonName = readLine();
    Season *season = s->seasons;
    while (season && strcmp(season->name, seasonName) != 0)
        season = season->next;

    if (!season) {
        free(showName);
        free(seasonName);
        return;
    }

    char *epName = readLine();
    char *len = readLine();

    Episode *e = safeMalloc(sizeof(Episode));
    e->name = epName;
    e->length = len;
    e->next = season->episodes;
    season->episodes = e;

    free(showName);
    free(seasonName);
}

/* ---------- Print ---------- */

void printDatabase(void) {
    for (int i = 0; i < showCount; i++) {
        printf("Show: %s\n", shows[i]->name);
        Season *s = shows[i]->seasons;
        while (s) {
            printf("  Season: %s\n", s->name);
            Episode *e = s->episodes;
            while (e) {
                printf("    Episode: %s (%s)\n", e->name, e->length);
                e = e->next;
            }
            s = s->next;
        }
    }
}

/* ---------- Cleanup ---------- */

void cleanup(void) {
    for (int i = 0; i < showCount; i++) {
        Season *s = shows[i]->seasons;
        while (s) {
            Episode *e = s->episodes;
            while (e) {
                Episode *et = e;
                e = e->next;
                free(et->name);
                free(et->length);
                free(et);
            }
            Season *st = s;
            s = s->next;
            free(st->name);
            free(st);
        }
        free(shows[i]->name);
        free(shows[i]);
    }
    free(shows);
}

/* ---------- Main ---------- */

int main(void) {
    while (1) {
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

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
#include <ctype.h>

/* ===================== Structs ===================== */

typedef struct Episode {
    char *name;
    char length;              / xx:xx:xx */
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

static TVShow **shows = NULL;
static int showCount = 0;

/* ===================== Helpers ===================== */

static void *xmalloc(size_t n) {
    void *p = malloc(n ? n : 1);
    if (!p) exit(1);
    return p;
}

static char *xstrdup(const char *s) {
    char *p = xmalloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

static char *readLine(void) {
    char buf[1024];
    if (!fgets(buf, sizeof(buf), stdin)) {
        char *e = xmalloc(1);
        e[0] = '\0';
        return e;
    }
    buf[strcspn(buf, "\n")] = '\0';
    return xstrdup(buf);
}

static int readInt(void) {
    char line[128];
    if (!fgets(line, sizeof(line), stdin)) return -1;
    return atoi(line);
}

/* validate length xx:xx:xx with ranges 0–99:0–59:0–59 */
static int validLength(const char *s) {
    if (strlen(s) != 8) return 0;
    if (!isdigit(s[0]) || !isdigit(s[1]) || s[2] != ':' ||
        !isdigit(s[3]) || !isdigit(s[4]) || s[5] != ':' ||
        !isdigit(s[6]) || !isdigit(s[7])) return 0;
    int hh = (s[0]-'0')*10 + (s[1]-'0');
    int mm = (s[3]-'0')*10 + (s[4]-'0');
    int ss = (s[6]-'0')*10 + (s[7]-'0');
    if (hh < 0 || hh > 99) return 0;
    if (mm < 0 || mm > 59) return 0;
    if (ss < 0 || ss > 59) return 0;
    return 1;
}

/* ===================== Find ===================== */

static TVShow *findShow(const char *name) {
    for (int i = 0; i < showCount; i++)
        if (strcmp(shows[i]->name, name) == 0)
            return shows[i];
    return NULL;
}

static Season *findSeason(TVShow *s, const char *name) {
    for (Season *p = s->seasons; p; p = p->next)
        if (strcmp(p->name, name) == 0)
            return p;
    return NULL;
}

static Episode *findEpisode(Season *s, const char *name) {
    for (Episode *e = s->episodes; e; e = e->next)
        if (strcmp(e->name, name) == 0)
            return e;
    return NULL;
}

/* ===================== Add ===================== */

static void addTVShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();
    if (name[0] == '\0') { free(name); return; }
    if (findShow(name)) {
        printf("Show already exists.\n");
        free(name);
        return;
    }
    TVShow *s = xmalloc(sizeof(*s));
    s->name = name;
    s->seasons = NULL;
    shows = realloc(shows, (showCount + 1) * sizeof(TVShow *));
    shows[showCount++] = s;
}

static void addSeason(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    if (findSeason(s, seasonName)) {
        printf("Season already exists.\n");
        free(showName);
        free(seasonName);
        return;
    }
    Season *se = xmalloc(sizeof(*se));
    se->name = seasonName;
    se->episodes = NULL;
    se->next = s->seasons;
    s->seasons = se;
    free(showName);
}

static void addEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *se = findSeason(s, seasonName);
    if (!se) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }
    printf("Enter the name of the episode:\n");
    char *epName = readLine();
    if (findEpisode(se, epName)) {
        printf("Episode already exists.\n");
        free(showName); free(seasonName); free(epName);
        return;
    }
    printf("Enter the length (xx:xx:xx):\n");
    char *len = readLine();
    while (!validLength(len)) {
        printf("Invalid length, enter again:\n");
        free(len);
        len = readLine();
    }
    Episode *e = xmalloc(sizeof(*e));
    e->name = epName;
    e->length = len;
    e->next = se->episodes;
    se->episodes = e;
    free(showName);
    free(seasonName);
}

/* ===================== Delete ===================== */

static void deleteTVShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();
    for (int i = 0; i < showCount; i++) {
        if (strcmp(shows[i]->name, name) == 0) {
            /* free seasons & episodes */
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
            for (int j = i; j < showCount-1; j++)
                shows[j] = shows[j+1];
            showCount--;
            free(name);
            return;
        }
    }
    printf("Show not found.\n");
    free(name);
}

static void deleteSeason(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *prev = NULL, *cur = s->seasons;
    while (cur && strcmp(cur->name, seasonName) != 0) {
        prev = cur; cur = cur->next;
    }
    if (!cur) {
        printf("Season not found.\n");
        free(showName); free(seasonName);
        return;
    }
    if (prev) prev->next = cur->next;
    else s->seasons = cur->next;
    Episode *e = cur->episodes;
    while (e) {
        Episode *et = e;
        e = e->next;
        free(et->name);
        free(et->length);
        free(et);
    }
    free(cur->name);
    free(cur);
    free(showName);
    free(seasonName);
}

static void deleteEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *se = findSeason(s, seasonName);
    if (!se) {
        printf("Season not found.\n");
        free(showName); free(seasonName);
        return;
    }
    printf("Enter the name of the episode:\n");
    char *epName = readLine();
    Episode *prev = NULL, *cur = se->episodes;
    while (cur && strcmp(cur->name, epName) != 0) {
        prev = cur; cur = cur->next;
    }
    if (!cur) {
        printf("Episode not found.\n");
        free(showName); free(seasonName); free(epName);
        return;
    }
    if (prev) prev->next = cur->next;
    else se->episodes = cur->next;
    free(cur->name);
    free(cur->length);
    free(cur);
    free(showName); free(seasonName); free(epName);
}

/* ===================== Print ===================== */

static void printTVShow(void) {
    printf("Enter the name of the show:\n");
    char *name = readLine();
    TVShow *s = findShow(name);
    if (!s) {
        printf("Show not found.\n");
        free(name);
        return;
    }
    printf("[%s]\n", s->name);
    for (Season *se = s->seasons; se; se = se->next) {
        printf("\t(%s)\n", se->name);
        for (Episode *e = se->episodes; e; e = e->next)
            printf("\t\t%s %s\n", e->name, e->length);
    }
    free(name);
}

static void printEpisode(void) {
    printf("Enter the name of the show:\n");
    char *showName = readLine();
    TVShow *s = findShow(showName);
    if (!s) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = readLine();
    Season *se = findSeason(s, seasonName);
    if (!se) {
        printf("Season not found.\n");
        free(showName); free(seasonName);
        return;
    }
    printf("Enter the name of the episode:\n");
    char *epName = readLine();
    Episode *e = findEpisode(se, epName);
    if (!e) {
        printf("Episode not found.\n");
        free(showName); free(seasonName); free(epName);
        return;
    }
    printf("%s %s\n", e->name, e->length);
    free(showName); free(seasonName); free(epName);
}

static void printArray(void) {
    for (int i = 0; i < showCount; i++) {
        printf("[%s]", shows[i]->name);
        if ((i+1) % 2 == 0 || i == showCount-1) printf("\n");
        else printf(" ");
    }
}

/* ===================== Menus ===================== */

static void addMenu(void) {
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    int c = readInt();
    if (c == 1) addTVShow();
    else if (c == 2) addSeason();
    else if (c == 3) addEpisode();
}

static void deleteMenu(void) {
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    int c = readInt();
    if (c == 1) deleteTVShow();
    else if (c == 2) deleteSeason();
    else if (c == 3) deleteEpisode();
}

static void printMenu(void) {
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    int c = readInt();
    if (c == 1) printTVShow();
    else if (c == 2) printEpisode();
    else if (c == 3) printArray();
}

/* ===================== Cleanup ===================== */

static void cleanup(void) {
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

/* ===================== Main ===================== */

int main(void) {
    while (1) {
        printf("Choose an option:\n");
        printf("1. Add\n");
        printf("2. Delete\n");
        printf("3. Print\n");
        printf("4. Exit\n");
        int c = readInt();
        if (c == 1) addMenu();
        else if (c == 2) deleteMenu();
        else if (c == 3) printMenu();
        else if (c == 4) break;
    }
    cleanup();
    return 0;
}

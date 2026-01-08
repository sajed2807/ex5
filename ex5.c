/*
Name: Sajad Isa
ID: 325949089
Exercise: ex5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Episode {
    char *name;
    char *length;              // format XX:XX:XX
    struct Episode *next;
} Episode;

typedef struct Season {
    int number;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

/* -------- Globals -------- */
TVShow **shows = NULL;
int showsCount = 0;

/* -------- Utilities -------- */
char *readLine(void) {
    char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return NULL;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *s = malloc(strlen(buffer) + 1);
    strcpy(s, buffer);
    return s;
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

/* -------- Core Functions -------- */
TVShow *findShow(const char *name) {
    for (int i = 0; i < showsCount; i++) {
        if (strcmp(shows[i]->name, name) == 0)
            return shows[i];
    }
    return NULL;
}

Season *findSeason(TVShow *show, int number) {
    Season *s = show->seasons;
    while (s) {
        if (s->number == number)
            return s;
        s = s->next;
    }
    return NULL;
}

void addShow(void) {
    char *name = readLine();
    if (findShow(name)) {
        printf("Show already exists.\n");
        free(name);
        return;
    }

    TVShow *s = malloc(sizeof(TVShow));
    s->name = name;
    s->seasons = NULL;

    shows = realloc(shows, sizeof(TVShow*) * (showsCount + 1));
    shows[showsCount++] = s;
}

void addSeason(void) {
    char *showName = readLine();
    TVShow *show = findShow(showName);
    free(showName);

    if (!show) {
        printf("Show not found.\n");
        return;
    }

    int num;
    scanf("%d", &num);
    getchar();

    if (findSeason(show, num)) {
        printf("Season already exists.\n");
        return;
    }

    Season *s = malloc(sizeof(Season));
    s->number = num;
    s->episodes = NULL;
    s->next = show->seasons;
    show->seasons = s;
}

void addEpisode(void) {
    char *showName = readLine();
    TVShow *show = findShow(showName);
    free(showName);

    if (!show) {
        printf("Show not found.\n");
        return;
    }

    int seasonNum;
    scanf("%d", &seasonNum);
    getchar();

    Season *season = findSeason(show, seasonNum);
    if (!season) {
        printf("Season not found.\n");
        return;
    }

    char *epName = readLine();
    char *len = readLine();

    if (!validLength(len)) {
        printf("Invalid length.\n");
        free(epName);
        free(len);
        return;
    }

    Episode *e = malloc(sizeof(Episode));
    e->name = epName;
    e->length = len;
    e->next = season->episodes;
    season->episodes = e;
}

void printAll(void) {
    for (int i = 0; i < showsCount; i++) {
        TVShow *s = shows[i];
        printf("%s\n", s->name);
        Season *se = s->seasons;
        while (se) {
            printf("\tSeason %d\n", se->number);
            Episode *e = se->episodes;
            while (e) {
                printf("\t\t%s %s\n", e->name, e->length);
                e = e->next;
            }
            se = se->next;
        }
    }
}

void cleanup(void) {
    for (int i = 0; i < showsCount; i++) {
        TVShow *s = shows[i];
        Season *se = s->seasons;
        while (se) {
            Episode *e = se->episodes;
            while (e) {
                Episode *tmpE = e;
                e = e->next;
                free(tmpE->name);
                free(tmpE->length);
                free(tmpE);
            }
            Season *tmpS = se;
            se = se->next;
            free(tmpS);
        }
        free(s->name);
        free(s);
    }
    free(shows);
}

/* -------- Main -------- */
int main(void) {
    int choice;
    do {
        printf("Choose an option:\n");
        printf("1. Add\n2. Delete\n3. Print\n4. Exit\n");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            int sub;
            printf("1. Add a TV show\n2. Add a season\n3. Add an episode\n");
            scanf("%d", &sub);
            getchar();
            if (sub == 1) addShow();
            else if (sub == 2) addSeason();
            else if (sub == 3) addEpisode();
        } else if (choice == 3) {
            printAll();
        }

    } while (choice != 4);

    cleanup();
    return 0;
}

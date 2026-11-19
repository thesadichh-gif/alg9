#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <queue>

using namespace std;

struct UzelSpiska {
    int versh;
    struct UzelSpiska* sled;
};

struct UzelSpiska* sozd_uzel(int v) {
    struct UzelSpiska* nov = (struct UzelSpiska*)malloc(sizeof(struct UzelSpiska));
    if (nov != NULL) {
        nov->versh = v;
        nov->sled = NULL;
    }
    return nov;
}

int** sozd_matr(int n, int ver) {
    int** G = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        G[i] = (int*)malloc(n * sizeof(int));
    }

    for (int i = 0; i < n; i++) {
        G[i][i] = 0;
        for (int j = i + 1; j < n; j++) {
            int val = (rand() % 100 < ver) ? 1 : 0;
            G[i][j] = val;
            G[j][i] = val;
        }
    }
    return G;
}

void ochist_matr(int** G, int n) {
    if (G == NULL) return;
    for (int i = 0; i < n; i++) {
        free(G[i]);
    }
    free(G);
}

void vivod_matr(int** G, int n) {
    printf("\nМатрица смежности (%dx%d)\n     ", n, n);
    for (int j = 0; j < n; j++) {
        printf("v%-3d", j);
    }
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("v%-3d ", i);
        for (int j = 0; j < n; j++) {
            printf("%-4d", G[i][j]);
        }
        printf("\n");
    }
}

struct UzelSpiska** sozd_spiski(int n, int** G) {
    struct UzelSpiska** spiski = (struct UzelSpiska**)malloc(n * sizeof(struct UzelSpiska*));
    for (int i = 0; i < n; i++) {
        spiski[i] = NULL;
    }

    for (int i = 0; i < n; i++) {
        struct UzelSpiska* hvost = NULL;
        for (int j = 0; j < n; j++) {
            if (G[i][j] == 1) {
                struct UzelSpiska* nov = sozd_uzel(j);
                if (spiski[i] == NULL) {
                    spiski[i] = hvost = nov;
                } else {
                    hvost->sled = nov;
                    hvost = nov;
                }
            }
        }
    }
    return spiski;
}

void ochist_spiski(struct UzelSpiska** spiski, int n) {
    if (spiski == NULL) return;
    for (int i = 0; i < n; i++) {
        struct UzelSpiska* tek = spiski[i];
        while (tek != NULL) {
            struct UzelSpiska* tmp = tek;
            tek = tek->sled;
            free(tmp);
        }
    }
    free(spiski);
}

void vivod_spiskov(struct UzelSpiska** spiski, int n) {
    printf("\nСписки смежности\n");
    for (int i = 0; i < n; i++) {
        printf("v%d: ", i);
        struct UzelSpiska* tek = spiski[i];
        if (tek == NULL) {
            printf("[нет смежных вершин]");
        }
        while (tek != NULL) {
            printf("-> v%d ", tek->versh);
            tek = tek->sled;
        }
        printf("\n");
    }
}

void bfs_matr(int start, int n, int** G, int* dist) {
    for (int i = 0; i < n; i++) dist[i] = -1;

    queue<int> Q;
    Q.push(start);
    dist[start] = 0;

    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();
        for (int i = 0; i < n; i++) {
            if (G[v][i] == 1 && dist[i] == -1) {
                Q.push(i);
                dist[i] = dist[v] + 1;
            }
        }
    }
}

void bfs_spis(int start, int n, struct UzelSpiska** spiski, int* dist) {
    for (int i = 0; i < n; i++) dist[i] = -1;

    queue<int> Q;
    Q.push(start);
    dist[start] = 0;

    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();

        struct UzelSpiska* tek = spiski[v];
        while (tek != NULL) {
            int to = tek->versh;
            if (dist[to] == -1) {
                dist[to] = dist[v] + 1;
                Q.push(to);
            }
            tek = tek->sled;
        }
    }
}

void dfs_helper_matr(int v, int n, int** G, int* dist, int current_dist) {
    dist[v] = current_dist;

    for (int i = 0; i < n; i++) {
        if (G[v][i] == 1) {
            if (dist[i] == -1 || current_dist + 1 < dist[i]) {
                dfs_helper_matr(i, n, G, dist, current_dist + 1);
            }
        }
    }
}

void dfs_matr(int start, int n, int** G, int* dist) {
    for (int i = 0; i < n; i++) dist[i] = -1;
    dfs_helper_matr(start, n, G, dist, 0);
}

void dfs_helper_spis(int v, struct UzelSpiska** spiski, int* dist, int current_dist) {
    dist[v] = current_dist;

    struct UzelSpiska* tek = spiski[v];
    while (tek != NULL) {
        int to = tek->versh;
        if (dist[to] == -1 || current_dist + 1 < dist[to]) {
            dfs_helper_spis(to, spiski, dist, current_dist + 1);
        }
        tek = tek->sled;
    }
}

void dfs_spis(int start, int n, struct UzelSpiska** spiski, int* dist) {
    for (int i = 0; i < n; i++) dist[i] = -1;
    dfs_helper_spis(start, spiski, dist, 0);
}

void vivod_rasst(int start, int n, int* dist, const char* title) {
    printf("\nВектор расстояний (%s) от вершины v%d\n", title, start);
    for (int i = 0; i < n; i++) {
        if (dist[i] == -1) {
            printf("  До вершины v%-2d: недостижима (нет пути)\n", i);
        } else {
            printf("  До вершины v%-2d: расстояние = %d\n", i, dist[i]);
        }
    }
}

double vremya() {
    return (double)clock() / CLOCKS_PER_SEC;
}

void zapusk_benchmark() {
    printf("\nЗАМЕР ВРЕМЕНИ РАБОТЫ (Задание 2*)\n\n");

    int razmeri[] = { 100, 300, 600, 1000 };
    int kolichestvo = sizeof(razmeri) / sizeof(razmeri[0]);
    int ver = 25;

    printf("%-8s | %-16s | %-16s | %-16s | %-16s\n", 
           "Вершин", "BFS (Матрица)", "BFS (Списки)", "DFS (Матрица)", "DFS (Списки)");

    for (int t = 0; t < kolichestvo; t++) {
        int n = razmeri[t];
        int** G = sozd_matr(n, ver);
        struct UzelSpiska** spiski = sozd_spiski(n, G);
        int* dist = (int*)malloc(n * sizeof(int));

        double t_bfs_m, t_bfs_l, t_dfs_m, t_dfs_l;
        clock_t start_t;

        start_t = clock(); bfs_matr(0, n, G, dist);            t_bfs_m = (double)(clock() - start_t) / CLOCKS_PER_SEC;
        start_t = clock(); bfs_spis(0, n, spiski, dist);        t_bfs_l = (double)(clock() - start_t) / CLOCKS_PER_SEC;
        start_t = clock(); dfs_matr(0, n, G, dist);             t_dfs_m = (double)(clock() - start_t) / CLOCKS_PER_SEC;
        start_t = clock(); dfs_spis(0, n, spiski, dist);        t_dfs_l = (double)(clock() - start_t) / CLOCKS_PER_SEC;

        printf("%-8d | %14.6f с | %14.6f с | %14.6f с | %14.6f с\n", 
               n, t_bfs_m, t_bfs_l, t_dfs_m, t_dfs_l);

        free(dist);
        ochist_spiski(spiski, n);
        ochist_matr(G, n);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    while (1) {
        printf("1. Случайный граф\n");
        printf("2. Сравнение времени BFS и DFS (Задание 2*)\n");
        printf("0. Выход\n");
        printf("Выбор: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (choice == 0) break;

        if (choice == 1) {
            int n, ver;
            printf("\nВведите количество вершин графа: ");
            if (scanf("%d", &n) != 1 || n <= 0) {
                while (getchar() != '\n');
                printf("Некорректный ввод!\n");
                continue;
            }

            printf("Введите вероятность наличия ребра (0..100): ");
            if (scanf("%d", &ver) != 1 || ver < 0 || ver > 100) {
                while (getchar() != '\n');
                printf("Некорректная вероятность!\n");
                continue;
            }

            int** G = sozd_matr(n, ver);
            struct UzelSpiska** spiski = sozd_spiski(n, G);

            if (n <= 25) {
                vivod_matr(G, n);
                vivod_spiskov(spiski, n);
            } else {
                printf("\nГраф слишком большой (%d вершин), подробный вывод матриц пропущен.\n", n);
            }

            int start;
            do {
                printf("\nВведите начальную вершину (0..%d): ", n - 1);
                if (scanf("%d", &start) != 1) {
                    while (getchar() != '\n');
                    start = -1;
                }
                if (start < 0 || start >= n) {
                    printf("Такой вершины нет\n");
                }
				else if (spiski[start] == NULL) {
					printf("Вершина изолирована! Выберите другую\n");
					start = -1;
				}

            } while (start < 0 || start >= n);
                
            int* dist_bfs_m = (int*)malloc(n * sizeof(int));
            int* dist_bfs_l = (int*)malloc(n * sizeof(int));
            int* dist_dfs_m = (int*)malloc(n * sizeof(int));
            int* dist_dfs_l = (int*)malloc(n * sizeof(int));

            clock_t t1, t2;

            t1 = clock(); bfs_matr(start, n, G, dist_bfs_m);             t2 = clock(); double time_bfs_m = (double)(t2 - t1) / CLOCKS_PER_SEC;
            t1 = clock(); bfs_spis(start, n, spiski, dist_bfs_l);        t2 = clock(); double time_bfs_l = (double)(t2 - t1) / CLOCKS_PER_SEC;
            t1 = clock(); dfs_matr(start, n, G, dist_dfs_m);             t2 = clock(); double time_dfs_m = (double)(t2 - t1) / CLOCKS_PER_SEC;
            t1 = clock(); dfs_spis(start, n, spiski, dist_dfs_l);        t2 = clock(); double time_dfs_l = (double)(t2 - t1) / CLOCKS_PER_SEC;

            if (n <= 30) {
                vivod_rasst(start, n, dist_bfs_m, "BFS, Матрица");
                vivod_rasst(start, n, dist_bfs_l, "BFS, Списки");
                vivod_rasst(start, n, dist_dfs_m, "DFS, Матрица");
                vivod_rasst(start, n, dist_dfs_l, "DFS, Списки");
            }

            printf("\nВремя выполнения от v%d\n", start);
            printf("BFS (Матрица): %.6f с.\n", time_bfs_m);
            printf("BFS (Списки):  %.6f с.\n", time_bfs_l);
            printf("DFS (Матрица): %.6f с.\n", time_dfs_m);
            printf("DFS (Списки):  %.6f с.\n", time_dfs_l);

            free(dist_bfs_m); free(dist_bfs_l);
            free(dist_dfs_m); free(dist_dfs_l);
            ochist_spiski(spiski, n);
            ochist_matr(G, n);
        } 
        else if (choice == 2) {
            zapusk_benchmark();
        }
    }

    printf("\nПрограмма завершена.\n");
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

void fileExists(FILE *duomf)
{
    if  (!duomf)
    {
        printf("Toks duomenu failas neegzistuoja!\n");
        exit(1);
    }
}

void isFileEmpty(FILE *duomf)
{
    fseek(duomf, 0, SEEK_END);
    if (ftell(duomf) == 0)
    {
        printf("Duomenu failas yra tuscias!\n");
        exit(1);
    }
    fseek(duomf, 0, SEEK_SET);
}

int howManyLines(FILE *duomf)
{
    int lines = 0;
    char last = '\n';
    char c;

    while(EOF != (c = fgetc(duomf)))
    {
        if (c == '\n'  &&  last != '\n')
            lines++;
        last = c;
    }
    if (last != '\n')
        lines++;
    rewind(duomf);
    return lines;
}

void getRoute(char *townFrom, char *townTo)
{
    printf("Iveskite isvykimo miesta: ");
    scanf("%s", townFrom);
    printf("Iveskite atvykimo miesta: ");
    scanf("%s", townTo);
}

int addTown(char *town, char **towns, int v)
{
    int isEqual = 1;
    for(int i=0; i<v; i++)
    {
        if(strcmp(towns[i], town)==0)
        {
            isEqual = 0;
            break;
        }
    }

    if(isEqual == 1)
    {
        towns[v] = (char *)malloc(strlen(town) * sizeof(char));
        for(int j = 0; j<strlen(town)+1; j++)
            towns[v][j] = town[j];
        v++;
    }
    return v;
}

int getVertices(FILE *duomf, char **towns, int n)
{
    char town1[255], town2[255];
    int v=0;

    for(int i=0; i<n; i++)
    {
        fscanf(duomf, "%s %s %*d", town1, town2);
        v = addTown(town1, towns, v);
        v = addTown(town2, towns, v);
    }
    rewind(duomf);
    return v;
}

int getIndex(char *town, char **towns, int v)
{
    for(int i=0; i<v; i++)
        if(strcmp(towns[i], town)==0)
            return i;
    return -1;
}

void makeGraph(FILE *duomf, int v, int (*graph)[v], char **towns, int n)
{
    for(int i = 0; i<v; i++)
        for(int j = 0; j<v; j++)
            graph[i][j] = 0;

    char town1[255], town2[255];
    int distance;

    for(int i=0; i<n; i++)
    {
        fscanf(duomf, "%s %s %d", town1, town2, &distance);
        int index1 = getIndex(town1, towns, v);
        int index2 = getIndex(town2, towns, v);
        graph[index1][index2] = distance;
        graph[index2][index1] = distance;
    }
}

void printPath(char **towns, int *path, int destination)
{
    if (path[destination]==-1)
    {
        return;
    }
    printPath(towns, path, path[destination]);
    printf("%s ", towns[destination]);
}

int printDistance(int *distance, int *path, char **towns, int source, int destination, int v)
{
    if(distance[destination]!=INT_MAX)
    {
        printf("\nKeliones %s %s ilgis: %d\n", towns[source], towns[destination], distance[destination]);
        printf("Keliones marsrutas: %s ", towns[source]);
        printPath(towns, path, destination);
        printf("\n");
    }
    else
        printf("Keliones marsrutas %s %s neegzistuoja :(\n", towns[source], towns[destination]);
}

int minDistance(int distance[], int isVisited[], int v)
{
    int min = INT_MAX, minIndex;
    for (int i=0; i < v; i++)
        if (isVisited[i] == 0 && distance[i] <= min)
            min = distance[i], minIndex = i;
    return minIndex;
}

void countVerticesWeight(int v, int (*graph)[v], char **towns, char *townFrom, char *townTo)
{
    int distance[v];
    int isVisited[v];
    int path[v];
    int source = getIndex(townFrom, towns, v);
    int destination = getIndex(townTo, towns, v);
    if(source == -1 || destination == -1)
    {
        printf("Tokio miesto duomenu faile nera!\n");
        exit(1);
    }

    for (int i=0; i<v; i++)
    {
        distance[i] = INT_MAX;
        isVisited[i] = 0;
        path[i] = 0;
        path[source] = -1;
    }
    distance[source] = 0;

    for (int i=0; i<v; i++)
    {
        int index = minDistance(distance, isVisited, v);
        isVisited[index] = 1;

        for (int j = 0; j < v; j++)
        {
            if (isVisited[j] == 0 && graph[index][j]!=0 && distance[index] != INT_MAX && distance[index] + graph[index][j] < distance[j])
            {
                path[j] = index;
                distance[j] = distance[index] + graph[index][j];
            }
        }
        if(index == destination)
            break;
    }
    printDistance(distance, path, towns, source, destination, v);
}

int main(int argc, char *argv[])
{
    FILE *duomf = fopen(*(argv+1), "r");
    fileExists(duomf);
    isFileEmpty(duomf);

    int n = howManyLines(duomf);
    char **towns = (char **)malloc((n+n) * sizeof(char *));
    int v = getVertices(duomf, towns, n);
    int graph[v][v];
    char townFrom[255], townTo[255];
    makeGraph(duomf, v, graph, towns, n);
    fclose(duomf);

    getRoute(townFrom, townTo);
    countVerticesWeight(v, graph, towns, townFrom, townTo);
    free(towns);

    return 0;
}

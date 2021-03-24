#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_CITY_SIZE 30

void doesFileExists(FILE *fp)
{
    if  (!fp)
    {
        printf("Data file does not exist!\n");
        exit(1);
    }
}

void isFileEmpty(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0)
    {
        printf("Data file is empty!\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_SET);
}

int howManyLines(FILE *fp)
{
    int lines = 0;
    char last = '\n';
    char c;

    while(EOF != (c = fgetc(fp)))
    {
        if (c == '\n'  &&  last != '\n')
            lines++;

        last = c;
    }

    if (last != '\n')
        lines++;
        
    rewind(fp);

    return lines;
}

void getRoute(char *cityFrom, char *cityTo)
{
    printf("Departure city: ");
    scanf("%s", cityFrom);
    printf("Arrive city: ");
    scanf("%s", cityTo);
}

void addCity(char *city, char **cities, int *vertices)
{
    int isEqual = 1;
    for(int i=0; i<*vertices; i++)
    {
        if(strcmp(cities[i], city)==0)
        {
            isEqual = 0;
            break;
        }
    }

    if(isEqual == 1)
    {
        cities[*vertices] = (char *)malloc(strlen(city) * sizeof(char));
        for(int j = 0; j<strlen(city)+1; j++)
            cities[*vertices][j] = city[j];
        ++*vertices;
    }
}

int getVertices(FILE *fp, char **cities, int n)
{
    char city1[MAX_CITY_SIZE] = {0}, city2[MAX_CITY_SIZE] = {0};
    int vertices=0;

    for(int i=0; i<n; i++)
    {
        fscanf(fp, "%[^\t]\t%[^\t]\t%*d\t\n", city1, city2);

        addCity(city1, cities, &vertices);
        addCity(city2, cities, &vertices);

        memset(city1, 0, strlen(city1));
        memset(city2, 0, strlen(city2));
    }
    rewind(fp);

    return vertices;
}

int getIndex(char *city, char **cities, int vertices)
{
    for(int i=0; i<vertices; i++)
        if(strcmp(cities[i], city)==0)
            return i;
    return -1;
}

void makeGraph(FILE *fp, int vertices, int (*graph)[vertices], char **cities, int n)
{
    char city1[MAX_CITY_SIZE] = {0}, city2[MAX_CITY_SIZE] = {0};
    int distance = 0;

    for(int i=0; i<n; i++)
    {
        fscanf(fp, "%[^\t]\t%[^\t]\t%d\t\n", city1, city2, &distance);

        int index1 = getIndex(city1, cities, vertices);
        int index2 = getIndex(city2, cities, vertices);

        graph[index1][index2] = distance;
        graph[index2][index1] = distance;

        memset(city1, 0, strlen(city1));
        memset(city2, 0, strlen(city2));
    }
}

void printPath(char **cities, int *path, int destination)
{
    if (path[destination]==-1)
        return;

    printPath(cities, path, path[destination]);
    printf("%s ", cities[destination]);
}

int printDistance(int *distance, int *path, char **towns, int source, int destination, int v)
{
    if(distance[destination]!=INT_MAX)
    {
        printf("\nThe distance between %s-%s: %d\n", towns[source], towns[destination], distance[destination]);
        printf("The route: %s ", towns[source]);
        printPath(towns, path, destination);
        printf("\n");
    }
    else
    {
        printf("The route between %s-%s does not exist :(\n", towns[source], towns[destination]);
    }
}

int minDistance(int distance[], int isVisited[], int v)
{
    int min = INT_MAX, minIndex = 0;
    for (int i=0; i < v; i++)
        if (isVisited[i] == 0 && distance[i] <= min)
            min = distance[i], minIndex = i;
    return minIndex;
}

void countVerticesWeight(int vertices, int (*graph)[vertices], char **cities, char *cityFrom, char *cityTo)
{
    int distance[vertices];
    int isVisited[vertices];
    int path[vertices];
    int source = getIndex(cityFrom, cities, vertices);
    int destination = getIndex(cityTo, cities, vertices);
    if(source == -1 || destination == -1)
    {
        printf("There is no such city in the data file!\n");
        exit(1);
    }

    for (int i=0; i<vertices; i++)
    {
        distance[i] = INT_MAX;
        isVisited[i] = 0;
        path[i] = 0;
        path[source] = -1;
    }
    distance[source] = 0;

    for (int i=0; i<vertices; i++)
    {
        int index = minDistance(distance, isVisited, vertices);
        isVisited[index] = 1;

        for (int j = 0; j < vertices; j++)
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
    printDistance(distance, path, cities, source, destination, vertices);
}

int main(int argc, char *argv[])
{
    FILE *fp = fopen(*(argv+1), "r");
    doesFileExists(fp);
    isFileEmpty(fp);

    int n = howManyLines(fp);
    char cities = (char **)calloc(n+n, sizeof(char *));
    int v = getVertices(fp, cities, n);
    int graph[v][v];
    memset(graph, 0, sizeof(graph[0][0]) * v * v);
    char cityFrom[MAX_CITY_SIZE], cityTo[MAX_CITY_SIZE];

    makeGraph(fp, v, graph, cities, n);
    fclose(fp);

    getRoute(cityFrom, cityTo);
    countVerticesWeight(v, graph, cities, cityFrom, cityTo);

    free(cities);

    return 0;
}

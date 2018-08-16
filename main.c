#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

/* input arguments */
int threadsNumber;
char* PGMfileName;
char* filterFileName;
char* resultFileName;

/* data from files*/
int pictureWidth;
int pictureHeight;
int pixelsNumber;
int pixelsForThread;
int filterSize;

/* global variables*/
int** pixelsTab;
double** filterTab;
int** resultTab;

FILE* PGMfile;
FILE* filterFile;
FILE* resultFile;

typedef struct thread
{
    pthread_t thread;
    int startRow;
    int startCol;
    int endRow;
    int endCol;
} thread_t;

thread_t* threads;

bool pixels_file = false;
bool filter_file = false;
bool result_file = false;
bool pixels_tab = false;
bool filter_tab = false;
bool result_tab = false;
bool threads_args = false;

clock_t start, end;
double cpu_time_used;


void cleanBeforeExit()
{
    printf("\n--Program end of work--\n");

    if(pixels_file)
    {
        fclose(PGMfile);
        printf("%s closed\n", PGMfileName);
    }

    if(filter_file)
    {
        fclose(filterFile);
        printf("%s closed\n",filterFileName);
    }

    if(result_file)
    {
        fclose(resultFile);
        printf("%s closed\n",resultFileName);
    }

    if(pixels_tab)
    {
        for(int i = 0; i < pictureHeight; i++)
            free(pixelsTab[i]);
        free(pixelsTab);
        printf("pixels tab removed\n");
    }

    if(filter_tab)
    {
        for(int i = 0; i < filterSize; i++)
            free(filterTab[i]);
        free(filterTab);
        printf("filter tab removed\n");
    }

    if(result_tab)
    {
        for(int i = 0; i < pictureHeight; i++)
            free(resultTab[i]);
        free(resultTab);
        printf("result tab removed\n");
    }

    if(threads_args)
    {
        free(threads);
        printf("threads data removed\n");
    }

}

void checkArguments(int argc, char** argv)
{
    if(argc != 5)
    {
        printf("Error: to few arguments, should be 5:\n");
        printf("[0] main\n");
        printf("[1] threads number\n");
        printf("[2] PGM picture file name\n");
        printf("[3] filter file name\n");
        printf("[4] file name for output\n");
        cleanBeforeExit();
        exit(1);
    }
    threadsNumber = strtol(argv[1], NULL, 10);

    if(threadsNumber == 0)
        threadsNumber = 4;

    PGMfileName = argv[2];
    filterFileName = argv[3];
    resultFileName = argv[4];
}

FILE* checkFileContent()
{
    PGMfile = fopen(PGMfileName, "r");
    if(PGMfile == NULL)
    {
        printf("Error: could not open %s file\n",PGMfileName);
        cleanBeforeExit();
        exit(1);
    }
    pixels_file = true;

    char* line = NULL;
    size_t len = 0;
    getline(&line, &len, PGMfile);

    char* p2 = strtok(line, " \t\n");
    if(strcmp(p2,"P2") != 0)
    {
        printf("Error: %s is not PGM file\n",PGMfileName);
        cleanBeforeExit();
        exit(1);
    }

    getline(&line, &len, PGMfile);
    while(line[0] == '#')
        getline(&line, &len, PGMfile);

    char* value = strtok(line, " \t\n");
    pictureWidth = strtol(value, NULL, 10);

    value = strtok(NULL, " \t\n");
    pictureHeight = strtol(value, NULL, 10);

    pixelsNumber = pictureWidth * pictureHeight;

    getline(&line, &len, PGMfile);
    int maxColor = strtol(line, NULL, 10);
    if(maxColor > 255 || maxColor < 0)
    {
        printf("Error: wrong PGM file content\n");
        cleanBeforeExit();
        exit(1);
    }

    return PGMfile;
}

int** createPixelsTab()
{
    int** tabOfTabs = malloc(sizeof(int*) * pictureHeight);
    if(tabOfTabs == NULL)
    {
        printf("Error: cannot allocate memory.\n");
        cleanBeforeExit();
        exit(1);
    }
    pixels_tab = true;


    for(int i = 0; i < pictureHeight; i++)
    {
        int* tab = malloc(sizeof(int) * pictureWidth);

        for(int j = 0; j < pictureWidth; j++)
        {
            char* line = NULL;
            size_t len = 0;
            int ret = getline(&line, &len, PGMfile);

            if(ret == -1)
            {
                tab[j] = -1;
            } 
            else
            {
                int number = strtol(line, NULL, 10);
                tab[j] = number;
            }
        }
        tabOfTabs[i] = &tab[0];
    }
    return tabOfTabs;
}

void dispalyPixelsTab()
{
    printf("\nPixels tab\n");
    for(int i = 0; i < pictureHeight; i++)
    {
        for(int j = 0; j < pictureWidth; j++)
        {
            printf("%d ",pixelsTab[i][j]);
        }
        printf("\n");
    }
}

double** createFilterTab()
{
    filterFile = fopen(filterFileName, "r");
    if(filterFile == NULL)
    {
        printf("Error: could not open %s file\n",filterFileName);
        cleanBeforeExit();
        exit(1);
    }
    filter_file = true;
    
    char* line = NULL;
    size_t len = 0;
    int i = 0;

    getline(&line, &len, filterFile);
    filterSize = strtol(line, NULL, 10);

    double** tabOfTabs = malloc(sizeof(double*) * filterSize);
    filter_tab = true;
    for(int i = 0; i < filterSize; i++)
    {
        double* tab = malloc(sizeof(double) * filterSize);
        for(int j = 0; j < filterSize; j++)
        {
            char* line = NULL;
            size_t len = 0;
            getline(&line, &len, filterFile);
            double number;
            sscanf(line, "%lf", &number);
            tab[j] = number;
        }
        tabOfTabs[i] = &tab[0];
    }

    return tabOfTabs;
}

void dispalyFilterTab()
{
    printf("\nFilter tab\n");
    for(int i = 0; i < filterSize; i++)
    {
        for(int j = 0; j < filterSize; j++)
        {
            printf("%lf ",filterTab[i][j]);
        }
        printf("\n");
    }
}

int** createResultTab()
{
    int** tabOfTabs = malloc(sizeof(int*) * pictureHeight);
    result_tab = true;

    for(int i = 0; i < pictureHeight; i++)
    {
        int* tab = malloc(sizeof(int) * pictureWidth);

        for(int j = 0; j < pictureWidth; j++)
        {
            tab[j] = -1;
        }
        tabOfTabs[i] = &tab[0];
    }
    return tabOfTabs;
}

void displayResultTab()
{
    printf("\nResult tab\n");
    for(int i = 0; i < pictureHeight; i++)
    {
        for(int j = 0; j < pictureWidth; j++)
        {
            printf("%d ",resultTab[i][j]);
        }
        printf("\n");
    }
}

void findRange(thread_t* arg)
{

    arg -> startCol = arg -> endCol;
    arg -> startRow = arg -> endRow;

    int col = arg -> startCol;
    int row = arg -> startRow;
    bool rangeFound = false;
    int cellsToShift = pixelsForThread;

    while(rangeFound == false)
    {
        if(col + cellsToShift < pictureWidth)
        {
            col += cellsToShift;
            rangeFound = true;
        }
        else if(row + 1 >= pictureHeight)
        {
            col = 0;
            row++;
            rangeFound = true;
        }
        else
        {
            int toSubtract = pictureWidth - col;
            cellsToShift -= toSubtract;
            col = 0;
            row++;
        }
    }

    arg -> endCol = col;
    arg -> endRow = row;
}

int getMaxValue(int a, int b)
{
    if(a > b)
    {
        return a;
    } 
    else 
        return b;
}

int calculateSinglePixel(int x, int y)
{
    double pixelWithFilter = 0;
    for(int f1 = 0; f1 < filterSize; f1++)
    {
        for(int f2 = 0; f2 < filterSize; f2++)
        {
            int dim1 = getMaxValue(0, x - (filterSize/2) + f1);
            int dim2 = getMaxValue(0, y - (filterSize/2) + f2);
            if(dim1 >= pictureHeight)
                dim1 = 0;
            if(dim2 >= pictureWidth)
                dim2 = 0;
            int filterValue = pixelsTab[dim1][dim2] * filterTab[f1][f2];
            pixelWithFilter += filterValue;
        }
    }
    return pixelWithFilter;
}

void* calculatePixels(void* threadRange)
{

    thread_t* range = (thread_t*) threadRange;

    resultTab[range -> startRow][range -> startCol] = 8;

    int x = range -> startRow;
    int y = range -> startCol;

    while(x != range -> endRow || y != range -> endCol)
    {
        resultTab[x][y] = calculateSinglePixel(x,y);

        if(y + 1 < pictureWidth)
        {
            y++;
        }
        else
        {
            y = 0;
            x++;
        }
    }

    return NULL;
}


void runThrads()
{   
    start = clock();
    for(int i = 0; i < threadsNumber; i++)
    {
        int thread_created = pthread_create(&threads[i].thread, NULL, calculatePixels, (void*)&threads[i]);
        if(thread_created != 0)
        {
            printf("Error!\n");
            cleanBeforeExit();
            exit(1);
        }
    }
}

void displayThreadArgs()
{
    printf("\nThreads args \n");
    for(int i = 0; i < threadsNumber; i++)
    {
        printf("\nt%d: start: [%d][%d]  --  ",i,threads[i].startRow, threads[i].startCol);
        printf("end: [%d][%d]\n",threads[i].endRow, threads[i].endCol);
    }
}

void prepareThreadsArgs()
{
    thread_t arg;
    arg.startRow = 0;
    arg.startCol = 0;
    arg.endRow = 0;
    arg.endCol = 0;

    for(int i = 0; i < threadsNumber; i++)
    {
        findRange(&arg);
        threads[i].startRow = arg.startRow;
        threads[i].startCol = arg.startCol;
        threads[i].endRow = arg.endRow;
        threads[i].endCol = arg.endCol;
    }
}

void saveResult()
{
    resultFile = fopen(resultFileName, "w");
    if(resultFile == NULL)
    {
        printf("Error: cannot create file.\n");
        cleanBeforeExit();
        exit(1);
    }
    result_file = true;

    char strNumb[10];

    fprintf(resultFile, "%s\n", "P2");
    fprintf(resultFile, "%d %d\n", pictureWidth, pictureHeight);
    fprintf(resultFile, "%d\n", 255);

    for(int i = 0; i < pictureHeight; i++)
        for(int j = 0; j < pictureWidth; j++)
            fprintf(resultFile, "%d\n", resultTab[i][j]);

}

int main(int argc, char** argv)
{
    checkArguments(argc, argv);
    PGMfile = checkFileContent();
    pixelsForThread = (pixelsNumber / threadsNumber) + 1;

    threads = malloc(threadsNumber * sizeof(thread_t));
    if(threads == NULL)
    {
        printf("Error: cannot allocate memory for threads\n");
        cleanBeforeExit();
        exit(1);
    }
    threads_args = true;

    pixelsTab = createPixelsTab();

    filterTab = createFilterTab();

    resultTab = createResultTab();

    prepareThreadsArgs();
    runThrads();

    for(int i = 0; i < threadsNumber; i++)
    {
        pthread_join(threads[i].thread, NULL);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTime: %f seconds.\n",cpu_time_used);

    saveResult();

    cleanBeforeExit();
    return 0; 
}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

int number;
int tabSize;

void checkArguments(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Error: to few arguments, should be 2:\n");
        printf("[0] main\n");
        printf("[1] number\n");
        exit(1);
    }
    number = strtol(argv[1], NULL, 10);
    printf("Number: %d\n",number);
    if(number == 0)
    {
        number = 5;
    }
    tabSize = number * number;
}

void displayTab(double* tab)
{
    printf("\n\n");
    for(int i = 0; i < tabSize; i++)
    {
        if(i % number == 0) printf("\n");
        printf("%f ",tab[i]);
    }
    printf("\n\n");
}

void insertNumber(double n, double* tab)
{
    int index = rand() % tabSize;
    if(tab[index] == -1)
    {
        tab[index] = n;
    }
    else
    {
        int found = 0;
        while(found == 0)
        {
            index = (index + 1) % tabSize;
            if(tab[index] == -1)
            {
                tab[index] = n;
                found = 1;
            }
        }
    }
    //displayTab(tab);
}

int main(int argc, char** argv)
{
    checkArguments(argc, argv);

    double* filterTab = malloc(sizeof(double) * tabSize);
    if(filterTab == NULL)
    {
        printf("Error: cannot allocate memory\n");
        exit(1);
    }

    int range = 1000000;
    double base = 1000000.0;
    printf("przedzial: %d\n",range);

    for(int i = 0; i < tabSize; i++)
    {
        filterTab[i] = -1;
    }

    //displayTab(filterTab);

    srand(time(NULL));
    for(int i = 0; i < tabSize-1; i++)
    {
        if(range == 0)
        {
            printf("wylosowana liczba: 0.0\n");
            insertNumber(0, filterTab);
        } 
        else
        {
            int n = rand() % range/(number) + 1;

            printf("wylosowana liczba: %d  ",n );

            double fn = (double)n/base;
            printf("%f\n",fn );
            insertNumber(fn, filterTab);
            
            range -= n;
            printf("przedzial: %d\n",range);
        }
    }
    double fn = (double)range/base;
    printf("%f\n",fn );
    insertNumber(fn, filterTab);

    double wyn = 0;
    for(int i = 0; i < tabSize; i++)
    {
        wyn += filterTab[i];
    }
    printf("%f\n",wyn );


    FILE* filter = fopen("filter.txt", "w");
    if(filter == NULL)
    {
        printf("Error: cannot create file.\n");
    }

    char strNumb[10];

    sprintf(strNumb, "%d\n", number);
    fprintf(filter, "%s", strNumb);

    for(int i = 0; i < tabSize; i++)
    {
        sprintf(strNumb, "%f\n", filterTab[i]);
        fprintf(filter, "%s", strNumb);
    }

    fclose(filter);

    //displayTab(filterTab);
    free(filterTab);

}
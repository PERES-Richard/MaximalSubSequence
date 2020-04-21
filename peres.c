#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <limits.h>

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

int *reverseArray(int array[], int size)
{
    int *arr;
    arr = malloc(sizeof(int) * size);

    if (arr == NULL)
    {
        fprintf(stderr, "Rip malloc intermédiare de taille %d :(", size);
        fflush(stderr);
        exit(-1);
    }

#pragma omp parallel for
    for (int i = size; i > 0; i--)
        arr[size - i] = array[i - 1];

    return arr;
}

struct table
{
    int *tab;
    int size;
};

struct table *allocateT(int size)
{
    struct table *tmp = malloc(sizeof(struct table));
    tmp->size = size;
    tmp->tab = malloc(sizeof(int) * size);

    return tmp;
}

void displayTable(struct table *table)
{
    int size = table->size;

    for (int i = 0; i < size; i++)
        printf("%d ", table->tab[i]);
    printf("\n");
}

void montee(struct table *nums, struct table *resultat)
{
    int size = nums->size, power = log2(size) - 1;

#pragma omp parallel for
    for (int k = 0; k < size; k++)
        resultat->tab[size + k] = nums->tab[k];

    for (int l = power; l >= 0; l--)
    {
        int end = pow(2, l + 1) - 1;

#pragma omp parallel for
        for (int j = pow(2, l); j <= end; j++)
            resultat->tab[j] = resultat->tab[2 * j] + resultat->tab[2 * j + 1];
    }
}

void descente(struct table *resultat_montee, struct table *resultat)
{
    resultat->tab[1] = 0;

    int power = log2(resultat_montee->size);

    for (int l = 1; l < power; l++)
    {
        int end = pow(2, l + 1) - 1;

#pragma omp parallel for
        for (int j = pow(2, l); j <= end; j++)
        {
            if (j % 2 == 0)
                resultat->tab[j] = resultat->tab[j / 2];
            else
                resultat->tab[j] = resultat->tab[j / 2] + resultat_montee->tab[j - 1];
        }
    }
}

void fusion(struct table *resultat_montee, struct table *resultat_descente, struct table *resultat)
{
    int log = log2(resultat_montee->size / 2), start = pow(2, log), end = pow(2, log + 1) - 1;

#pragma omp parallel for
    for (int i = start; i <= end; i++)
        resultat->tab[i % start] = resultat_descente->tab[i] + resultat_montee->tab[i];
}

struct table *prefixSum(int nums[], int size, int verbose)
{
    struct table *numsT = allocateT(size);
    numsT->tab = nums;

    struct table *res_montee = allocateT(size * 2);
    montee(numsT, res_montee);
    free(numsT);

    struct table *res_descente = allocateT(size * 2);
    descente(res_montee, res_descente);

    struct table *res_fusion = allocateT(size);
    fusion(res_montee, res_descente, res_fusion);
    free(res_montee);
    free(res_descente);

    if (verbose)
    {
        printf("Resultat PSUM : ");
        displayTable(res_fusion);
    }

    return res_fusion;
}

struct table *suffixSum(int nums[], int size, int verbose)
{
    int *numsR = reverseArray(nums, size);

    struct table *resultat = prefixSum(numsR, size, 0);

    resultat->tab = reverseArray(resultat->tab, size);

    if (verbose)
    {
        printf("Resultat SSUM : ");
        displayTable(resultat);
    }

    return resultat;
}

void montee_max(struct table *nums, struct table *resultat)
{
    int size = nums->size;

#pragma omp parallel for
    for (int k = 0; k < size; k++)
        resultat->tab[size + k] = nums->tab[k];

    int power = log2(size) - 1;

    for (int l = power; l >= 0; l--)
    {
        int end = pow(2, l + 1) - 1;
#pragma omp parallel for
        for (int j = pow(2, l); j <= end; j++)
            resultat->tab[j] = max(resultat->tab[2 * j], resultat->tab[2 * j + 1]);
    }
}

void descente_max(struct table *resultat_montee, struct table *resultat)
{
    resultat->tab[1] = INT_MIN;
    int power = log2(resultat_montee->size);

    for (int l = 1; l < power; l++)
    {
        int end = pow(2, l + 1) - 1;

#pragma omp parallel for
        for (int j = pow(2, l); j <= end; j++)
        {
            if (j % 2 == 0)
                resultat->tab[j] = resultat->tab[j / 2];
            else
                resultat->tab[j] = max(resultat->tab[j / 2], resultat_montee->tab[j - 1]);
        }
    }
}

void fusion_max(struct table *resultat_montee, struct table *resultat_descente, struct table *resultat)
{
    int log = log2(resultat_montee->size / 2);
    int start = pow(2, log);
    int end = pow(2, log + 1) - 1;

#pragma omp parallel for
    for (int i = start; i <= end; i++)
        resultat->tab[i % start] = max(resultat_descente->tab[i], resultat_montee->tab[i]);
}

struct table *prefixMax(int nums[], int size, int verbose)
{

    struct table *numsT = allocateT(size);
    numsT->tab = nums;

    struct table *res_montee = allocateT(size * 2);
    montee_max(numsT, res_montee);
    free(numsT);

    struct table *res_descente = allocateT(size * 2);
    descente_max(res_montee, res_descente);

    struct table *res_fusion = allocateT(size);
    fusion_max(res_montee, res_descente, res_fusion);
    free(res_montee);
    free(res_descente);

    if (verbose)
    {
        printf("Resultat PMAX : ");
        displayTable(res_fusion);
    }

    return res_fusion;
}

struct table *suffixMax(int nums[], int size, int verbose)
{
    int *numsR = reverseArray(nums, size);

    struct table *resultat = prefixMax(numsR, size, 0);

    resultat->tab = reverseArray(resultat->tab, size);

    if (verbose)
    {
        printf("Resultat SMAX : ");
        displayTable(resultat);
    }

    return resultat;
}

void maxSubSequence(int nums[], int size, struct table *PSUM, struct table *SSUM, struct table *PMAX, struct table *SMAX, /**int *resultat,**/ int VERBOSE)
{
    struct table *M = allocateT(size);
    if (M == NULL)
    {
        fprintf(stderr, "Rip M malloc :(");
        fflush(stderr);
        exit(-1);
    }

#pragma omp parallel for
    for (int i = 0; i < size; i++)
        M->tab[i] = PMAX->tab[i] - SSUM->tab[i] + SMAX->tab[i] - PSUM->tab[i] + nums[i];

    if (VERBOSE)
    {
        printf("M : ");
        displayTable(M);
    }

    int maxi = M->tab[0], si, se;

    int id;
#pragma omp parallel for reduction(max \
                                   : maxi)
    for (id = 0; id < size; id++)
        maxi = maxi > M->tab[id] ? maxi : M->tab[id];

    // //#pragma omp parallel sections
    //     {
    // //#pragma omp section
    //         {
    //             si = 0;
    //             while (M->tab[si] != maxi)
    //                 si++;
    //         }

    // //#pragma omp section
    //         {
    //             se = size - 1;
    //             while (M->tab[se] != maxi)
    //                 se--;
    //         }
    //     }

    printf("%d ", maxi);
    si = 0;
    while (M->tab[si] != maxi)
    {
        si++;
    }

    se = si;

    while (M->tab[se] == maxi)
    {
        printf("%d ", nums[se]);
        se++;
    }
    printf("\n");
    
    free(M);

    if (VERBOSE)
    {
        printf("Maxi = %d\n", maxi);
        printf("Start = %d\n", si);
        printf("End = %d\n", se);
    }

    // resultat[0] = maxi;
    // resultat[1] = si;
    // resultat[2] = se;
}

void displayResult(int nums[], int resultat[])
{

    printf("%d ", resultat[0]);
    for (int i = resultat[1]; i <= resultat[2]; i++)
        printf("%d ", nums[i]);
    printf("\n");

    // char *pcBuf;
    // int iLen = 0;

    // pcBuf = malloc((size * 6) + size + 1);

    // sprintf(pcBuf + iLen, "%d ", resultat[0]);
    // for (int i = resultat[1]; i <= resultat[2]; i++)
    //     iLen += sprintf(pcBuf + iLen, "%d ", nums[i]);
    // printf("%s\n", pcBuf);
}

int main(int argc, char *argv[])
{
    // Parsing
    FILE *f = fopen(argv[1], "r");

    int *nums = malloc(sizeof(int) * 9999999);

    if (nums == NULL)
    {
        fprintf(stderr, "Rip 1st malloc :(");
        fflush(stderr);
        exit(-1);
    }

    int size = 0;

    while (fscanf(f, "%d", &nums[size]) > 0)
        size++;

    nums = (int *)realloc(nums, size * sizeof(int));

    fclose(f);

    int VERBOSE = 0;

    if (VERBOSE)
    {
        printf("Input : \t");
        for (int i = 0; i < size; i++)
            printf("%d ", nums[i]);
        printf("\n");
    }

    struct table *PSUM;
    struct table *SSUM;
    struct table *PMAX;
    struct table *SMAX;

#pragma omp parallel sections
    {
#pragma omp section
        {
            // PSUM
            PSUM = prefixSum(nums, size, VERBOSE);
        }
#pragma omp section
        {
            // SSUM
            SSUM = suffixSum(nums, size, VERBOSE);
        }
    }

#pragma omp parallel sections
    {
#pragma omp section
        {
            // SMAX
            SMAX = suffixMax(PSUM->tab, size, VERBOSE);
        }
#pragma omp section
        {
            // PMAX
            PMAX = prefixMax(SSUM->tab, size, VERBOSE);
        }
    }

    // int *resultat = malloc(sizeof(int) * 3);
    // if (resultat == NULL)
    // {
    //     fprintf(stderr, "Rip resultat malloc :(");
    //     fflush(stderr);
    //     exit(-1);
    // }

    // M
    maxSubSequence(nums, size, PSUM, SSUM, PMAX, SMAX, /**resultat,**/ VERBOSE);

    // Display
    // displayResult(nums, resultat);

    free(PSUM);
    free(SSUM);
    free(SMAX);
    free(PMAX);

    return 0;
}
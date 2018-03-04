#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include "readbuf.h"
#include "mergesort.h"

#define BOUNDARY 2048

int compare(char *a, char *b);
int lexcmp(char *a, int alen, char *b, int blen);
int charcmp(char a, char b);
int strbegin(char *str);
int strend(char *str);
void merge(int low, int mid, int high, int field_index, char ***matrix);

/*
void serial_mergesort(int low, int high, int field, char ***matrix)
{
    //printf("low is %d, mid is %d, high is %d\n", low, (low + high)/2, high);
    if ((high - low) <= 1){
        return;
    }
    int mid = (high + low) / 2;
    serial_mergesort(low, mid, field, matrix);
    serial_mergesort(mid, high, field, matrix);

    merge(low, mid, high, field, matrix);
}

*/

void mergesort(int low, int high, int field, char ***matrix)
{
    int i;
    int s_index[high -low];
    int p_index = low;
    s_index[0] = low + 1; 
    i = low + 1;
    printf("matrix0][0] is %s\n", matrix[0][10]);
    while (i < high){
        if(compare(matrix[i][field], matrix[i-1][field]) < 0){
            s_index[p_index - low] = i;
            p_index = i;

        }
        i++;
    }
    s_index[p_index - low] = high;

    while (s_index[0] != high){
        i = low;
        while (i < high && s_index[i - low] < high){
            merge(i, s_index[i-low], s_index[s_index[i - low]-low], field, matrix);
            s_index[i-low] = s_index[s_index[i-low] - low];
            i = s_index[i - low]; 
        }
    }		

}





void merge(int low, int mid, int high, int field_index, char ***matrix)
{
    //printf("**********low is %d, mid is %d, high is %d**********\n", low, mid, high);
    int i, j;
    int counter;
    for (i = low, j = mid, counter = low; (i < mid) && (j < high);){
        if (compare(matrix[i][field_index], matrix[j][field_index]) <= 0){
            smatrix[counter++] = matrix[i++];
            //printf("1******%s at %d comes before %s at %d, new index is %d\n", smatrix[counter -1][field_index], i-1, matrix[j][field_index], j, counter -1);
        }
        else{
            smatrix[counter++] = matrix[j++];
            //printf("2*****%s at %d comes before %s at %d, new index is %d\n", smatrix[counter - 1][field_index], j-1, matrix[i][field_index], i,  counter -1);
        }

    }
    while(i < mid){
        smatrix[counter++] = matrix[i++];
        //printf("3******follows by %s at %d, new index is %d\n********", smatrix[counter - 1][field_index], i-1, counter -1);
    }

    while(j < high){
        smatrix[counter++] = matrix[j++];
        //printf("4*****follows by %s at %d, new index is%d\n*******", smatrix[counter - 1][field_index], j-1, counter -1);
    }
    for(i = low; i < high; i++){
        matrix[i] = smatrix[i];
    }

} 




int compare(char *a, char *b)
{
    int ret;
    double ad, bd;
    bool ac, bc;
    int ab, bb;
    int ae, be;
    int alen, blen;
    bool aempty, bempty;
    char *endptr;
    ab = strbegin(a);
    ae = strend(a);
    bb = strbegin(b);
    be = strend(b);
    alen = ae - ab + 1;
    blen = be - bb + 1;
    aempty = (alen <= 0);
    bempty = (blen <= 0);
    if (aempty && bempty) {
        ret = 0;
    } else if (aempty) {
        ret = -1;
    } else if (bempty) {
        ret = 1;
    } else {
        ac = false;
        bc = false;
        ad = strtod(a + ab, &endptr);
        if (endptr == a + ae + 1)
            ac = true;
        bd = strtod(b + bb, &endptr);
        if (endptr == b + be + 1)
            bc = true;
        if (ac && bc) {
            if (fabs(ad - bd) < 0.0001)
                ret = 0;
            else if (ad < bd)
                ret = -1;
            else
                ret = 1;
        } else if (ac) {
            ret = -1;
        } else if (bc) {
            ret = 1;
        } else {
            ret = lexcmp(a + ab, alen, b+ bb, blen);
        }
    }
    return ret;
}

int lexcmp(char *a, int alen, char *b, int blen)
{
    int ret;
    int i, j;
    int cmp;
    i = 0;
    j = 0;
    ret = 2;
    while (ret == 2 && i < alen && j < blen) {
        while (a[i] < '\0' || isspace(a[i]))
            i++;
        while (b[j] < '\0' || isspace(b[j]))
            j++;
        if (i == alen && j == blen)
            ret = 0;
        else if (i == alen)
            ret = -1;
        else if (j == blen)
            ret = 1;
        else if ((cmp = charcmp(a[i], b[j])))
            ret = cmp;
        i++;
        j++;
    }
    if (ret == 2) {
        if (i >= alen && j >= blen)
            ret = 0;
        else if (i >= alen)
            ret = -1;
        else
            ret = 1;
    }
    return ret;
}

int charcmp(char a, char b)
{
    int ret;
    if (isalpha(a) && isalpha(b)) {
        if (toupper(a) == toupper(b)) {
            if (isupper(a) && isupper(b))
                ret = 0;
            else if (isupper(a))
                ret = -1;
            else if (isupper(b))
                ret = 1;
            else
                ret = 0;
        } else if (toupper(a) < toupper(b)) {
            ret = -1;
        } else {
            ret = 1;
        }
    } else if (toupper(a) == toupper(b)) {
        ret = 0;
    } else if (toupper(a) < toupper(b)) {
        ret = -1;
    } else {
        ret = 1;
    }
    return ret;
}

int strbegin(char *str)
{
    int i, len, begin;
    if (str)
        len = strlen(str);
    else
        len = 0;
    begin = len;
    i = 0;
    for (i = 0; i < len; i++) {
        if (str[i] > '\0' && !isspace(str[i])) {
            begin = i;
            break;
        }
    }
    return begin;
}

int strend(char *str)
{
    int i, len, end;
    if (str)
        len = strlen(str);
    else
        len = 0;
    end = -1;
    for (i = len - 1; i >= 0; i--) {
        if (str[i] > '\0' && !isspace(str[i])) {
            end = i;
            break;
        }
    }
    return end;
}

void insertionsort(char ***matrix, int field_index, int low, int high)
{
    int i, j;
    char **tmp;
    for (i = low + 1; i < high; i++) {
        tmp = matrix[i];
        for (j = i; j > 0 && compare(matrix[j - 1][field_index], tmp[field_index]) > 0; j--)
            matrix[j] = matrix[j - 1];
        matrix[j] = tmp;
    }
}

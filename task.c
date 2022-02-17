/*
 * Arbitrary fork task
 */
#include "task.h"

/* https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c */
char* form_rand_string(char* str, unsigned int size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    int charset_size = 25;
    int key = 0;
    if (size > 0) 
    {
        --size;
        for (unsigned int n = 0; n < size; n++) 
        {
            key = rand() % charset_size;
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

long hash()
{
    long hash = 0;
    unsigned int str_size = 10;
    char rand_str[str_size]; 
    form_rand_string(rand_str, str_size);
    const int a = rand() * 10;
    const int m = rand() * 15;
    for (unsigned int i = 0; i < str_size; ++i)
    {
        hash += (long)pow(a, str_size - (i+1)) * rand_str[i];
        hash = hash % m;
    }
    return hash;
}

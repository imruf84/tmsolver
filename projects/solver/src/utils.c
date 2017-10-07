#include "utils.h"

uint32_t get_file_rows_count(char *file_name)
{
    uint32_t lines = 0;
    char ch;
    FILE* fp = fopen(file_name, "r");
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }
    fclose(fp);

    return lines;
}

long nCr(long n, long r)
{
    if (n < r) return 0;

    // minimal computation since: NCR(n, r) = NCR(n, n-r)
    if (r > n/2)
        return nCr(n, n-r);

    long out = 1;

    // guaranteed divisible
    for(long k = 1; k <= r; ++k)
    {
        out *= n-k+1;
        out /= k;
    }

    return out;
}

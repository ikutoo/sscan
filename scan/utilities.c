#include "utilities.h"
#include <sys/types.h>
unsigned short my_cksum(unsigned short *data, int len)
{
    int result = 0;
    int i;
    size_t count;

    count = len;
    for(i=0; i<len/2; i++)
    {
        result += *data;
        data++;
        count-=2;
    }

    if (count>0)
        result+=*(unsigned char*)data;

    while(result >> 16)
        result = (result&0xffff) + (result>>16);

    return ~result;
}


void log_append(char* str)
{
    strcat(g_buf, str);
}
void log_clear()
{
    memset(g_buf, 0, BUF_SIZE);
}

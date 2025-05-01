#include "minishell.h"
int message(int i)
{
    if(i==0)
    {
        printf("%s","Çift tırnak kapanmadı.");
        return 0;
    }
    else if (i==1)
    {
        printf("%s","Tek tırnak kapanmadı.");
        return 0;
    }
    
}
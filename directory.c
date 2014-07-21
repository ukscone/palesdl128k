#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
 
#include "directory.h"

void get_directory(char *pathext,char *listing,unsigned int highlight,char *name)
{
    DIR *dirp;
    struct dirent *direntp;
   
    char lbl[200],lbl2[200];
    char list_dir[256];
    int noof_entries_horiz = 0;
    unsigned int noof_entries = 0;
 
    sprintf(list_dir,"./%s",pathext);

    sprintf(listing,"Directory:%s\n",list_dir);

    dirp = opendir( list_dir );
    if( dirp != NULL )
    {
        noof_entries_horiz = 0;
        for(;;)
        {
            direntp = readdir( dirp );
            if(direntp == NULL)
                break;
            if(noof_entries == highlight)
            {
                sprintf(lbl,"\a1\f7");
                sprintf(lbl2,"%s",direntp->d_name);
                strcat(lbl,lbl2);
                sprintf(lbl2,"\a0\f2   ");
                strcat(lbl,lbl2);
                strcpy(name,direntp->d_name);
            }
            else
                sprintf(lbl,"%s   ",direntp->d_name);
            
            noof_entries_horiz++;
            if(noof_entries_horiz > 1)
            {
                strcat(listing,"\n");    
                noof_entries_horiz = 0;
                //noof_entries_vert++;
                //if(noof_entries_vert > 15)break;
            }
            noof_entries ++;
            strcat(listing,lbl);    
        }
        closedir(dirp);
     }
    sprintf(lbl,"\n\n\a1\f4Filename: ");
    strcat(lbl,name);
    strcat(listing,lbl);    
    strcat(listing,"\0");
    sprintf(list_dir,"%s/%s",pathext,name);
    strcpy(name,list_dir);
}

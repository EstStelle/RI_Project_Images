#include <stdio.h>
#include <stdlib.h>

#include "cgiu.h"
#include "proc.h"

#define  DEBUG 1

void exitfail()
{
  print_html_tail(); 
  exit(0);
}

int main(int argc, char *argv[])
{
  int x,m;
  char *qs;
  entry *entries;
    
  int n;
  char **list,**urls;
  
  /* récupération de la chaîne de paramètres */
  qs = get_query_string(argc,argv);

  /* récupération des couples (nom,valeur) */
  entries = get_entries(qs,&m);

  /* émission de l'entête */
  print_html_head("R&eacute;sultat");

  /* affichage éventuel des informations de debug */
  if (DEBUG) print_debug_query(entries,m,qs,argc,argv);

  /* lecture de la liste des images */
  list = readList("/media/simon/Share_Dir/RICM4/RI/TP_image/RI_Project_Images/list.txt",&n);
  if (list == NULL) {printf("Failed to read %s file.\n","list.txt"); exitfail();}
  if (DEBUG) printf("Read list, %d images.<BR>\n",n);
  
  /* lecture des urls des images */
  urls = readList("/media/simon/Share_Dir/RICM4/RI/TP_image/RI_Project_Images/urls.txt",&n);
  if (list == NULL) {printf("Failed to read %s file.\n","urls.txt"); exitfail();}
  if (DEBUG) printf("Read list, %d images.<BR>\n",n);
  
  /* émission de la fin de corps et de document */
  print_html_tail();
    
  exit(0);
}

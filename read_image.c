#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "rdjpeg.h"

int main(int argc, char *argv[])
{
  //create index, file which contain histogram for one picture
	
  int i,j,n,nx,ny,nb;
  CIMAGE cim;
 
  /*------------------------------------------------*/
  /* lecture d'une image requête                    */
  /*------------------------------------------------*/
  read_cimage(argv[1],&cim);
  /*------------------------------------------------*/
  /* affichage des valeurs pour le premier bloc 8x8 */
  /* comme exemple de traitement                    */
  /*------------------------------------------------*/
  printf("Largeur de l'image : %d\n",cim.nx);
  printf("Heuteur de l'image : %d\n",cim.ny);
  printf("Plan rouge du premier bloc 8x8 :\n");
  for (j = 0; j < 8; j++) {       /* ligne par ligne */
    printf("  ");
    for (i = 0; i < 8; i++) {   /* pixel par pixel */
      printf("%4d",cim.r[i][j]);
    }
    printf("\n");
  }
  printf("Plan vert du premier bloc 8x8 :\n");
  for (j = 0; j < 8; j++) {       /* ligne par ligne */
    printf("  ");
    for (i = 0; i < 8; i++) {   /* pixel par pixel */
      printf("%4d",cim.g[i][j]);
    }
    printf("\n");
  }
  printf("Plan bleu du premier bloc 8x8 :\n");
  for (j = 0; j < 8; j++) {       /* ligne par ligne */
    printf("  ");
    for (i = 0; i < 8; i++) {   /* pixel par pixel */
      printf("%4d",cim.b[i][j]);
    }
    printf("\n");
  }
  
  
  
  /* Calcul de l'histogramme */
   int histR, histV, histB, nbPixTot;
  histR = 0;
  histV = 0;
  histB = 0;
  nbPixTot = cim.nx * cim.ny;
  for (j = 0; j < cim.nx; j++) {       /* ligne par ligne */
    printf("  ");
    for (i = 0; i < cim.ny; i++) {   /* pixel par pixel */
      histR = histR + cim.r[i][j];
      histV = histV + cim.v[i][j];
      histB = histB + cim.b[i][j];
    }
  }
  
  int R = histR/64;
  int V = histV/64;
  int B = histB/64;
  
  k = R + V*4 + B*16;
  /* FIN calcul histogramme */

  
  
  /*------------------------------------------------*/

  exit(0);
}













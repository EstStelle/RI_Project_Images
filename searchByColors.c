#include "rdjpeg.h"
#include "proc.h"


#define WIDTH 64
#define TOTAL_FICHIER 9637

KEY tableauTri[TOTAL_FICHIER];

void fullHistogramme (float histo[], CIMAGE cim);
void initHisto (float histo[], int taille);
void getCim (char *url, CIMAGE *cim);
void buildHistogrammes (int paramBoucle);
void buildAndStore (CIMAGE cim, FILE *outFile);
void normalisation (float histo[], int nbPixel, int taille);
void allDistanceEucli (char *requete);
int getBackHist(FILE *file,float buffer[]);




int main (int argc, char *argv[]){
	int nbImages;
	char **imagesName;
	int nbVal;
	int noUse;
	char *requete;

	while (argc != 3){
		printf("argc = %d\n\n", argc);
		printf("Construire histogrammes : ./searchByColors -h n\n");
		printf("Param (n : nombre d'image pour construire l'histogramme | ALL pour tous)\n\n");
		printf("Recherche euclidienne : ./searchByColors -e [FILE]\n");
		printf("Param ([FILE] : fichier sur lequel faire la requête)\n\n");
		printf("Press 1 pour afficher la liste des fichiers possibles AUTRE sinon\n");
		scanf("%d", &noUse);
		if (noUse == 1){
			imagesName = readList("entree/list.txt",&nbImages); //récupère la liste du nom des images
			if(imagesName == NULL) {printf("Impossible d'ouvrir le fichier list.txt");}
			printf ("L'argument doit être une image du fichier list.txt. Choisir parmi :");
			for (int i = 0; i < nbImages; i++)
			{
				printf("%s\n", imagesName[i]);
			}
			//freeList("entree/list.txt", nbImages);
		}
		else{exit(0);}
	}

	if (strcmp(argv[1], "-h") == 0){
	
		if (strcmp(argv[2], "ALL") == 0){
			nbVal = TOTAL_FICHIER;
		}
		else{
			nbVal = atoi(argv[2]);
		}
		printf("Construction de l'histogramme sur %d valeurs.\nAppuyer sur une touche pour continuer ...", nbVal);
		scanf("%d", &noUse);
		buildHistogrammes(nbVal); //ICI IL FAUT PASSER EN PARAM LE NOMBRE DE VALEURS DEMANDÉ
	}
	else if (strcmp(argv[1], "-e") == 0){
		requete = argv[2];
		printf("Comparaison euclidienne de l'image %s avec l'histogramme\n", requete);
		allDistanceEucli (requete);
	}
	else{
		printf("La commande n'est pas bonne\n");
		exit(0);
	}
}





/*construit tous les histogrammes*/
void buildHistogrammes (int paramBoucle){
	char **imagesName;
	char **imagesUrl;
	int nbImages;
	CIMAGE cim;
	FILE *outFile;
	char *imgName, *imgUrl;


	imagesName = readList("entree/list.txt",&nbImages); //récupère la liste du nom des images
	if(imagesName == NULL) {printf("Failed to read %s file.\n", "list.txt");}

	imagesUrl = readList("entree/urls.txt",&nbImages); //récupère la liste d'urls des images
	if(imagesUrl == NULL) {printf("Failed to read %s file.\n", "list.txt");}

	
	outFile = fopen("img/histogrammes", "wb"); //ici on écrit en mode binaire !!!
	if(outFile == NULL){
		printf("Impossible de créer le fichier de sortie img/histogrammes\n");
	}


	for (int i = 0; i < paramBoucle; i++)
	{
		
		getCim(imagesUrl[i], &cim); //on récupère cim de l'image courante
		printf("Traitement Image %d/%d | Taille : %d * %d\n", i, TOTAL_FICHIER, cim.nx, cim.ny);
		imgName = imagesName[i];
		imgUrl = imagesUrl[i];
		buildAndStore(cim, outFile);
		free_cimage(imagesUrl[i], &cim);	
	}



	fclose(outFile);
	//freeList("entree/list.txt", nbImages); //ces deux lignes génère des seg fault
	//freeList("entree/urls.txt", nbImages);
}

/*pour construire un seul histogramme*/
/*param String : nameImg
        String : urlImg
        CIMAGE : cim*/
void buildAndStore (CIMAGE cim, FILE *outFile){
	int nbPixel = cim.nx * cim.ny;
	float histo[WIDTH];
	initHisto(histo, WIDTH);



	fullHistogramme (histo, cim); //remplit l'histogramme
	normalisation(histo, nbPixel, WIDTH); //normalise les vecteurs [0..1]

	printf("Stockage\n");
	fwrite(histo, sizeof(float), WIDTH, outFile); //écrit dans le fichier de sortie en binaire
}


/*aller chercher l'image et et retourne cim*/
/*param entrée : string : url
               : CIMAGE : cim*/
void getCim (char *url, CIMAGE *cim){
	read_cimage(url, cim);
}


/*initialise un histogramme*/
/*param histogramme à initialilser
        taille du tableau (nb de couleurs)*/
void initHisto (float histo[], int taille){
	for (int i=0; i<taille; i++){
		histo[i] = 0;
	}
}

/*Calcul d'un histogramme */
/* param tableau : histogramme
         CIMAGE*/
void  fullHistogramme (float histo[], CIMAGE cim){
	int R, V, B, k;
	R = 0;
	V = 0;
	B = 0;
	k = 0;

	for (int j = 0; j < cim.nx; j++) {       /* ligne par ligne */
		for (int i = 0; i < cim.ny; i++) {   /* pixel par pixel */
			//printf("on veut acceder au pixel %d\n", i);
			R = cim.r[j][i] / 64;
			V = cim.g[j][i] / 64;
			B = cim.b[j][i] / 64;
			k = R + V*4 + B*16;
			histo[k]++;
			//printf("on traite la ligne %d et on est au pixel %d\n", j, i);
		}
		//printf("on passe à la ligne %d\n", j);
	}
}

/*normalisation d'un histograme*/
/*param float [] : histo
        int      : nb de pixels
        int      : taille*/
void normalisation (float histo[], int nbPixel, int taille){
	while(taille > 0){
		histo[taille] = histo[taille] / nbPixel;
		taille--;
	}
}

/*Calcule de la distance euclidienne entre 2 images (histogrammes)*/
float distanceEucli (float req1[], float req2[]){
	float res = 0;
	for (int i = 0; i < WIDTH; i++)
	{
		res += (req1[i]-req2[i]) * (req1[i]-req2[i]);
	}
	return res;
}

/*Calcul de la distance euclidiennne entre la requête et l'ensemble des documents*/
void allDistanceEucli (char *requete){
	CIMAGE req_cim;
	float req_usr[WIDTH], req_current[WIDTH];
	initHisto (req_usr, WIDTH);
	initHisto (req_current, WIDTH);
	char **imagesName;
	int nbImages;

	//récupération de l'image requête
	char urlImgReq[100] = "http://mrim.imag.fr/voc10/images/";
	strcat (urlImgReq, requete);
	printf("Récupération image requête : %s. Et construction histogramme\n", urlImgReq);
	read_cimage(urlImgReq, &req_cim);
	fullHistogramme(req_usr, req_cim);
	normalisation(req_usr, (float)(req_cim.nx*req_cim.ny), WIDTH);

	int i = 0;
	float res;
	FILE *inFile;
	inFile = fopen("img/histogrammes", "rb"); //on ouvre le fichier hist créer en mode read binary
	while(!feof(inFile)){
		printf("Récupération image courante : %d\n", i);
		getBackHist(inFile, req_current);
		res = distanceEucli(req_usr, req_current);
		tableauTri[i].d = res;

		tableauTri[i].k = i;
		i++;
	}
	printf("On tri le tableau\n");
	qsort(tableauTri, TOTAL_FICHIER, sizeof(KEY), keyCompare);

	//TODO afficher infos
	imagesName = readList("entree/list.txt",&nbImages); //récupère la liste du nom des images
	if(imagesName == NULL) {printf("Failed to read %s file.\n", "list.txt");}

	for (int i = 0; i < 10; i++)
	{
		printf("rang : %d | fichier : %s | distance : %f\n", i, imagesName[tableauTri[i].k], tableauTri[i].d);
	}
	fclose(inFile);
	free_cimage(requete, &req_cim);
	//freeList("entree/list.txt", nbImages);
}

/*récupérer un histogramme du fichier img/histogrammes*/
int getBackHist(FILE *file,float buffer[])
{
	int res =fread(buffer,sizeof(float),WIDTH,file);

	return res;
}




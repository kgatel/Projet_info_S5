#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/select.h>
#include <termios.h>

#define Morte 0
#define Vivante 1
#define red "\x1B[31m"
#define cyan   "\x1B[36m"
#define yellow   "\x1B[33m"
#define blue   "\x1B[34m"
#define magenta   "\x1B[35m"
#define white   "\x1B[37m"
#define green  "\x1B[32m"
#define reset "\x1B[0m"

#define color white    			//définir la couleur des cellules vivantes
#define vitesse 150				//définir le temps d'attente (ms) entre les tours

//Initialisons la taille du carré que l'on veut afficher
#define N 45					//22 pour les fichiers de 0 à 3 et 45 pour >=4

//indice allant de 0 à n+1 qui fait un tableau 2D contenant (n+2)² cases
typedef int Grille[N+2][N+2];

/*struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()							
{
    struct termios new_termios;

    // take two copies - one for now, one for later
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    // register cleanup handler, and set the new terminal mode 
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}	
*/

void affichage(Grille cellule)//affichage de la grille sans les lignes/colonnes 0 et N+1
{
	int i,j;
	for (i=1; i<N+1; i++)
	{
		for (j=1; j<N+1; j++)
			if (cellule[i][j]==Morte)
				printf(red "x " reset);
			else
				printf(color "o " reset);
		printf("\n");
	}
}

void initfGrille(Grille cellule)	//grille initialisée dans le fichier fgrille.txt
{	
	FILE* fichier = NULL;
	char fgrille[10];
	printf("Quel fichier voulez-vous charger ?\n");
	scanf("%s",fgrille);
    	fichier = fopen(fgrille, "r");
    	int tab[N*N];		//tableau contenant les N² valeurs
    	int i,j,caractere=0;
    	//initialisation de la grille
    	for (i=0; i<N+2; i++)
    		for (j=0; j<N+2; j++)
    			cellule[i][j]=0;
    	i=0;
    	if (fichier != NULL)
    	{
    		while (caractere != EOF)
    		{
    			caractere = fgetc(fichier); // On lit le caractère
    			if (caractere=='x')
    			{
    				tab[i]=Morte;
    				i++;
    			}
    			if (caractere=='o')
    			{
    				tab[i]=Vivante;
    				i++;
    			}
           	 	//printf("%c", caractere); // On l'affiche
        	}
    		fclose(fichier);
    		for (i=0; i<N*N; i++)
    			cellule[i/N+1][i%N+1]=tab[i]; //affectations des N² valeurs dans notre tableau 2D (qui contient (N+2)² cases)
    	}
   	else
  	{
    		printf("Impossible d'ouvrir le fichier fgrille\n");
        }
}

void initGrille(Grille cellule)
{
	int i,j;
	for (i=0; i<N+2; i++)
		for (j=1; j<N+2; j++)
			cellule[i][j]=Morte;
        for (i=7;i<17;i++)
                cellule[11][i]=Vivante;
}

int seraVivant(Grille cellule,int x,int y);	//permet de déclarer la fonction avant l'appel d'Evolution qui l'ultilise

void Evolution(Grille cellule)
{
	Grille cellule_temp={Morte};
	int i,j;
        for (i=1; i<N+1; i++) //on ne s'occupe pas de la ligne 0 et N+1
                for (j=1; j<N+1; j++) //on ne s'occupe pas de la colonne 0 et N+1
		{
			cellule_temp[i][j]=seraVivant(cellule,i,j);
			//if ((i==1)|(i==N)|(j==1)|(j==N))
			//	cellule_temp[i][j]=Morte;	//les bords les cellules meurent
		}
	//cellule=cellule_temp

	for (i=0; i<N+2; i++)
                for (j=0; j<N+2; j++)
			cellule[i][j]=cellule_temp[i][j];
}

int seraVivant(Grille cellule,int x,int y)
{
	int result,i,j,compteur=0; //compteur compte le nombre de cellules voisines vivantes
	for (i=x-1;i<=x+1;i++)
		for (j=y-1;j<=y+1;j++)
			if ( (i!=x) | (j!=y) )  //8 voisins, il faut exclure le cas où on se compte soit même
				if (cellule[i][j]==1)
					compteur++;

	//une cellule vivante possédant deux ou trois voisines vivantes le reste, sinon elle meurt
	if (cellule[x][y]==Vivante)
		if ( (compteur!=2) & (compteur!=3) )
			result=Morte;
		else
			result=Vivante;
	//une cellule morte possédant exactement trois voisines vivantes devient vivante (elle naît)
	if (cellule[x][y]==Morte)
                if (compteur==3) 
                        result=Vivante;
		else
			result=Morte;

	return result;
}

int etatLCextreme(Grille cellule)
{
	int i,j,resultat=Morte;
	for (i=1; i<N+1; i++)
	{
		if ( (cellule[0][i]==Vivante)|(cellule[N][i]==Vivante)|(cellule[i][1]==Vivante)|(cellule[i][N]==Vivante) )
			resultat=Vivante;
	}
	return resultat;
}

int main(void)		//void=vide
{
	int i=0,tour=-1,arret=0,c;
	int tour_temp;
	Grille cellule;
	initfGrille(cellule);
	printf("\nTour n°0\n");
	affichage(cellule);
	printf("Combien voulez-vous voir de générations ?\nNbre :");
	scanf("%u",&tour);
	for (i=1; i<tour; i++)
	{
		if (etatLCextreme(cellule)==Morte)
		{
			printf("Tour n°%d\n",i);
			Evolution(cellule);
			affichage(cellule);
			tour_temp=i;
			if (i!=tour)
				usleep(vitesse*1000);	//attendre vitesse seconde pour chaque génération
		}
		else
			arret=1;
	}

	if (etatLCextreme(cellule)==Vivante)
		printf("L'animation s'est arrêtée car un bord a été touché\n");
	tour=tour_temp;	//la vrai valeur de tour car l'animation peut s'arrétée
	if (tour<0)
		tour=0;
	tour++;
	while ( (c = getchar()) == '\n')
	{
		printf("Tour n°%d\n",tour);
		Evolution(cellule);
		affichage(cellule);
		tour++;
		if (arret==1)
		{	
			arret=0;
			printf("L'animation s'est arrêtée car un bord a été touché.\n");
			sleep(3);
		}
		printf("Appuyez sur <Entrée> pour voir la suite : ");
	}	
	return 0;
}

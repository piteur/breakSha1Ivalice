#include <stdlib.h>
#include <stdio.h>
int transformeinv(char carac);

int main(int argc, char *argv[])
{
    FILE* fichier = NULL;
    char code[40];
    printf("Code à lancer?\n");
    scanf("%s",code);
    int i,j,k;
    fichier = fopen("code.txt", "w+");
    for(i=0;i<40;i++)
    {
    fprintf(fichier, "%c", code[i]);
    }
    fclose(fichier);
    int choix1;
    int choix2;
    int choix3;
    int choix4;char carac1; int c1;   
    char mot[6]; mot[5]='\0';
    fflush(stdin); 
    printf("Rechercher parmis les chiffres ?(0=non 1=oui)\n");
    scanf("%d",&choix1);
    fflush(stdin); 
    printf("Rechercher parmis les minuscules ?(0=non 1=oui)\n");
    scanf("%d",&choix2);
    fflush(stdin); 
    printf("Rechercher parmis les MAJUSCULES ?(0=non 1=oui)\n");
    scanf("%d",&choix3);
    fflush(stdin); 
    printf("Idee de premier caractere ?(0=non 1=oui)\n");
    scanf("%d",&choix4);
    if(choix4==1){fflush(stdin); 
                  printf("Donnez le premier caractere\n");
                  scanf("%c",&carac1);
                  c1=transformeinv(carac1);
                  }
    for(i=0;i<3844;i++)
    {
    sprintf (mot, "%d", i);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "0");
    fclose(fichier);  
    }    
////////////////ENTIER
if(choix1==0)
{     
    for(i=0;i<620;i++)
    {
    sprintf (mot, "%d", i);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "2");
    fclose(fichier);  
    }

    for(j=10;j<62;j++)
    {    
    for(i=0;i<10;i++)
    {
    k=i+62*j;
    sprintf (mot, "%d", k);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "2");
    fclose(fichier);                       
    }      
    } 
}   
///////////MINUSCULES
if(choix2==0)
{
    for(i=620;i<2232;i++)
    {
    sprintf (mot, "%d", i);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "2");
    fclose(fichier);  
    }
    
    for(j=0;j<62;j++)
    {    
    for(i=10;i<36;i++)
    {
    k=i+62*j;
    sprintf (mot, "%d", k);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "2");
    fclose(fichier);                       
    }      
    }
}         
//////////////MAJUSCULES
if(choix3==0)
{ 
    for(i=2232;i<3844;i++)
    {
    sprintf (mot, "%d", i);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "2");
    fclose(fichier);  
    }
    
    for(j=0;j<62;j++)
    {    
    for(i=36;i<62;i++)
    {
    k=i+62*j;
    sprintf (mot, "%d", k);
    fichier = fopen(mot, "w+");
    fprintf(fichier, "2");
    fclose(fichier);                       
    }      
    }     
}                                 
////////////////PREMIER CARACTERE
    for(j=0;j<62;j++)
    {    
    for(i=0;i<62;i++)
    {
    k=i+62*j;
    if(j!=c1)
        {
        sprintf (mot, "%d", k);
        fichier = fopen(mot, "w+");
        fprintf(fichier, "2");
        fclose(fichier);
        }                       
    }      
    }     
}

int transformeinv(char carac)
{
    switch(carac)
    {
    case '0' : return(0);break;
    case '1' : return(1);break;
    case '2' : return(2);break;
    case '3' : return(3);break;
    case '4' : return(4);break;
    case '5' : return(5);break;
    case '6' : return(6);break;
    case '7' : return(7);break;
    case '8' : return(8);break;
    case '9' : return(9);break;
    case 'a' : return(10);break;
    case 'b' : return(11);break;
    case 'c' : return(12);break;
    case 'd' : return(13);break;
    case 'e' : return(14);break;
    case 'f' : return(15);break;
    case 'g' : return(16);break;
    case 'h' : return(17);break;
    case 'i' : return(18);break;
    case 'j' : return(19);break;
    case 'k' : return(20);break;
    case 'l' : return(21);break;
    case 'm' : return(22);break;
    case 'n' : return(23);break;
    case 'o' : return(24);break;
    case 'p' : return(25);break;
    case 'q' : return(26);break;
    case 'r' : return(27);break;
    case 's' : return(28);break;
    case 't' : return(29);break;
    case 'u' : return(30);break;
    case 'v' : return(31);break;
    case 'w' : return(32);break;
    case 'x' : return(33);break;
    case 'y' : return(34);break;
    case 'z' : return(35);break;
    case 'A' : return(36);break;
    case 'B' : return(37);break;
    case 'C' : return(38);break;
    case 'D' : return(39);break;
    case 'E' : return(40);break;
    case 'F' : return(41);break;
    case 'G' : return(42);break;
    case 'H' : return(43);break;
    case 'I' : return(44);break;
    case 'J' : return(45);break;
    case 'K' : return(46);break;
    case 'L' : return(47);break;
    case 'M' : return(48);break;
    case 'N' : return(49);break;
    case 'O' : return(50);break;
    case 'P' : return(51);break;
    case 'Q' : return(52);break;
    case 'R' : return(53);break;
    case 'S' : return(54);break;
    case 'T' : return(55);break;
    case 'U' : return(56);break;
    case 'V' : return(57);break;
    case 'W' : return(58);break;
    case 'X' : return(59);break;
    case 'Y' : return(60);break;
    case 'Z' : return(61);break;
    }
}

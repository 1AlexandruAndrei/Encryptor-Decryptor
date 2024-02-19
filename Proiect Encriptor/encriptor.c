#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

///pentru criptare
char* permuteWord(char *word, int *permutare, int lungime) 
{
    char *cuvantNou=(char *)malloc((lungime+1)*sizeof(char));

    for (int i=0;i<lungime;i++) 
    	cuvantNou[i]=word[permutare[i]-1];///folosesc permutare ca sa combin literele
    	///-1 pentru ca pp ca permutarea incepe de la 1

    cuvantNou[lungime]='\0';
    return cuvantNou;
}

///pentru decriptare
char* permuteWordInverse(char *word, int *permutare, int lungime) 
{
    char *cuvantNou=(char *)malloc((lungime+1)*sizeof(char));

    for (int i=0;i<lungime;i++) 
        cuvantNou[permutare[i]-1]=word[i];///in ordine.txt incep de la 1, de asta am pus aici -1
    
    cuvantNou[lungime]='\0';
    return cuvantNou;
}

void faraGarbage(char *str) 
{
    int lungime=strlen(str);

    while (lungime>0 && (str[lungime-1]=='\n' || str[lungime-1]=='\r' || str[lungime-1]==' ' || str[lungime-1] == '\t')) ///da remove la space taburi, carriage , etc
  	  str[lungime-1] = '\0';
    
}

int main(int argc, char *argv[]) 
{
    if (argc == 2) /// CRIPTARE
    {
        char *inputFileName = argv[1];
        FILE *inputFile = fopen(inputFileName, "r");

        if (inputFile == NULL) 
        {
            perror("ERROR WHILE OPENING INPUT FILE\n");
            return 1;
        }

        char *outputFileName = "encrypted.txt";
        FILE *outputFile = fopen(outputFileName, "w");

        if (outputFile == NULL) 
        {
            perror("ERROR WHILE OPENING OUTPUT FILE\n");
            fclose(inputFile);
            return 1;
        }

        srand(time(NULL)); /// ca sa am numere random de fiecare data

        char line[1024];
        int permutare[100];
        int lungimePermutare;

        while (fgets(line, sizeof(line), inputFile) != NULL) 
        {
            faraGarbage(line);
            lungimePermutare=strlen(line); ///cate litere are cuvantu

            for (int i=0;i<lungimePermutare;i++) 
                permutare[i]=i+1;
            

	    ///permutare random cu fisher yates
            for (int i=lungimePermutare-1;i>0;i--) 
            {
                int j=rand()%(i+1);
                int temp=permutare[i];
                permutare[i]=permutare[j];
                permutare[j]=temp;
            }

            char *cuvantcriptat=permuteWord(line, permutare, lungimePermutare);

            write(fileno(outputFile),cuvantcriptat,strlen(cuvantcriptat));
            write(fileno(outputFile),"\n",1);
            free(cuvantcriptat);
        }

        fclose(inputFile);
        fclose(outputFile);

        printf("ENCRYPTION SUCCESSFUL!\nCHECK %s TO VIEW THE ENCRYPTED FILE.\n", outputFileName);
    } 
    
    
    /// DECRIPTOR
    else if (argc==3) 
    {
        

        char *inputFileName=argv[1];
        char *orderFileName=argv[2];

        FILE *inputFile=fopen(inputFileName, "r");
        FILE *orderFile=fopen(orderFileName, "r");

        if (inputFile==NULL) 
            perror("ERROR WHILE OPENING INPUT FILE\n");

        if (orderFile==NULL) 
            perror("ERROR WHILE OPENING ORDER FILE\n");

        char *outputFileName="decrypted.txt";
        FILE *outputFile=fopen(outputFileName, "w");

        if (outputFile==NULL) 
        {
            perror("ERROR WHILE OPENING OUTPUT FILE\n");
            fclose(inputFile);
            fclose(orderFile);
            return 1;
        }

        char line[1024];
        int permutare[100];
        int lungimePermutare;

        while (fgets(line, sizeof(line), orderFile) != NULL) 
        {
            lungimePermutare=0;
            char *token = strtok(line, ",");

            while (token != NULL && lungimePermutare<100) 
            {
                sscanf(token, "%d", &permutare[lungimePermutare]);
                lungimePermutare++;
                token=strtok(NULL, ",");
            }

            if (fgets(line, sizeof(line), inputFile) == NULL) 
            {
                perror("ERROR WHILE READING LINE FROM INPUT FILE\n");
                break;
            }

            faraGarbage(line);

            int lungime=strlen(line);
            if (lungime==lungimePermutare) 
            {
                char *cuvantdecriptat = permuteWordInverse(line, permutare, lungimePermutare);

                write(fileno(outputFile), cuvantdecriptat, strlen(cuvantdecriptat));
                write(fileno(outputFile), "\n", 1);

                free(cuvantdecriptat);
            } 
            else 
            {
                fprintf(stderr, "EROARE LA LUNGIMEA CUVANTULUI\n");
            }
        }

        fclose(inputFile);
        fclose(orderFile);
        fclose(outputFile);

        printf("DECRYPTION SUCCESSFUL!\nCHECK %s TO VIEW THE DECRYPTED FILE.\n", outputFileName);
    } 
    else 
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "For encryption: %s input_file\n", argv[0]);
        fprintf(stderr, "For decryption: %s input_file order_file\n", argv[0]);
        return 1;
    }

    return 0;
}

/// ./encrip inputcrip.txt ordine.txt
/// ./encrip input.txt


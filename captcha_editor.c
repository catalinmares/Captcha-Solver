#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_header.h"

#define MAX1 30
#define MAX2 100

struct Pixel
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;

};

struct Cifre
{
	int cifra;
	int linie;
	int coloana;
	int de_extras;
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

void parse_input (char* linia1, int* blue, int* green, int* red,
				  int* to_extract, int* cifre_de_extras)
{
	FILE *fisier; // pointer catre input.txt

	ssize_t citire; // variabila pentru citirea din input.txt
	char* linie = NULL; // linia citita cu getline
	size_t lungime = 0; // lungimea liniei 
	int nr_linie = 0; // variabila care contorizeaza numarul liniei

	char linia2[MAX1], linia3[MAX1], linia4[MAX1]; // vector pt liniile 2, 3, 4

	fisier = fopen ("input.txt", "r"); // deschid input.txt

	if (fisier == NULL)
	{
		exit(1);
	}
	
	// citesc din input.txt cat timp am linii
	while ((citire = getline (&linie, &lungime, fisier)) != -1)
	{
		nr_linie++;
		switch (nr_linie)
		{
			case 1:
			{
				strcpy (linia1, linie); // copiez prima linie in linia1
				break;
			}

			case 2:
			{
				strcpy (linia2, linie); // copiez a doua linie in linia2
				break;
			}

			case 3:
			{
				strcpy (linia3, linie); // copiez a treia linie in linia3
				break;
			}

			case 4:
			{
				strcpy (linia4, linie); // copiez a patra linie in linia4
			}
		}
	}

	fclose (fisier); // inchid fisierul input.txt

	char* integer; // pointer de parsare cu strtok
	int culoare = 0; // contor pentru culoare
	
	integer = strtok (linia2, " ");

	while (integer != NULL)
	{
		culoare++; // incrementez culoarea
		
		switch (culoare)
		{
			case 1:
			{
				*blue = atoi (integer); // salvez culoarea BLUE
				break;
			}
			case 2:
			{
				*green = atoi (integer); // salvez culoarea GREEN
				break; 
			}
			case 3:
			{
				*red = atoi (integer); // salvez culoarea RED
				break;
			}
		}
		
		integer = strtok (NULL, " "); // scot urmatoarea culoare
	}

	if (linia3[0] == '\n') // verific daca am cifre pe linia 3
	{
		*to_extract = 0; // daca nu am, setez pe 0 contorul cifrelor de extras
	}
	else
	{
		integer = strtok (linia3, " "); // scot prima cifra
		
		while (integer != NULL) // cat timp am gasit o cifra
		{
			cifre_de_extras[*to_extract] = atoi (integer); // salvez cifra

			(*to_extract)++; // incrementez numarul cifrelor de extras
			integer = strtok (NULL, " "); // scot urmatoarea cifra
		}
	}

	linia1[strlen (linia1) - 1] = '\0'; // scot \n din numele imaginii
}

void read_image (char* linia1, int* padding, struct Pixel pixel_mat[100][100],
				 BITMAPFILEHEADER* fileheader, BITMAPINFOHEADER* infoheader)
{
	FILE *bmp_ptr; // pointer pentru imaginea de input

	int i, j;
	int pixel_size = sizeof (struct Pixel); // dimensiunea pixelilor

	bmp_ptr = fopen (linia1, "rb"); // deschid imaginea

	if(bmp_ptr == NULL) 
	{
		exit(1); // daca nu s-a deschis ies din program
	}

	// citesc headerele
	fread (fileheader, sizeof (BITMAPFILEHEADER), 1, bmp_ptr);
	fread (infoheader, sizeof (BITMAPINFOHEADER), 1, bmp_ptr);
	
	*padding = (4 - (3 * infoheader->width) % 4) % 4; // calculez paddingul

	for (i = 0; i < infoheader->height; i++)
	{
		for (j = 0; j < infoheader->width; j++)
		{	
			// citesc imaginea pixel cu pixel
			fread (&pixel_mat[i][j], pixel_size, 1, bmp_ptr);
		}
		
		// la finalul fiecarei linii ignor paddingul
		fseek (bmp_ptr, *padding, SEEK_CUR);
	}

	fclose (bmp_ptr); // inchid fisierul imaginii
}

void write_image (char* linia1_task, int padding, BITMAPFILEHEADER fileheader,
				  BITMAPINFOHEADER infoheader, struct Pixel pixel_mat[100][100])
{
	FILE *bmp_out; // pointer pentru fisierul de output al taskului 1

	int i, j;

	unsigned char bmppad[3] = {0, 0, 0}; // vector pentru padding
	int pixel_size = sizeof (struct Pixel); // dimensiunea structurii

	bmp_out = fopen (linia1_task, "wb"); // deschid noua imagine

	// scriu headerele
	fwrite (&fileheader, sizeof (BITMAPFILEHEADER), 1, bmp_out);
	fwrite (&infoheader, sizeof (BITMAPINFOHEADER), 1, bmp_out);

	for (i = 0; i < infoheader.height; i++)
	{
		for (j = 0; j < infoheader.width; j++)
		{
			// scriu imaginea pixel cu pixel
			fwrite (&pixel_mat[i][j], pixel_size, 1, bmp_out);
		}
		
		// scriu paddingul octet cu octet
		fwrite (bmppad, sizeof (unsigned char), padding, bmp_out);
	}
	
	fclose(bmp_out); // inchid fisierul de output
}

void write_numbers (char* linia1_task2, int t, struct Cifre *cfr)
{
	FILE *bmp_txt; // pointer catre fisierul text

	int i;

	bmp_txt = fopen (linia1_task2, "w"); // deschid fisierul text

	for (i = 0; i < t; i++)
	{
		fprintf(bmp_txt, "%d", cfr[i].cifra); // scriu cifra in fisier
	}

	fclose(bmp_txt); // inchid fisierul text
}

void find_numbers(BITMAPINFOHEADER infoheader, struct Pixel mat_r[100][100],
				  int pix_col[100][100], int *nr_cifre_gasite, 
				  struct Cifre *cfr)
{
	int i, j, contor, contor2;

	int nrpix_stanga; // nr de pixeli pe coloana din stanga a cifrei
	int nrpix_jos; // nr de pixeli pe linia de jos a cifrei
	int nrpix_dreapta; // nr de pixeli pe coloana din dreapta a cifrei
	int nrpix_sus; // nr de pixeli pe linia de sus a cifrei
	int nrpix_centru; // nr de pixeli pe linia centrala 
	int nrpix_col; // nr de pixeli colorati pe urmatoarea coloana (pentru 1)

	for (i = 0; i < infoheader.height; i++)
	{
		for (j = 0; j < infoheader.width; j++)
		{
			if (mat_r[i][j].blue != 255 || mat_r[i][j].green != 255 ||
				mat_r[i][j].red != 255) 
			{
				pix_col[i][j] = 1; // daca pixelul e colorat il salvez cu 1
			}
			else
			{
				pix_col[i][j] = 0; // altfel cu 0
			}
		}
	}

	for (j = 0; j < infoheader.width; j++)
	{
		for (i = 0; i < infoheader.height; i++)
		{
			if (mat_r[i][j].blue == 255 && mat_r[i][j].green == 255 && 
				mat_r[i][j].red == 255)
			{
				continue; // daca pixelul e alb trec mai departe
			}
			if (pix_col[i][j] == 2)
			{
				// daca pixelul e de la o cifra deja intalnita trec mai departe
				continue;
			}
			else
			{
				nrpix_stanga = 1; // 1 pentru ca am gasit deja un pixel colorat
				nrpix_jos = 0;
				nrpix_dreapta = 0;
				nrpix_sus = 0;
				nrpix_centru = 0;
				nrpix_col = 0;
				for (contor = 1; contor <= 4; contor++)
				{
					// verific pixelii de pe coloana din stanga
					if (mat_r[i + contor][j].blue != 255 || 
						mat_r[i + contor][j].green != 255 || 
						mat_r[i + contor][j].red != 255)
					{
						if (pix_col[i + contor][j] == 2)
						{
							// daca pixelul de pe coloana din stanga a fost
							// deja gasit trec mai departe
							continue;
						}
						else
						{
							// altfel il contorizez
							nrpix_stanga++;
						}
					}
				}
				for (contor = 1; contor <= 4; contor++)
				{
					// verific pixelii de pe coloana urmatoare celei din stanga
					if (mat_r[i + contor][j + 1].blue != 255 || 
						mat_r[i + contor][j + 1].green != 255 || 
						mat_r[i + contor][j + 1].red != 255)
					{
						if (pix_col[i + contor][j + 1] == 2)
						{
							// daca pixelul de pe coloana urmatoare a fost
							// deja gasit trec mai departe
							continue;
						}
						else
						{
							// altfel il contorizez
							nrpix_col++;
						}
					}
				}

				if (nrpix_stanga == 5 && nrpix_col == 0) // daca am gasit 1
				{
					// salvez cifra, coordonatele si culorile intr-o structura
					cfr[*nr_cifre_gasite].cifra = 1;
					cfr[*nr_cifre_gasite].coloana = j - 4;
					cfr[*nr_cifre_gasite].linie = i;
					cfr[*nr_cifre_gasite].blue = mat_r[i][j].blue;
					cfr[*nr_cifre_gasite].green = mat_r[i][j].green;
					cfr[*nr_cifre_gasite].red = mat_r[i][j].red;
					(*nr_cifre_gasite)++;
					
					for (contor = 0; contor < 5; contor++)
					{
						for (contor2 = 0; contor2 < 5; contor2++)
						{
							// setez toti octetii cifre 1 pe 2
							pix_col[i + contor][j - contor2] = 2;
						}
					}
				}
				
				else
				{
					for (contor = 1; contor <= 4; contor++)
					{
						// verific pixelii de pe linia de jos
						if (mat_r[i + 4][j + contor].blue != 255 || 
							mat_r[i + 4][j + contor].green != 255 || 
							mat_r[i + 4][j + contor].red != 255)
						{
							if (pix_col[i + 4][j + contor] == 2)
							{
								// daca pixelul de pe linia de jos a fost
								// deja gasit trec mai departe
								continue; 
							}
							else
							{
								// altfel il contorizez
								nrpix_jos++;
							}
						}
					}
					
					for (contor = 0; contor <= 3; contor++)
					{
						// verific pixelii de pe coloana din dreapta
						if (mat_r[i + contor][j + 4].blue != 255 || 
							mat_r[i + contor][j + 4].green != 255 || 
							mat_r[i + contor][j + 4].red != 255)
						{
							if (pix_col[i + contor][j + 4] == 2)
							{
								// daca pixelul de pe coloana din dreapta a fost
								// deja gasit trec mai departe
								continue;
							}
							else
							{
								// altfel il contorizez
								nrpix_dreapta++;
							}
						}
					}
					
					for (contor = 1; contor <= 3; contor++)
					{
						// verific pixelii de pe linia de sus
						if (mat_r[i][j + contor].blue != 255 || 
							mat_r[i][j + contor].green != 255 || 
							mat_r[i][j + contor].red != 255)
						{
							if (pix_col[i][j + contor] == 2)
							{
								// daca pixelul de pe linia de sus a fost
								// deja gasit trec mai departe
								continue;
							}
							else
							{
								// altfel il contorizez
								nrpix_sus++;
							}
						}
					}
					
					for (contor = 1; contor <= 3; contor++)
					{
						// verific pixelii de pe linia de centru
						if (mat_r[i + 2][j + contor].blue != 255 || 
							mat_r[i + 2][j + contor].green != 255 || 
							mat_r[i + 2][j + contor].red != 255)
						{
							if (pix_col[i + 2][j + contor] == 2)
							{
								// daca pixelul de pe coloana din stanga a fost
								// deja gasit trec mai departe
								continue;
							}
							else
							{
								// altfel il contorizez
								nrpix_centru++;
							}
						}
					}

					if (nrpix_stanga == 5 && nrpix_jos == 4 && 
						nrpix_dreapta == 4 && nrpix_sus == 3 && 
						nrpix_centru == 0)
					{
						// daca am gasit 0 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 0;
					}

					if (nrpix_stanga == 3 && nrpix_jos == 4 && 
						nrpix_dreapta == 4 && nrpix_sus == 3 && 
						nrpix_centru == 3)
					{
						// daca am gasit 3 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 3;
					}

					if (nrpix_stanga == 3 && nrpix_jos == 1 && 
						nrpix_dreapta == 4 && nrpix_sus == 0 && 
						nrpix_centru == 3)
					{
						// daca am gasit 4 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 4;
					}

					if (nrpix_stanga == 5 && nrpix_jos == 4 && 
						nrpix_dreapta == 3 && nrpix_sus == 3 && 
						nrpix_centru == 3)
					{
						// daca am gasit 6 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 6;
					}

					if (nrpix_stanga == 1 && nrpix_jos == 1 && 
						nrpix_dreapta == 4 && nrpix_sus == 3 && 
						nrpix_centru == 0)
					{
						// daca am gasit 7 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 7;
					}

					if (nrpix_stanga == 5 && nrpix_jos == 4 && 
						nrpix_dreapta == 4 && nrpix_sus == 3 && 
						nrpix_centru == 3)
					{
						// daca am gasit 8 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 8;
					}

					if (nrpix_stanga == 4 && nrpix_jos == 4 && 
						nrpix_dreapta == 4 && nrpix_sus == 3 && 
						nrpix_centru == 3)
					{
						// daca am gasit 9 o salvez in structura
						cfr[*nr_cifre_gasite].cifra = 9;
					}

					if (nrpix_stanga == 4 && nrpix_jos == 4 && 
						nrpix_dreapta == 3 && nrpix_sus == 3 && 
						nrpix_centru == 3)
					{
						if (mat_r[i + 1][j + 4].blue != 255 || 
							mat_r[i + 1][j + 4].green != 255 || 
							mat_r[i + 1][j + 4].red != 255)
						{
							// daca am gasit 2 o salvez in structura
							cfr[*nr_cifre_gasite].cifra = 2;
						}
						
						else
						{
							// daca am gasit 2 o salvez in structura
							cfr[*nr_cifre_gasite].cifra = 5;
						}
					}

					// salvez coordonatele si culoarea cifrei in structura
					cfr[*nr_cifre_gasite].coloana = j;
					cfr[*nr_cifre_gasite].linie = i;
					cfr[*nr_cifre_gasite].blue = mat_r[i][j].blue;
					cfr[*nr_cifre_gasite].green = mat_r[i][j].green;
					cfr[*nr_cifre_gasite].red = mat_r[i][j].red;
					(*nr_cifre_gasite)++;

					for (contor = 0; contor < 5; contor++)
					{
						for (contor2 = 0; contor2 < 5; contor2++)
						{
							// setez pe 2 pixelii cifrei
							pix_col[i + contor][j + contor2] = 2;
						}
					}
				}
			}
		}
	}
}

void task3(int to_extract, int nr_cifre_gasite, struct Cifre *cfr, 
		   int *cifre_de_extras, struct Cifre *cfr2, 
		   struct Pixel mat_r[100][100])
{
	int i, j, k;
	int nr_cifre_ramase; // variabila care retine numarul cifrelor ramase
	int cifre_de_scos = 0; // contor pentru numarul de cifre de scos

	for(i = 0; i < to_extract; i++)
	{
		for (j = 0; j < nr_cifre_gasite; j++)
		{
			if (cfr[j].cifra == cifre_de_extras[i])
			{
				// setez pe 1 cifrele care trebuiesc scoase
				cfr[j].de_extras = 1;
				cifre_de_scos++;
			}
		}
	}

	k = 0;
	
	// parcurg vectorul de structuri in care am salvat cifrele
	for (i = 0; i < nr_cifre_gasite; i++)
	{
		if (cfr[i].de_extras == 1)
		{
			continue; // daca cifra trebuie extrasa trec mai departe
		}
		
		else
		{
			// altfel salvez in alt vector 
			cfr2[k].cifra = cfr[i].cifra; // ce cifra e
			cfr2[k].linie = cfr[i].linie; // linia la care se afla
			cfr2[k].coloana = cfr[i].coloana; // coloana la care se afla
			cfr2[k].blue = cfr[i].blue; // valoarea BLUE
			cfr2[k].green = cfr[i].green; // valoarea GREEN
			cfr2[k].red = cfr[i].red; // valoarea RED
			k++;	
		}
	}
	
	nr_cifre_ramase = nr_cifre_gasite - cifre_de_scos;
	k = 0;

	// salvez coordonatele pentru cifrele care ramas
	for (i = 0; i < nr_cifre_ramase; i++)
	{
		cfr2[k].linie = cfr[i].linie;
		cfr2[k].coloana = cfr[i].coloana;
		k++;
	}

	for (i = 0; i < nr_cifre_ramase; i++)
	{
		// albesc toti pixelii pentru fiecare cifra
		for (j = 0; j < 5; j++)
		{
			for (k = 0; k < 5; k++)
			{
				mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue = 255;
				mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green = 255;
				mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red = 255;
			}
		}

		switch(cfr2[i].cifra)
		{
			case 1: // colorez manual pixelii pentru cifra 1
			{
				for(j = 0; j < 5; j++)
				{	
					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].blue = 
					cfr2[i].blue;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].green = 
					cfr2[i].green;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].red = 
					cfr2[i].red;
				}

				break;
			}

			case 2: // colorez manual pixelii pentru cifra 2
			{
				for (j = 0; j < 5; j += 2)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue = 
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green = 
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red = 
						cfr2[i].red;
					}
				}

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].blue = 
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].green = 
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].red = 
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].blue = 
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].green = 
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].red = 
				cfr2[i].red;

				break;
			}

			case 3: // colorez manual pixelii pentru cifra 3
			{
				for (j = 0; j < 5; j += 2)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].blue = 
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].blue = 
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].red =
				cfr2[i].red;

				break;
			}

			case 4: // colorez manual pixelii pentru cifra 4
			{
				for (j = 0; j < 3; j++)
				{
					mat_r[cfr2[i].linie + j][cfr2[i].coloana].blue =
					cfr2[i].blue;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana].green =
					cfr2[i].green;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana].red =
					cfr2[i].red;
				}

				for (j = 0; j < 5; j++)
				{
					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].blue =
					cfr2[i].blue;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].green =
					cfr2[i].green;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].red =
					cfr2[i].red;
				}

				for (k = 1; k < 4; k++)
				{
					mat_r[cfr2[i].linie + 2][cfr2[i].coloana + k].blue =
					cfr2[i].blue;

					mat_r[cfr2[i].linie + 2][cfr2[i].coloana + k].green =
					cfr2[i].green;

					mat_r[cfr2[i].linie + 2][cfr2[i].coloana + k].red =
					cfr2[i].red;
				}

				break;
			}

			case 5: // colorez manual pixelii pentru cifra 5
			{
				for (j = 0; j < 5; j += 2)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].red =
				cfr2[i].red;

				break;
			}

			case 6: // colorez manual pixelii pentru cifra 6
			{
				for (j = 0; j < 5; j += 2)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].red =
				cfr2[i].red;

				break;
			}

			case 7: // colorez manual pixelii pentru cifra 7
			{
				for (k = 0; k < 5; k++)
				{
					mat_r[cfr2[i].linie][cfr2[i].coloana + k].blue =
					cfr2[i].blue;

					mat_r[cfr2[i].linie][cfr2[i].coloana + k].green =
					cfr2[i].green;

					mat_r[cfr2[i].linie][cfr2[i].coloana + k].red =
					cfr2[i].red;
				}

				for (j = 1; j < 5; j++)
				{
					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].blue =
					cfr2[i].blue;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].green =
					cfr2[i].green;

					mat_r[cfr2[i].linie + j][cfr2[i].coloana + 4].red =
					cfr2[i].red;
				}

				break;
			}

			case 8: // colorez manual pixelii pentru cifra 8
			{
				for (j = 0; j < 5; j += 2)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].red =
				cfr2[i].red;

				break;
			}

			case 9: // colorez manual pixelii pentru cifra 9
			{
				for (j = 0; j < 5; j += 2)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 3][cfr2[i].coloana + 4].red =
				cfr2[i].red;


				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].blue =
				cfr2[i].blue;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].green =
				cfr2[i].green;

				mat_r[cfr2[i].linie + 1][cfr2[i].coloana + 4].red =
				cfr2[i].red;

				break;
			}

			case 0: // colorez manual pixelii pentru cifra 0
			{
				for (j = 0; j < 5; j += 4)
				{
					for (k = 0; k < 5; k++)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}

				for (j = 1; j < 4; j++)
				{
					for (k = 0; k < 5; k += 4)
					{
						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].blue =
						cfr2[i].blue;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].green =
						cfr2[i].green;

						mat_r[cfr2[i].linie + j][cfr2[i].coloana + k].red =
						cfr2[i].red;
					}
				}
				break;
			}
		}
	}

	// setez pe alb pixelii pentru cifrele care raman din primul vector
	// in care sunt salvate toate cifrele gasite la task 2
	for (i = nr_cifre_ramase; i < nr_cifre_gasite; i++)
	{
		for (j = 0; j < 5; j++)
		{
			for (k = 0; k < 5; k++)
			{
				mat_r[cfr[i].linie + j][cfr[i].coloana + k].blue = 255;
				mat_r[cfr[i].linie + j][cfr[i].coloana + k].green = 255;
				mat_r[cfr[i].linie + j][cfr[i].coloana + k].red = 255;
			}
		}
	}
}

int main()
{
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;

	char linia1[MAX1]; // linia 1 din input.txt
	
	int cifre_de_extras[MAX1]; // vectorul cifrelor de pe linia 3 din input.txt
	int blue, green, red; // valorile BGR din input.txt
	int to_extract = 0; // numarul cifrelor de extras din input.txt
	
	int i, j, k, ok;
	int nr_cifre_gasite = 0; // numarul cifrelor din image
	int padding = 0; // paddingul imaginii

	struct Pixel pixel_mat[MAX2][MAX2]; // matricea de pixeli
	struct Pixel mat_r[MAX2][MAX2]; // rasturnata matricei de pixeli
	struct Cifre cfr[MAX1], cfr2[MAX1], aux; // vectori in care salvez cifrele
	int pix_col[MAX2][MAX2]; // vector pentru task 2

	// apelez functia in care extrag informatii din input.txt
	parse_input(linia1, &blue, &green, &red, &to_extract, cifre_de_extras);
	
	//					TASK-UL 1

	// apelez functia in care "citesc" imaginea (headere si matrice de pixeli)
	read_image(linia1, &padding, pixel_mat, &fileheader, &infoheader);
	
	char* bmp_image = strtok(linia1, "."); // numele imaginii fara extensie
	// adaugam extensia pentru task 1
	char* linia1_task1 = strcat(bmp_image, "_task1.bmp");

	k = 0;

	// rastorn matricea de pixeli
	for (i = infoheader.height - 1; i >= 0; i--)
	{
		for (j = 0; j < infoheader.width; j++)
		{
			mat_r[k][j] = pixel_mat[i][j];
		}
		k++;
	}

	// schimbam culoarea pixelilor colorati
	for (i = 0; i < infoheader.height; i++)
	{
		for (j = 0; j < infoheader.width; j++)
		{
			if (pixel_mat[i][j].blue != 255 || pixel_mat[i][j].green != 255 ||
				pixel_mat[i][j].red != 255)
			{
				pixel_mat[i][j].blue = blue;
				pixel_mat[i][j].green = green;
				pixel_mat[i][j].red = red;
			}
		}
	}

	// apelez functia care scrie imaginea modificata intr-un fisier nou
	write_image(linia1_task1, padding, fileheader, infoheader, pixel_mat);
	
	//             			TASK-UL 2

	bmp_image = strtok(bmp_image, "_");
	// adaug extensia pentru task 2
	char* linia1_task2 = strcat(bmp_image, "_task2.txt");

	// apelez functia care cauta cifre in matricea de pixeli
	find_numbers(infoheader, mat_r, pix_col, &nr_cifre_gasite, cfr);

	// sortez crescator vectorul cu cifre dupa coloana la care incep
	do
	{
		ok = 1;
		for (i = 0; i < nr_cifre_gasite - 1; i++)
		{
			if (cfr[i].coloana > cfr[i + 1].coloana)
			{
				ok = 0;
				aux = cfr[i];
				cfr[i] = cfr[i + 1];
				cfr[i + 1] = aux;
			}

		}
	} while (!ok);

	// apelez functia care scrie cifrele in fisier text
	write_numbers(linia1_task2, nr_cifre_gasite, cfr);

	//						TASK-UL 3

	bmp_image = strtok(bmp_image, "_");
	// adaug extensia pentru task 3
	char* linia1_task3 = strcat(bmp_image, "_task3.bmp");

	// apelex functia care extrage cifrele 
	task3(to_extract, nr_cifre_gasite, cfr, cifre_de_extras, cfr2, mat_r);

	k = 0;
	
	// rastorn la loc matricea de pixeli
	for (i = infoheader.height - 1; i >= 0; i--)
	{
		for (j = 0; j < infoheader.width; j++)
		{
			pixel_mat[i][j] = mat_r[k][j];
		}
		k++;
	}

	// apelez functia care scrie noua imagine intr-un nou fisier
	write_image(linia1_task3, padding, fileheader, infoheader, pixel_mat);

	return 0;
}

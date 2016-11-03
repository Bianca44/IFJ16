#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"


int length (char *string);
int compare (char *s1, char *s2);
char * substr (char *s, int i, int n);


int main2 () {

	//length
	char str [] = "blablabla";
	int dlzka = length(str);
	printf("String length is: %d\n", dlzka);

	//compare
	char string1 [] = "ahoj";
	char string2 [] = "cau";
	int comparison = compare(string1, string2);
	printf("Vysledok porovnavania: %d\n", comparison);

	//substr
	char source [] = "Cauko volam sa Tamara";
	int position = 6;
	int length_of_substring = 4;
	char *ot = substr(source, position, length_of_substring);
    printf("Substr: %s\n", ot);

	return 0;
}

int length (char *string) {
	return (strlen(string));
}

int compare (char *s1, char *s2) {
	int comparison = strcmp (s1, s2);
	if (comparison < 0) {
		return -1;
	}
	else if (comparison > 0) {
		return 1;
	}
	else if (comparison == 0) {
		return 0;
	} else {
		return -1;
	}
}

char *substr (char *s, int i, int n) {
	if (i<0 || n < 0) {
		return NULL;
	}
	char *new_str = (char *) malloc((n + 1) * sizeof(char));
	if (new_str == NULL) {
		return NULL;
	}
	//osetrenie stavu, kedy pozadujeme zacinajuci index > ako dlzka samotneho retazca
	if (i > (int) strlen(s)) {
		printf("ERROR: index je mimo rozsah daneho retazca!\n");
	}

	//osetrenie kedy pozadovana dlzka hladaneho podretazca je dlhsia ako samotny retazec
	if (n > (int) strlen(s) - i) {
		printf("ERROR: dlzka pozadovaneho retazca je vacsia ako povodny retazec! \n");
	}

	for (int j = 0; j<n; j++, i++) {
		new_str[j] = s[i];
	}
	new_str[n] = '\0';
	//printf("%s\n",new_s);
	return new_str;
}

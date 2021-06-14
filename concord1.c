/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define line_line_len 300        //max number of lines
#define max_char_len 3000    //max number of characters per line
#define word_max_len 60        //max character length of a word
#define text_word_max 500           //max number of text_word
#define text_word_max_ecx 500       //max number of exclusion text_word

char arrange_array[line_line_len][max_char_len];  //input array with dimensions: max number of lines x max number of characters per line
char out_array[text_word_max_ecx][word_max_len];        //exclusion array with dimensions: max number of exclusion text_word x max character length of a word
char text_word[text_word_max][word_max_len];                //word array with dimensions: max number of text_word x max character length of a word

//specific function used to produce a 2d array of text_word from a 2d array containing lines of text
void word_get_list(int num_lines, int *longest_word_num_text_word);
//display  keytext_word and their corresponding lines and line numbers
void display(int num_lines, int num_text_word, int exc_lines, int longest_word);
//get the max_word_for line 
int line_array(int max_char_line, int num_chars, char array[][num_chars], FILE *filename, int line_number);
//sway the string
int strcmp_wrapper(const void *a, const void *b);
//cal the frequency
int get_frequency(char *haystack, char *needle);

int main(int argc, char* argv[]) {
	int num_inp_lines;          //refers to number of lines in input file
	int num_exc_lines = 0;      //refers to number of lines in exclusion file
	FILE *input = NULL;         //pointer to address of input file
	FILE *exclusion = NULL;     //pointer to address of exclusion file
	int indent_num_text_word[2];    //array used to store the length of the indent (index 0)
								//and the number of text_word (including duplicates) in an input text file (index 1
	if (argc == 2) {
		input = fopen(argv[1], "r");

		int c = fgetc(input);   
		if (c == EOF) {
			exit(1);
		}
		else {
			ungetc(c, input);
		}
		num_inp_lines = line_array(line_line_len, max_char_len, arrange_array, input, 0);
		fclose(input);}
	else {
		if (strcmp(argv[1], "-e") == 0) {
			exclusion = fopen(argv[2], "r");
			input = fopen(argv[3], "r");


		}
		else {
			exclusion = fopen(argv[3], "r");
			input = fopen(argv[1], "r");
		}
		num_inp_lines = line_array(line_line_len, max_char_len, arrange_array, input, 0);
		fclose(input);
		num_exc_lines = line_array(text_word_max_ecx, word_max_len, out_array, exclusion, 0);
		fclose(exclusion);
	}
	word_get_list(num_inp_lines, indent_num_text_word);
	qsort(text_word, indent_num_text_word[1], word_max_len * sizeof(char), strcmp_wrapper);
	display(num_inp_lines, indent_num_text_word[1], num_exc_lines, indent_num_text_word[0]);
	return 0;

}

int line_array(int max_char_line, int num_chars, char array[][num_chars], FILE *filename, int line_number) {

	while (fgets(array[line_number], max_char_line, filename)) {
		int len = strlen(array[line_number]);

		if (array[line_number][len - 1] == '\n') {
			array[line_number][len - 1] = '\0';
		}
		line_number++;
	}
	return line_number;
}

void display(int num_lines, int num_text_word, int exc_lines, int longest_word) {

	int word_occurence_per_line; //counts number of occurrences per line of a word
	int flag = 0;                //flag is set if one of the text_word in our line is in the exclusion array
	int indent;                  //indent needed for proper line display

	for (int z = 0; z < num_text_word; z++) {             //check for duplicate text_word and only iterate when the word next to our given index is
		while (strcmp(text_word[z], text_word[z + 1]) == 0) {   //different from our current word
			z++;
		}

		for (int r = 0; r < exc_lines; r++) {             //check if our word is in the exclusion array
			flag = 0;
			if (strcmp(text_word[z], out_array[r]) == 0) {
				flag = -1;
				break;
			}
		}

		if (flag != -1) {                            //if word is in exclusion array do not continue with print process
			for (int h = 0; h < num_lines; h++) {

				word_occurence_per_line = get_frequency(arrange_array[h], text_word[z]);    //count number of word occurrences in a given line

				if (word_occurence_per_line > 0) {                    //if word occurs at least once within line continue with print process

					indent = longest_word - strlen(text_word[z]) + 2;

					for (int f = 0; f < strlen(text_word[z]); f++) {      //turn our word into ALL CAPS
						text_word[z][f] = toupper(text_word[z][f]);
					}

					if (word_occurence_per_line > 1) {
						printf("%s%*s%s (%d*)\n", text_word[z], indent, "", arrange_array[h], h + 1);   //print word and line
					}
					else {
						printf("%s%*s%s (%d)\n", text_word[z], indent, "", arrange_array[h], h + 1);
					}

					for (int f = 0; f < strlen(text_word[z]); f++) {  //turn our word into lower case so it can be used once more in the
						text_word[z][f] = tolower(text_word[z][f]);     //get_frequency function
					}
				}
			}
		}
	}
}

void word_get_list(int num_lines, int *longest_word_num_text_word) {
	char buffer[num_lines][max_char_len];
	char *token;
	int x = 0;

	longest_word_num_text_word[0] = 0;  //refers to length of longest word, ie. the indent length for displaying the lines in display
	longest_word_num_text_word[1] = 0;  //refers to number of text_word (including duplicates) in the input text file

	for (int i = 0; i < num_lines; i++) {
		strncpy(buffer[i], arrange_array[i], max_char_len);

		token = strtok(buffer[i], " \n");

		while (token != NULL) {
			strncpy(text_word[x], token, word_max_len);
			x++;

			if (strlen(token) > longest_word_num_text_word[0]) {     //compares word length to find longest word
				longest_word_num_text_word[0] = strlen(token);
			}

			longest_word_num_text_word[1]++;    //adds to word count

			token = strtok(NULL, " \n");
		}
	}
}

int strcmp_wrapper(const void *a, const void *b) {
	char *sa = (char *)a;
	char *sb = (char *)b;

	return(strcmp(sa, sb));
}


int get_frequency(char *haystack, char *needle)
{
	char buffer[line_line_len];
	char *t;
	int  num_occur = 0;

	strncpy(buffer, haystack, line_line_len);

	if (buffer[strlen(buffer) - 1] == '\n') {
		buffer[strlen(buffer) - 1] = '\0';
	}

	t = strtok(buffer, " ");
	while (t != NULL) {
		if (strcmp(needle, t) == 0) {
			num_occur++;
		}
		t = strtok(NULL, " ");
	}

	return (num_occur);
}

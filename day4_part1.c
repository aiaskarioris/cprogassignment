// Aias Karioris
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE	65536
#define GRID_SIZE	65536

// Functions for checking occurrences in various orientations
unsigned int check_horizontal(char *buffer, unsigned int x_len, unsigned int y_len);
unsigned int check_vertical(char *buffer, unsigned int x_len, unsigned int y_len);
unsigned int check_diagonal(char *buffer, unsigned int x_len, unsigned int y_len);

// Checks if "XMAS" appears in `substring` (both forwards or backwards)
unsigned int check_substring(char *substring);

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("usage: %s [input file]\n\n", argv[0]);
		return 30;
	}

	// Read the input file into a string buffer
	FILE *infile = fopen(argv[1], "r");
	if(infile == 0)
		return 1;

	// Store the file contents into a temporary buffer
	char *temp = malloc(sizeof(char) * FILE_SIZE);
	if(temp == 0) {
		printf("Memory allocation error.\n\n");
		return 10;
	}

	size_t bytes_read = fread(temp, 1, FILE_SIZE, infile); // note how many bytes we read
	fclose(infile);

	// Find the dimensions of the input grid
	unsigned int x_len, y_len;
	// To get the row size count the number of characters from the start of the buffer until the first \n
	for(x_len = 0; temp[x_len] != '\n'; x_len++);
	// NOTE: this only works in CR files, not CRLF

	// To get the number of rows simply divide the number of bytes read with the number of bytes per row
	y_len = (unsigned int)(bytes_read / ((size_t)(x_len+1) * sizeof(char)));
	// `x_len` does not count the \n at the end of each line but `bytes_read` does

	// Move contents of file to an actual buffer, skip newlines
	char buffer[GRID_SIZE];
	for(unsigned int i = 0; i < y_len; i++) {
		memcpy(&buffer[i*x_len], &temp[i*(x_len+1)], y_len*x_len*sizeof(char));
	}
	free(temp);

	// To find all occurrences of "XMAS" we have to check for all orientations (vertical, horizontal and diagonal)
	unsigned int result = 0;
	result += check_vertical(buffer, x_len, y_len);
	result += check_horizontal(buffer, x_len, y_len);
	result += check_diagonal(buffer, x_len, y_len);


	printf("* Result: %u\n\n", result);

	return 0;
}

// Vertical checks are the simplest, as we only need to check for "XMAS" in
// continuous regions of memory.
unsigned int check_horizontal(char *buffer, unsigned int x_len, unsigned int y_len) {
	unsigned int result = 0;

	// Iterate through lines...
	for(unsigned int line = 0; line < y_len; line++) {
		// Iterate through characters in a line
		for(unsigned int substr = 0; substr < x_len-3; substr++) { // skip the last 3 characters
			result += check_substring(&buffer[line*x_len + substr]);
		}
	}

	return result;
}

// In this function we create a small 4-char buffer and fill it with
// parts of columns. This way columns appear as continuous regions of data
// to `check_substring` and that function can be re-used
unsigned int check_vertical(char *buffer, unsigned int x_len, unsigned int y_len) {
	unsigned int result = 0;
	char short_buffer[4];

	// Iterate through columns...
	for(unsigned int col = 0; col < x_len; col++) {
		// Iterate through each column
		for(unsigned int row = 0; row < y_len-3; row++) {
			// Populate the 4-byte buffer
			for(unsigned int i = 0; i < 4; i++) {
				short_buffer[i] = buffer[
					(row+i) * x_len // select vertical position
					+ col // select horizontal offset
				];
			}

			result += check_substring(short_buffer);
		}
	}

	return result;
}

// Similar to `check_vertical`. Substrings are extracted from diagonal lines
unsigned int check_diagonal(char *buffer, unsigned int x_len, unsigned int y_len) {
	unsigned int result = 0;
	char short_buffer[4];

	// Calculate how many diagonal lines the grid has in one direction (i.e. left to right)
	// We get one diagonal line for each letter in the first row and first column
	// (notes: then the longest line is counted twice, don't count the diag. lines that have less than 4 letters)

	// We'll have to check diagonal lines starting from the first row and then diagonals starting from the first col.
	// These checks will have to be done twice, once left-to-right and once right-to-left.

	// Start from 1st row, LTR
	for(unsigned int x = 0; x < x_len-3; x++) {
		unsigned int idx = x;

		// Stop when `idx` reaches the third column from the right
		while(idx % x_len < x_len - 3) { // Check if the substring is big enough

			// Fill buffer
			unsigned int i;
			for(i = 0; i < 4; i++) {
				short_buffer[i] = buffer[idx];
				idx += x_len + 1; // +1 to get the next element in the next row
			}

			// Test buffer
			result += check_substring(short_buffer);

			// Set row index to the next letter of the current diagonal
			idx -= (x_len + 1) * 3;
		}
	}

	// Start from 1st column, LTR
	for(unsigned int y = 1; y < y_len-3; y++) { // we've already tested the diagonal from 0,0
		unsigned int idx = y*x_len;

		// Stop once `idx` reaches the third row from the bottom
		while(idx / x_len < y_len - 3) {

			// Fill buffer
			for(unsigned int i = 0; i < 4; i++) {
				short_buffer[i] = buffer[idx];
				idx += x_len + 1; // +1 to get the next element in the next row
			}

			// Test buffer
			result += check_substring(short_buffer);

			// Set row index to the next letter of the current diagonal
			idx -= (x_len + 1) * 3;
		}
	}


	// Start from 1st row, RTL
	for(unsigned int x = x_len-1; x >= 3; x--) {
		unsigned int idx = x;
		// Traverse the diagonal line starting with idx until only 4 columns remain
		while( idx % x_len >= 3 ) {

			// Fill buffer
			for(unsigned int i = 0; i < 4; i++) {
				short_buffer[i] = buffer[idx];
				idx += x_len - 1; // -1 to get the previous element in the next row
			}

			// Test buffer
			result += check_substring(short_buffer);

			// Set row index to the next letter of the current diagonal
			idx -= (x_len - 1) * 3;
		}
	}

	// Start from last column, RTL
	for(unsigned int y = 1; y < y_len-3; y++) { // we've already tested the diagonal from the upper-right corner
		unsigned int idx = y*(x_len) + x_len - 1;
		while( idx / x_len < x_len-3 ) {

			// Fill buffer
			for(unsigned int i = 0; i < 4; i++) {
				short_buffer[i] = buffer[idx];
				idx += x_len - 1; // +1 to get the next element in the next row
			}

			// Test buffer
			result += check_substring(short_buffer);

			// Set row index to the next letter of the current diagonal
			idx -= (x_len - 1) * 3;
		}
	}

	return result;
}

// Checks if "XMAS" appears in `substring` (both forwards or backwards)
unsigned int check_substring(char *substring) {
	const char magicword[] = {'X', 'M', 'A', 'S'};
	unsigned int test;

	// forward pass
	for(test = 0; test < 4; test++) {
		// Break this loop if chars don't match
		if(magicword[test] != substring[test])
			break;
	}
	// If the for-loop completed we found our word
	if(test == 4)
		return 1;

	// backward pass
	for(test = 0; test < 4; test++) {
		// Just return if chars don't match
		if(magicword[4-test-1] != substring[test])
			return 0;
	}

	return 1;
}



/*
 * Explanation
 * -----------
 *
 * This puzzle is solved by finding all occurrences of the word XMAS in a grid of letters. XMAS may appear
 * in a row, in a column or diagonally. The desired word may appear backwards as well (SAMX).
 *
 * This puzzle is quite complicated so we break down into smaller problems. The input letter grid is parsed
 * sequentially row-wise, column-wise and diagonally. The 4-letter substrings parsed are given as arguments
 * to the `check_substring` function, which returns 1 if the substring is the desired word (XMAS or SAMX).
 *
 * The input grid is supplied to the program by a file. By counting the number of letters before the first newline
 * character the row length (`x_len`) is found. By dividing the number of bytes read from the file with `x_len+1`,
 * the number of rows (`y_len`) is derived. 1 is added to `x_len` to accommodate for the newline character present
 * in the file buffer. After the dimensions of the grid are known, each line is copied into a buffer, skipping
 * the newline character that would otherwise complicate the algorithm slightly.
 *
 * The three directions checked are split into three different functions, one for each direction. To search for
 * occurrences in rows, `check_substring` is given as input pointers within the grid buffer. This is achieved via
 * the `check_horizontal` function. This function scans rows from top to bottom and goes to the next row when
 * there aren't any characters left in the current row to form a 4-letter substring.
 *
 * The function `check_vertical` scans columns for occurrences. Since `check_substring` expects a continuous buffer
 * of 4 letters, this function uses a small 4-letter buffer to extract substrings from columns into continuous memory.
 * Similar to `check_horizontal`, this function will go to the next column when the current one has not enough letters
 * to fill the short buffer.
 *
 * Finally, the `check_diagonal` function is the most complicated one but works similar to the previous ones. A short
 * buffer is used again but this time it is filled by letters in diagonal lines. A 2D grid has some diagonal lines
 * starting from the letters in the first row and some diagonal lines starting from the first column
 * when parsing lines from left to right and an equal number of diagonal lines when parsing from right to left.
 * In both LTR and RTL parsing, the longest diagonal line start both from the first/last row and first/last column.
 * The `check_diagonal` function starts from the first row of the grid and checks all diagonal lines LTR, then starts
 * from the first column and checks LTR and finally repeates these two processes from the last element of the first
 * row and the last column respectively.
 *
 * The three check functions can be executed in any order. They all return the number of occurrences the found. Since
 * the letter grid is just read (and not overwritten), the functions could be executed in parallel. Interestingly,
 * `check_substring` could be accelerated by SIMD operations, thus checking if the 4 input letters and the 4 target
 * letters are the same in a single operation.
 *

*/

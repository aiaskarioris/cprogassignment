// Aias Karioris
#include <stdio.h>
#include <string.h>

#define FILE_SIZE	65536
#define ENTRIES_MAX_COUNT	1024

typedef struct _ENTRY_ST {
	int data[8];
	unsigned int len;
} entry_t;


int check_report(entry_t *entry_t);

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("usage: %s [input file]\n\n", argv[0]);
		return 30;
	}

	// Read the input file into a string buffer
	FILE *infile = fopen(argv[1], "r");
	char buffer[FILE_SIZE];
	if(infile == 0)
		return 1;

	size_t bytes_read = fread(buffer, 1, FILE_SIZE, infile); // note how many bytes we read
	fclose(infile);

	// Buffer to store parsed integers
	entry_t entries[ENTRIES_MAX_COUNT]; // there are more memory-efficient ways of storing entries but this is the easiest
	// Number of reports (number of lines)
	unsigned int report_count = 0;
	// Number of entries per report
	unsigned int report_len;

	// Parse string buffer to an array of integers
	unsigned int buffer_idx = 0;
	unsigned int entry_idx = 0;
	entry_t temp;
	unsigned int got_report_len = 0;

	// We'll use temp's len as the index of the currently parsed number; reset it before starting
	temp.len = 0;
	while(buffer[buffer_idx] != '\0') {
		switch(buffer[buffer_idx]) {
			// Digit
			case '0' ... '9':
				temp.data[temp.len] = temp.data[temp.len] * 10 + (buffer[buffer_idx] - '0');
				break;

			// Space: increase temp's len
			case ' ':
				printf("%d ", temp.data[temp.len]);
				temp.len++;
				temp.data[temp.len] = 0; // reset because this might have the previous entry's contents
				break;

			// Newline: Store current entry
			case '\n':
				// Store temp entry
				temp.len++;
				printf("%d \t| entry %d (len %d)\n", temp.data[temp.len-1], entry_idx, temp.len);
				entries[entry_idx] = temp;
				entry_idx++;
				// Reset temp
				temp.len = 0;
				temp.data[0] = 0;
				// Increase report count if this is the end of a report
				report_count += (buffer[buffer_idx] == '\n') ? 1 : 0;
				// Toggle `got_report_len` on the first newline
				if(!got_report_len && buffer[buffer_idx] == 0x0A ) {
					report_len = entry_idx;
					got_report_len = 1;
				}
				break;


			// Do nothing in other cases (EOF or LF)
			default:
				break;
		}

		buffer_idx++;
	}


	printf("Reports: %d\n", report_count);

	// Count how many reports are safe
	unsigned int result = 0;
	for(unsigned int i = 0; i < report_count; i++){
		printf("\n#%d ", i);
		result += check_report(&entries[i]);
	}

	printf("* Result: %u\n\n", result);

	return 0;
}

int check_report(entry_t *entry){
	int *buffer = entry->data;

	// Check if the report should be all-increasing or all-decreasing
	printf("\n0x%8x", buffer);

	// Expect an all-decreasing report
	if(buffer[0] < buffer[1]) {
		for(unsigned int i = 0; i < entry->len-1; i++){
			// Check the entries are still monotonic
			if(buffer[i] >= buffer[i+1])	{ printf("\tX1 "); return 0; }
			// Check the increase is not too high
			if(buffer[i+1] > buffer[i] + 3)	{ printf("\tX2 (i=%d)", i); return 0; }
		}
	}
	// Expect an all-decreasing report
	else if(buffer[0] > buffer[1]) {
		for(unsigned int i = 0; i < entry->len-1; i++){
			// Check the entries are still monotonic
			if(buffer[i] <= buffer[i+1])	{ printf("\tX3 "); return 0; }
			// Check the decrease is not too high
			if(buffer[i+1] < buffer[i] - 3)	{ printf("\tX4 (i=%d)", i); return 0; }
		}
	}
	else {
		printf("\tX5 ");
		return 0;
	}


	// Since no check caused the function to terminate this report is safe
	printf("\tsafe\n");
	return 1;
}


/*
 * Explanation
 * -----------
 *
 * We want to compare the numeric differences of the two lists and sum them. After parsing the input file,
 * each list is repeatedly checked to get the next pair of smallest numbers via the `find_min_argpos` function.
 * In order to count each entry once, entries are replaced with the placeholder value `CHECKED_ID` after
 * being processed.
 *
 * Is this the best solution? Not at all.
 * The best (?) solution would be to simply sort both lists independently and then iterate over
 * each pair and summing the differences.
 *
 * The reason I came up with this much more convoluted method of solving this was because I was under the
 * impression the problem wanted to compare the *difference in position within the lists* of each pair.
*/

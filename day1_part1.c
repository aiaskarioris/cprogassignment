// Aias Karioris
#include <stdio.h>
#include <string.h>

#define FILE_SIZE	65536
#define LIST_SIZE	1000
#define CHECKED_ID	(0xFFFFFFFF)

unsigned int abs_diff(unsigned int a, unsigned int b);
size_t find_min_argpos(unsigned int* list, unsigned int len);

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


	// Parse the file into two lists
	unsigned int list_len = 0;
	unsigned int list1[LIST_SIZE], list2[LIST_SIZE];

	size_t i = 0; // Parsing index
	while(i < bytes_read) {
		// Find the next \n so that we know where the next line starts
		size_t newline_pos = 0;
		size_t p = i;
		for(; buffer[p] != '\n'; p++);

		// We'll use `sscanf` to parse our buffer
		if(!sscanf(buffer+i, "%u    %u\n", &list1[list_len], &list2[list_len])) {
			// this would be an error
			break;
		}

		// Update list length
		list_len += 1;

		// Set the `i` index to the next line
		i = p+1;
	}

	// In a loop, find everytime the two smallest numbers in both lists and compare their position differences.
	// Since IDs are not unique we cannot sort the arrays; Instead, we'll have to check the whole lists in
	// each loop (O(N^2)). Each processed ID will be replaced with `CHECKED_ID` so that it will get skipped
	// in the next pass.
	size_t pos1, pos2;
	unsigned int result = 0;
	for(unsigned int i = 0; i < list_len; i++) {
		// Find smallest number in 1
		pos1 = find_min_argpos(list1, list_len);
		// Find smallest number in 2
		pos2 = find_min_argpos(list2, list_len);

		pos2, abs_diff(pos1, pos2);
		// Compare position difference
		result += abs_diff(list1[pos1], list2[pos2]);

		// Clear these values from the lists
		list1[pos1] = CHECKED_ID;
		list2[pos2] = CHECKED_ID;
	}

	printf("* Result: %u\n\n", result);

	return 0;
}

size_t find_min_argpos(unsigned int* list, unsigned int len) {
	unsigned int curr_min = CHECKED_ID;
	size_t pos = 0;

	for(size_t i = 0; i < len; i++) {
		if(curr_min > list[i]) {
			curr_min = list[i];
			pos = i;
		}
	}

	return pos;
}

unsigned int abs_diff(unsigned int a, unsigned int b) {
	return (a >= b) ? (a - b) : (b - a);
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

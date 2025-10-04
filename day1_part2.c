// Aias Karioris
#include <stdio.h>
#include <string.h>

#define FILE_SIZE	65536
#define LIST_SIZE	1000

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

	// We'll iterate through list2 once for every entry in list1
	unsigned int result = 0;
	for(unsigned int i1 = 0; i1 < list_len; i1++) {
		unsigned int current_result = 0;

		for(unsigned int i2 = 0; i2 < list_len; i2++) {
			if(list1[i1] == list2[i2]) {
				current_result += 1;
			}
		}

		// Multiply the current result by the current number and accumulate
		result += list1[i1] * current_result;
	}

	printf("* Result: %u\n\n", result);

	return 0;
}


/*
 * Explanation
 * -----------
 *
 * The parsing part is the same as in Day1/Part1.
 * In this algorithm we simply go through every element of list1 and compare it with all elements of list2.
 * This is an O(N^2) operation.
 *
 * Another alternative would be to sort both arrays and then, when iterating elements in list1,
 * we could stop the iteration of list2 once its elements were greater than the current list1
 * element. In this method we could also re-use the last partial similarity score from the previous
 * list1 when two elements of list1 are the same. However, this method would be better only if an
 * O(NlogN) sorting algorithm was used; Otherwise, if an O(N^2) one was used, the alternative method
 * would prove to be slower than the present one.
*/

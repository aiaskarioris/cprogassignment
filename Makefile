all: day1 day2 day4

day1: 	
	gcc -o day1p1 day1_part1.c
	gcc -o day1p2 day1_part2.c

day2: 	
	gcc -o day2p1 day2_part1.c
	gcc -o day2p2 day2_part2.c


day4:
	gcc -o day4p1 day4_part1.c
	gcc -o day4p2 -g day4_part2.c

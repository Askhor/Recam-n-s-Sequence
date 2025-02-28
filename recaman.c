#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long number;
// typedef __int128 number;

typedef struct {
	number n;
	number current_term;
	number bound;
	char *bitset;
	int extra_state;
	number extra_counter;
} RecamanState;

void *myalloc(number bytes) {
	void *addr = malloc(bytes);
	if (addr != NULL)
		return addr;
	printf("Memory error, exiting\n");
	exit(-1);
}

RecamanState createState(number bound) {
	RecamanState state;
	state.bound = bound;
	state.n = 0;
	state.current_term = 0;
	state.bitset = myalloc(bound >> 3);
	for (int i = 0; i < bound >> 3; i++) {
		state.bitset[i] = 0;
	}
	state.bitset[0] = 1;
	state.extra_state = 0;
	state.extra_counter = 0;
	return state;
}

void destroyState(RecamanState *state) {
	free(state->bitset);
	state->bitset = 0;
}

void out_of_bounds(RecamanState *state, number num) {
	destroyState(state);
	printf("\n");
	exit(0);
}

char number_hit(RecamanState *state, number num) {
	if (num >= state->bound) {
		return 0;
	}

	return (state->bitset[num >> 3] >> (num & 0b111)) & 1;
}

void hit_number(RecamanState *state, number num) {
	if (num >= state->bound) {
		out_of_bounds(state, num);
		return;
	}

	state->bitset[num >> 3] |= (1 << (num & 0b111));
}

void print_help() {
	printf(
	    "Usage: recaman <MODE> <BOUND>\n"
	    "\n"
	    "This is the recaman tool for working with the recaman sequence "
	    "(https://oeis.org/A005132)\n"
	    "\n"
	    "The modes is a bit-string, where the meaning of the bits is:\n"
	    "\t1:\t Print the values of the recaman sequence\n"
	    "\t11_:\t Print the change in value (positive or negative) as "
	    "'x' and '_' respectively\n"
	    "\t101_:\t Print the change in value if it is not an oscillation\n"
	    "\t1_01_:\t Print the oscillations in change in value, collates "
	    "them to numbers\n"
	    "\t11_01_:\t Replaces oscillations with spaces\n");
}

int parse_base2(char *string) {
	int base = 2;
	int result = 0;
	int length = strlen(string);

	for (int valuation = 0; valuation < length; valuation++) {
		char digit = string[length - 1 - valuation] - '0';
		if (digit >= base || digit < 0) {
			printf("The digit %d is not allowed for base %d\n",
			       (int)digit, base);
			exit(-1);
		}
		result += (1 << valuation) * digit;
	}

	return result;
}

char *control_char(char up) { return up ? "x" : "_"; }

void process_control(RecamanState *state, int mode, char up) {
	if (mode & 8) {
		printf("%s", control_char(up));
	}
}

void process_oscillation(RecamanState *state, int mode) {
	if (!(mode & 16) || state->extra_counter == 0)
		return;

	if (mode & 32) {
		printf(" ");
	} else {
		printf("%lld", (long long)state->extra_counter);
		if (!(mode & 8)) {
			printf(" ");
		}
	}
	state->extra_counter = 0;
}

void process_change(RecamanState *state, int mode, number next) {
	char up = next >= state->current_term;

	if (mode & 4) {
		printf("%s", control_char(up));
		return;
	}

	if (state->extra_state) {
		if (up) {
			process_oscillation(state, mode);
			process_control(state, mode, 1);
		} else {
			state->extra_counter += 1;
			state->extra_state = 0;
		}
	} else {
		if (up) {
			state->extra_state = 1;
		} else {
			process_oscillation(state, mode);
			process_control(state, mode, 0);
		}
	}
}

void recaman(int mode, number bound) {
	if (bound & 0b111) {
		bound = (((bound >> 3) + 1) << 3);
	}

	RecamanState state = createState(bound);

	if (mode & 1)
		printf("%lld", (long long)state.current_term);

	while (1) {
		state.n += 1;
		number a = state.current_term;
		if (a - state.n >= 0 && (!number_hit(&state, a - state.n))) {
			a -= state.n;
			hit_number(&state, a);
		} else {
			a += state.n;
			hit_number(&state, a);
		}
		if (mode & 1)
			printf(", %lld", (long long)a);
		if (mode & 2) {
			process_change(&state, mode, a);
		}

		state.current_term = a;
	}

	destroyState(&state);
}

int main(int argc, char **args) {
	if (argc != 3) {
		print_help();
		return -1;
	}

	recaman(parse_base2(args[1]), atoi(args[2]));
	return 0;
}

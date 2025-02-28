#include <stdio.h>
#include <stdlib.h>
typedef long long number;

typedef struct {
	number n;
	number current_term;
	number bound;
	char *bitset;
	int extra_state;
} RecamanState;

RecamanState createState(number bound) {
	RecamanState state;
	state.bound = bound;
	state.n = 0;
	state.current_term = 0;
	state.bitset = malloc(bound >> 3);
	state.bitset[0] |= 1;
	state.extra_state = 0;
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
	printf("Usage: recaman <MODE> <BOUND>\n");
	printf("\n");
	printf("This is the recaman tool for working with the recaman sequence "
	       "(https://oeis.org/A005132)\n");
	printf("\n");
	printf("The possible modes are:\n");
	printf("\t0: Generate the values of the recaman sequence\n");
}

void recaman(int mode, number bound) {
	if (bound & 0b111) {
		bound = (((bound >> 3) + 1) << 3);
	}

	RecamanState state = createState(bound);

	printf("%lld", state.current_term);

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
		printf(", %lld", a);
		state.current_term = a;
	}

	destroyState(&state);
}

int main(int argc, char **args) {
	if (argc != 3) {
		print_help();
		return -1;
	}

	recaman(atoi(args[1]), atoi(args[2]));

	return 0;
}

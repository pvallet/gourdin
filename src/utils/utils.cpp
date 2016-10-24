#include "utils.h"

double randomD() {
	return (double) rand() / (double) RAND_MAX;
}

float randomF() {
	return (float) rand() / (float) RAND_MAX;
}

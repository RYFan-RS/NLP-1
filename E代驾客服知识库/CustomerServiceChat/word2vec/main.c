
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> 

#include <iostream>

#include "CustomerServiceChat/word2vec/distance.h"

static const long long max_size = 2000;         // max length of strings
static const long long N = 10;                  // number of closest words that will be shown
static const long long max_w = 50;              // max length of vocabulary entries

int main(int argc, char **argv) {
	robosay::base::WordDistance  wordDistance;
	char st1[max_size];
	char bestw[N][max_size];
	char file_name[max_size], st[100][max_size];
	float dist, len, bestd[N], vec[max_size];
	long long words, size, a, b, c, d, cn, bi[100];
	char ch;
	if (argc < 2) {
		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
		return 0;
	}

  	if(!wordDistance.Initialize(argv[1])){
		return 0;
	}

  while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    printf("Enter word or sentence (EXIT to break): ");
    a = 0;
    while (1) {
      st1[a] = fgetc(stdin);
      if ((st1[a] == '\n') || (st1[a] == ' ') || (st1[a] =='\t') || (st1[a] == '\n') 
	  	|| (a >= max_size - 1)) {
        st1[a] = 0;
        break;
      }
      a++;
    }
    if (!strcmp(st1, "EXIT")) break;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
 
 	std::vector<std::pair<std::string,float> > result = wordDistance.getWordDistance(st[0]);

    for (a = 0; a < result.size(); a++) printf("%50s\t\t%f\n", result[a].first.c_str(), result[a].second);
  }
  return 0;
}

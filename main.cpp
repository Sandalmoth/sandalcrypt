#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>


using namespace std;


#define BLOCK_SIZE 8


template <typename Trng>
void encrypt_single(const char &in, char &out, Trng &rng) {
  uniform_int_distribution<char> d(numeric_limits<char>::min(), numeric_limits<char>::max());
  out = in ^ d(rng);
}


template <typename Trng>
void encrypt(const char *in, char *out, size_t N, Trng &rng) {

  if (N % 2) {
    encrypt_single(in[N - 1], out[N - 1], rng);
    N -= 1;
  }

  vector<size_t> sff;
  sff.reserve(N);
  for (size_t i = 0; i < N; ++i) {
    sff.push_back(i);
  }
  shuffle(sff.begin(), sff.end(), rng);
  uniform_int_distribution<char> d(numeric_limits<char>::min(), numeric_limits<char>::max());
  for (size_t i = 0; i < N; i += 2) {
    char h = d(rng);
    out[sff[i]] = in[sff[i]] ^ h;
    out[sff[i+1]] = in[sff[i+1]] ^ h;
  }
  for (size_t i = 0; i < N; i += 2) {
    swap(out[sff[i]], out[sff[i+1]]);
  }
}


int main() {
  char s[32] = {"bananas"};
  char c[32];
  char s2[32];
  cout << s << endl;

  mt19937 rng;
  rng.seed(2701);

  encrypt(s, c, 7, rng);

  cout << c << endl;

  rng.seed(2701);
  encrypt(c, s2, 7, rng);

  cout << s2 << endl;
}

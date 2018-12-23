#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <tclap/CmdLine.h>


using namespace std;


const string VERSION = "1.0.1";


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


int main(int argc, char **argv) {

  mt19937 rng;

  string infile_name;
  string outfile_name;
  string key;

  try {

    TCLAP::CmdLine cmd("Simple file encryption/decryption. Probably neither fast nor secure. Have fun", ' ', VERSION);

		TCLAP::UnlabeledValueArg<string> a_input_file("input-file", "Path to input file", true, "", "Path to a valid file", cmd);
		TCLAP::UnlabeledValueArg<string> a_key("encryption-key", "Encryption key", true, "", "Any string", cmd);

    cmd.parse(argc, argv);

    infile_name = a_input_file.getValue();
    key = a_key.getValue();
    string ending(infile_name.end() - 3, infile_name.end());
    if (ending == ".sc") {
      outfile_name = string(infile_name.begin(), infile_name.end() - 3);
    } else {
      outfile_name = infile_name + ".sc";
    }

  } catch (TCLAP::ArgException &e) {
    cerr << "TCLAP Error: " << e.error() << endl << "\targ: " << e.argId() << endl;
    return 1;
  }

  fstream infile;
  fstream outfile;
  infile.open(infile_name, fstream::in);
  outfile.open(outfile_name, fstream::out | fstream::trunc);

  char buffer_in[BLOCK_SIZE];
  char buffer_out[BLOCK_SIZE];

  seed_seq ss(key.begin(), key.end());

  rng.seed(ss);

  while (true) {
    size_t i = 0;
    while (infile.get(buffer_in[i]) && ++i < BLOCK_SIZE);

    encrypt(buffer_in, buffer_out, i, rng);

    outfile.write(buffer_out, i);

    if (infile.eof()) break;
  }

  infile.close();
  outfile.close();
}

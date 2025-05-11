#include "utils.h"

vector<uint64_t> gen_log_uniform() {
  const size_t N = 100000;
  const unsigned random_bits = 64;

  default_random_engine gen;
  uniform_real_distribution<double> dist(0.0, random_bits * log(2));
  vector<uint64_t> vec;
  vec.reserve(N);
  while (vec.size() < N) vec.push_back(exp(dist(gen)));
  return vec;
}

// Read test vector from a file.
//
// Format: One unsigned number per line.
vector<uint64_t> read_test_vector(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (!f) {
    perror(filename);
    exit(1);
  }

  vector<uint64_t> vec;
  while (!feof(f)) {
    uint64_t val;
    int rc = fscanf(f, "%" SCNu64 "\n", &val);
    if (rc == 1) {
      vec.push_back(val);
    } else if (rc == EOF) {
      if (ferror(f)) {
        perror(filename);
        exit(1);
      } else {
        break;
      }
    }
  }
  return vec;
}

double time_decode(const uint8_t *in, vector<uint64_t> &out, decoder_func func,
                   unsigned repetitions) {
  using namespace chrono;
  auto before = high_resolution_clock::now();
  for (unsigned n = 0; n < repetitions; n++)
    func(in, out.data(), out.size());
  auto after = high_resolution_clock::now();
  auto ns = duration_cast<nanoseconds>(after - before).count();
  return double(ns) / 1e9;
}

double measure(const uint8_t *in, vector<uint64_t> &out, decoder_func func) {
  unsigned repetitions = 1;
  double runtime = time_decode(in, out, func, repetitions);
  while (runtime < 1) {
    repetitions *= 10;
    runtime = time_decode(in, out, func, repetitions);
  }

  repetitions = round(repetitions / runtime);
  runtime = time_decode(in, out, func, repetitions);
  return runtime / repetitions;
}

double do_codec(const codec_descriptor &codec,
                const vector<uint64_t> &numbers) {
  vector<uint64_t> buffer(numbers.size());
  auto encoded = codec.encoder(numbers);

  printf("%s: %.3f bytes/integer.\n", codec.name,
         double(encoded.size()) / numbers.size());
  fflush(stdout);

  double dtime = measure(encoded.data(), buffer, codec.decoder);
  assert(buffer == numbers);

  printf("%s: %.3e secs.\n", codec.name, dtime);

  return dtime;
}

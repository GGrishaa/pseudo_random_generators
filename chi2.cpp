#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define INTERVALS 16
#define LOWER_BOUND 6.262
#define UPPER_BOUND 27.488

using namespace std;

double compute_chi_2(const vector<uint32_t>& data, int k = INTERVALS) {
  uint64_t max = 0xFFFFFFFF;
  uint64_t step = (max + 1) / k;
  vector<int> experiment(k, 0);

  for (uint32_t x : data) {
    int idx = x / step;
    if (idx >= k) idx = k - 1;
    experiment[idx]++;
  }

  double expected = static_cast<double>(data.size()) / k;
  double chi2 = 0.0;
  for (int i = 0; i < k; ++i) {
    double diff = experiment[i] - expected;
    chi2 += diff * diff / expected;
  }
  return chi2;
}

int main() {
  const int samples = 20;
  for (int gen = 1; gen <= 3; ++gen) {
    string data_path = "generated_data/" + to_string(gen) + "/data.txt";
    ifstream data_in(data_path);
    string chi2_path = "generated_data/" + to_string(gen) + "/chi2.txt";
    ofstream chi2_out(chi2_path);
    if (!data_in || !chi2_out) {
      cerr << "Ошибка открытия файлов для генератора " << gen << endl;
      return 1;
    }
    chi2_out << "Выборка chi2 равномерность" << endl;
    for (int i = 1; i <= samples; ++i) {
      string line;
      if (!getline(data_in, line)) break;
      vector<uint32_t> sample;
      size_t pos = 0;
      while (pos < line.size()) {
        size_t end = line.find(' ', pos);
        if (end == string::npos) end = line.size();
        uint32_t val = stoul(line.substr(pos, end - pos));
        sample.push_back(val);
        pos = end + 1;
      }
      double chi2 = compute_chi_2(sample);
      bool is_good = (chi2 >= LOWER_BOUND && chi2 <= UPPER_BOUND);
      chi2_out << i << ' ' << chi2 << ' ' << is_good << endl;
    }
    data_in.close();
  }
  return 0;
}

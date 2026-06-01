#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main() {
  for (int gen = 1; gen <= 3; ++gen) {
    string data_path = "generated_data/" + to_string(gen) + "/data.txt";
    ifstream data_in(data_path);
    string bin_path = "generated_data/" + to_string(gen) + "/bin_data.txt";
    ofstream bin_out(bin_path);
    if (!data_in) {
      cerr << "Ошибка считывания выборки генератора " << gen << endl;
      return 1;
    }
    string line;
    while (getline(data_in, line)) {
      size_t pos = 0;
      while (pos < line.size()) {
        size_t end = line.find(' ', pos);
        if (end == string::npos) end = line.size();
        string token = line.substr(pos, end - pos);
        uint32_t val = stoul(token);
        for (int bit = 31; bit >= 0; --bit) {
          bin_out << ((val >> bit) & 1);
        }
        pos = end + 1;
      }
      bin_out << endl;
    }
    data_in.close();
    bin_out.close();
  }
  return 0;
}
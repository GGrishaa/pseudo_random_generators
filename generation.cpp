#include "generators.hpp"

int main() {
  const int samples = 20;
  const int sample_size = 1000;
  vector<uint32_t> sample;
  uint32_t min, max, diff = 0;

  for (int gen = 1; gen <= 3; ++gen) {
    string dir = "generated_data/" + to_string(gen);
    string data_path = dir + "/data.txt";
    string stats_path = dir + "/stats.txt";
    ofstream data_out(data_path);
    ofstream stats_out(stats_path);
    if (!data_out || !stats_out) {
      cerr << "Ошибка открытия файлов для генератора " << gen << endl;
      return 1;
    }
    stats_out << "Выборка размер среднее отклонение коэффициент_вариации"
              << endl;
    for (int s = 1; s <= samples; ++s) {
      diff = 0;
      sample.clear();
      switch (gen) {
        case 1:
          lcg_seed(s);
          for (int i = 0; i < sample_size || diff < 5000; ++i) {
            uint32_t elem = next_lcg();
            if (!i)
              min = max = elem;
            else if (elem < min)
              min = elem;
            else if (elem > max)
              max = elem;
            diff = max - min;
            sample.push_back(elem);
          }
          break;
        case 2:
          lagfib_carry_seed(s);
          for (int i = 0; i < sample_size || diff < 5000; ++i) {
            uint32_t elem = next_lagfib_carry();
            if (!i)
              min = max = elem;
            else if (elem < min)
              min = elem;
            else if (elem > max)
              max = elem;
            diff = max - min;
            sample.push_back(elem);
          }
          break;
        case 3:
          rotmix_seed(s);
          for (int i = 0; i < sample_size || diff < 5000; ++i) {
            uint32_t elem = next_rotmix();
            if (!i)
              min = max = elem;
            else if (elem < min)
              min = elem;
            else if (elem > max)
              max = elem;
            diff = max - min;
            sample.push_back(elem);
          }
          break;
      }
      for (int i = 0; i < sample.size(); ++i) {
        data_out << sample[i] << ' ';
      }
      data_out << endl;
      double sum = 0.0, sq_sum = 0.0;
      for (uint32_t x : sample) sum += static_cast<double>(x);
      double mean = sum / sample.size();
      for (uint32_t x : sample) {
        double diff = static_cast<double>(x) - mean;
        sq_sum += diff * diff;
      }
      double var = sq_sum / sample.size();
      double stddev = sqrt(var);
      double cv = (mean != 0.0) ? (stddev / mean) : 0.0;
      stats_out << s << ' ' << sample.size() << ' ' << mean << ' ' << stddev
                << ' ' << cv << endl;
    }
    data_out.close();
    stats_out.close();
  }
  return 0;
}
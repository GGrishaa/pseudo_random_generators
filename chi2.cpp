/**
 * @file chi2.cpp
 * @brief Проверка равномерности распределения выборок с помощью критерия
 * хи-квадрат
 *
 * Программа читает выборки из файлов `generated_data/<gen>/data.txt`,
 * вычисляет для каждой выборки значение хи-квадрат и проверяет, попадает ли оно
 * в интервал, соответствующий 95% доверительной вероятности
 * Результаты записываются в `generated_data/<gen>/chi2.txt`
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/** @def INTERVALS
 *  @brief Количество интервалов разбиения для проверки критерия хи-квадрат
 */
#define INTERVALS 16
/** @def LOWER_BOUND
 *  @brief Нижняя граница критической области для хи-квадрат
 *  Для 15 степеней свободы и уровня значимости 0.05 значение равно 6.262
 */
#define LOWER_BOUND 6.262
/** @def UPPER_BOUND
 *  @brief Верхняя граница критической области для хи-квадрат
 *  Для 15 степеней свободы и уровня значимости 0.05 значение равно 27.488
 */
#define UPPER_BOUND 27.488

using namespace std;

/**
 * @brief Вычисляет значение статистики хи-квадрат для выборки
 *
 * Диапазон возможных значений [0, 2^32-1] разбивается на `k` равных интервалов
 * Для каждого интервала подсчитывается наблюдаемая частота попаданий,
 * затем вычисляется:
 *   χ² = \sum(observed_i - expected)^2 / expected,
 * где expected = N / k
 *
 * @param data Вектор 32-битных целых чисел
 * @param k    Количество интервалов (по умолчанию k = INTERVALS = 16)
 * @return     Значение статистики хи-квадрат
 */
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

/**
 * @brief Главная функция проверки критерия хи-квадрат
 *
 * Для каждого генератора (1..3) читает 20 выборок из `data.txt`,
 * вычисляет хи-квадрат и записывает результат в `chi2.txt` (номер выборки,
 * полученное значение и флаг прохождения теста через пробелы), прохождением
 * считается попадание в интервал [LOWER_BOUND, UPPER_BOUND]
 *
 * @return 0 при успешном завершении, 1 при ошибке открытия файлов
 */
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

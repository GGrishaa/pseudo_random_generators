/**
 * @file timer.cpp
 * @brief Замер времени генерации псевдослучайных чисел
 *
 * Программа измеряет время, необходимое для генерации 1000, 10000, 100000 и
 * 1 000 000 чисел для каждого из четырёх генераторов:
 * - 1: LCG с перемешиванием
 * - 2: Фибоначчи с запаздыванием и переносом
 * - 3: RotMix
 * - 4: Стандартный std::mt19937
 *
 * Результаты сохраняются в файлы `time/<gen>/time.txt`, каждый содержит строки:
 *   <размер> <время_в_секундах>
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "generators.hpp"

/**
 * @brief Главная функция замера времени
 *
 * Перебирает все генераторы (1..4) и все заданные объёмы выборок
 * Для каждого объёма:
 * - Инициализирует генератор соответствующим начальным значением - номером
 * выборки
 * - Засекает время до и после цикла генерации `size` чисел
 * - Записывает размер и затраченное время в файл time/<gen>/time.txt
 *
 * @return 0 при успешном завершении, 1 при ошибке открытия файла
 */
int main() {
  const vector<int> sizes = {1000, 10000, 100000, 1000000};
  for (int gen = 1; gen <= 4; ++gen) {
    string time_path = "time/" + to_string(gen) + "/time.txt";
    ofstream time_out(time_path);
    if (!time_out) {
      cerr << "Ошибка открытия файла " << time_path << endl;
      return 1;
    }
    uint32_t temp;
    for (int i = 0; i < sizes.size(); ++i) {
      int size = sizes[i];
      if (gen == 1) {
        time_out << size << ' ';
        lcg_seed(i + 1);
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) temp = next_lcg();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> dur = end - start;
        time_out << dur.count() << endl;
      } else if (gen == 2) {
        time_out << size << ' ';
        lagfib_carry_seed(i + 1);
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) temp = next_lagfib_carry();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> dur = end - start;
        time_out << dur.count() << endl;
      } else if (gen == 3) {
        time_out << size << ' ';
        rotmix_seed(i + 1);
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) temp = next_rotmix();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> dur = end - start;
        time_out << dur.count() << endl;
      } else {
        time_out << size << ' ';
        mt19937 mt_gen(i + 1);
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) temp = mt_gen();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> dur = end - start;
        time_out << dur.count() << endl;
      }
    }
    time_out.close();
  }
  return 0;
}
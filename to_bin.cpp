/**
 * @file to_bin.cpp
 * @brief Преобразование выборок из десятичного формата в битовый
 *
 * Программа читает файлы `generated_data/<gen>/data.txt`, где каждая строка
 * содержит 32-битные целые числа, разделённые пробелами
 * Для каждого числа записывается его 32-битное двоичное представление
 * (от старших к младшим битам) в файл `generated_data/<gen>/bin_data.txt`
 * Каждая выборка сохраняется в отдельной строке
 */

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief Главная функция преобразования в двоичный вид
 *
 * Для каждого генератора (1..3) открывает `data.txt`, читает строку за строкой,
 * разбивает на числа, преобразует каждое число в 32 бита и записывает в
 * `bin_data.txt` без пробелов, разделяя строки выборок символом новой строки
 *
 * @return 0 при успешном завершении, 1 при ошибке открытия файлов
 */
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
/**
 * @file nist_test.cpp
 * @brief Реализация пяти статистических тестов NIST SP800-22
 *
 * Программа читает битовые последовательности из файлов `bin_data.txt`,
 * сгенерированных на основе выборок, и применяет к ним следующие тесты:
 * - Frequency (Monobit)
 * - Frequency within a Block (M = 20)
 * - Runs
 * - Longest Run of Ones in a Block (M = 128)
 * - Serial (m = 3)
 *
 * Результаты (P‑values) сохраняются в `nist.txt` в каждой папке генератора
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/** @def ALPHA
 *  @brief Уровень значимости для принятия/отклонения нулевой гипотезы
 */
#define ALPHA 0.01

using namespace std;

/**
 * @brief Вычисляет неполную гамма-функцию Q(a,x) = igamc(a,x)
 *
 * Реализация основана на аппроксимациях из Numerical Recipes
 * Для x < a+1 используется разложение в ряд, иначе – цепная дробь
 *
 * @param a Параметр формы (a > 0)
 * @param x Верхний предел интегрирования (x >= 0)
 * @return Значение igamc(a,x) = 1 - P(a,x)
 */
double igamc(double a, double x) {
  if (x < 0.0 || a <= 0.0) return 1.0;
  if (x < a + 1.0) {
    double sum = 1.0 / a;
    double term = sum;
    for (int i = 1; i <= 100; ++i) {
      term *= x / (a + i);
      sum += term;
      if (fabs(term) < 1e-15) break;
    }
    return 1.0 - sum * exp(-x + a * log(x) - lgamma(a));
  } else {
    double b = x + 1.0 - a, c = 1.0 / 1e-30, d = 1.0 / b, h = d;
    for (int i = 1; i <= 100; ++i) {
      double an = -i * (i - a);
      b += 2.0;
      d = an * d + b;
      if (fabs(d) < 1e-30) d = 1e-30;
      c = b + an / c;
      if (fabs(c) < 1e-30) c = 1e-30;
      d = 1.0 / d;
      double del = d * c;
      h *= del;
      if (fabs(del - 1.0) < 1e-12) break;
    }
    return h * exp(-x + a * log(x) - lgamma(a));
  }
}

/**
 * @brief Преобразует строку из символов '0'/'1' в вектор bool
 *
 * @param s Входная строка, состоящая только из '0' и '1'
 * @return Вектор булевых значений (true – '1', false – '0')
 */
vector<bool> string_to_bits(const string& s) {
  vector<bool> bits;
  bits.reserve(s.size());
  for (char ch : s) bits.push_back(ch == '1');
  return bits;
}

/**
 * @brief Частотный/монобитный тест NIST
 *
 * Проверяет, близка ли доля единиц к 1/2
 *
 * @param bits Последовательность битов
 * @return P‑value
 */
double frequency_test(const vector<bool>& bits) {
  double sum = 0.0;
  for (bool b : bits) sum += (b ? 1.0 : -1.0);
  double s_obs = fabs(sum) / sqrt(bits.size());
  return erfc(s_obs / sqrt(2.0));
}

/**
 * @brief Блочный частотный тест (M = 20)
 *
 * Разбивает последовательность на блоки по M бит и проверяет,
 * что доля единиц в каждом блоке близка к 0.5
 *
 * @param bits Последовательность битов
 * @param M    Размер блока (по умолчанию 20)
 * @return P‑value
 */
double block_frequency_test(const vector<bool>& bits, int M = 20) {
  int n = bits.size(), N = n / M;
  if (!N) return 1.0;
  double chi2 = 0.0;
  for (int i = 0; i < N; ++i) {
    int ones = 0;
    for (int j = 0; j < M; ++j)
      if (bits[i * M + j]) ++ones;
    double pi = static_cast<double>(ones) / M;
    chi2 += (pi - 0.5) * (pi - 0.5);
  }
  chi2 = 4.0 * M * chi2;
  return igamc(N / 2.0, chi2 / 2.0);
}

/**
 * @brief Тест на серии (Runs Test)
 *
 * Проверяет, не слишком быстро или слишком медленно чередуются ли 0 и 1
 *
 * @param bits Последовательность битов
 * @return P‑value (0, если частотный тест предварительно не пройден)
 */
double runs_test(const vector<bool>& bits) {
  int n = bits.size();
  int ones = 0;
  for (bool b : bits)
    if (b) ones++;
  double pi = static_cast<double>(ones) / n;
  if (fabs(pi - 0.5) >= 2.0 / sqrt(n)) return 0.0;
  int runs = 1;
  for (int i = 1; i < n; ++i)
    if (bits[i] != bits[i - 1]) runs++;
  double numerator = runs - 2.0 * n * pi * (1.0 - pi);
  double denominator = 2.0 * sqrt(2.0 * n) * pi * (1.0 - pi);
  double z = numerator / denominator;
  return erfc(fabs(z) / sqrt(2.0));
}

/**
 * @brief Тест на самую длинную серию единиц в блоке (M = 128)
 *
 * Разбивает последовательность на блоки по 128 бит и сравнивает
 * распределение максимальных длин серий единиц с теоретическим
 *
 * @param bits Последовательность битов
 * @return P‑value
 */
double longest_run_test(const vector<bool>& bits) {
  int n = bits.size();
  const int M = 128;
  int N = n / M;
  if (N == 0) return 1.0;
  int freq[6] = {0};
  for (int i = 0; i < N; ++i) {
    int longest = 0, cur = 0;
    for (int j = 0; j < M; ++j) {
      if (bits[i * M + j]) {
        cur++;
        if (cur > longest) longest = cur;
      } else
        cur = 0;
    }
    if (longest <= 4)
      freq[0]++;
    else if (longest == 5)
      freq[1]++;
    else if (longest == 6)
      freq[2]++;
    else if (longest == 7)
      freq[3]++;
    else if (longest == 8)
      freq[4]++;
    else
      freq[5]++;
  }
  double pi[6] = {0.1174, 0.2430, 0.2493, 0.1752, 0.1027, 0.1124};
  double chi2 = 0.0;
  for (int i = 0; i < 6; ++i) {
    double expected = N * pi[i];
    if (expected > 0) {
      double diff = freq[i] - expected;
      chi2 += diff * diff / expected;
    }
  }
  return igamc(5.0 / 2.0, chi2 / 2.0);
}

/**
 * @brief Серийный тест NIST (m = 3)
 *
 * Проверяет равномерность распределения всех возможных m-битовых шаблонов
 * Возвращает минимальное P‑value из двух компонент теста
 *
 * @param bits Последовательность битов
 * @param m    Длина шаблона (по умолчанию 3)
 * @return P‑value
 */
double serial_test(const vector<bool>& bits, int m = 3) {
  int n = bits.size();
  vector<bool> ext = bits;
  for (int i = 0; i < m - 1; ++i) ext.push_back(bits[i]);
  int N = n;
  int total_m = 1 << m;
  vector<int> freq_m(total_m, 0);
  for (int i = 0; i < N; ++i) {
    int idx = 0;
    for (int j = 0; j < m; ++j) idx = (idx << 1) | (ext[i + j] ? 1 : 0);
    freq_m[idx]++;
  }
  double psi_m = 0.0;
  for (int v : freq_m) psi_m += (double)v * v;
  psi_m = (double)(1 << m) / N * psi_m - N;
  int total_m1 = 1 << (m - 1);
  vector<int> freq_m1(total_m1, 0);
  for (int i = 0; i < N; ++i) {
    int idx = 0;
    for (int j = 0; j < m - 1; ++j) idx = (idx << 1) | (ext[i + j] ? 1 : 0);
    freq_m1[idx]++;
  }
  double psi_m1 = 0.0;
  for (int v : freq_m1) psi_m1 += (double)v * v;
  psi_m1 = (double)(1 << (m - 1)) / N * psi_m1 - N;
  double psi_m2 = 0.0;
  if (m - 2 >= 1) {
    int total_m2 = 1 << (m - 2);
    vector<int> freq_m2(total_m2, 0);
    for (int i = 0; i < N; ++i) {
      int idx = 0;
      for (int j = 0; j < m - 2; ++j) idx = (idx << 1) | (ext[i + j] ? 1 : 0);
      freq_m2[idx]++;
    }
    for (int v : freq_m2) psi_m2 += (double)v * v;
    psi_m2 = (double)(1 << (m - 2)) / N * psi_m2 - N;
  }
  double nabla1 = psi_m - psi_m1;
  double nabla2 = psi_m - 2 * psi_m1 + psi_m2;
  int df1 = (1 << (m - 1)) - 1;
  int df2 = (1 << (m - 2)) - 1;
  double P1 = igamc(df1 / 2.0, nabla1 / 2.0);
  double P2 = igamc(df2 / 2.0, nabla2 / 2.0);
  return min(P1, P2);
}

/**
 * @brief Главная функция
 *
 * Для каждого генератора (1..3) читает битовые строки из `bin_data.txt`,
 * применяет пять тестов NIST и записывает P‑values в `nist.txt`
 *
 * @return 0 при успешном завершении, 1 при ошибке открытия файлов
 */
int main() {
  const int samples = 20;
  for (int gen = 1; gen <= 3; ++gen) {
    string bin_path = "generated_data/" + to_string(gen) + "/bin_data.txt";
    string nist_path = "generated_data/" + to_string(gen) + "/nist.txt";
    ifstream bin_in(bin_path);
    ofstream nist_out(nist_path);
    if (!bin_in || !nist_out) {
      cerr << "Ошибка открытия при проведении NIST-тестов для выборок "
              "генератора "
           << gen << endl;
      return 1;
    }
    nist_out << "sample frequency block_frequency runs longest_run serial"
             << endl;
    string line;
    int idx = 0;
    while (getline(bin_in, line) && idx < samples) {
      ++idx;
      vector<bool> bits = string_to_bits(line);
      double p_freq = frequency_test(bits),
             p_block = block_frequency_test(bits, 20), p_runs = runs_test(bits),
             p_long = longest_run_test(bits), p_ser = serial_test(bits, 3);
      nist_out << idx << ' ' << p_freq << ' ' << p_block << ' ' << p_runs << ' '
               << p_long << ' ' << p_ser << endl;
    }
    bin_in.close();
    nist_out.close();
  }
  return 0;
}
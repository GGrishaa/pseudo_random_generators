import matplotlib.pyplot as plt

def read_time(path):
    times = []
    with open(path) as f:
        for line in f:
            if not line.strip():
                continue
            parts = line.split()
            times.append(float(parts[1]))
    return times

t1 = read_time('time/1/time.txt')
t2 = read_time('time/2/time.txt')
t3 = read_time('time/3/time.txt')
t4 = read_time('time/4/time.txt')

sizes = [1000, 10000, 100000, 1000000]

plt.figure(figsize=(10, 6))
plt.loglog(sizes, t1, 'o-', label='Генератор 1')
plt.loglog(sizes, t2, 's-', label='Генератор 2')
plt.loglog(sizes, t3, '^-', label='Генератор 3')
plt.loglog(sizes, t4, 'd-', label='std::mt19937')

plt.xlabel('Размер выборки', fontsize=12)
plt.ylabel('Время (в секундах)', fontsize=12)
plt.title('Сравнение скорости генерации псевдослучайных чисел', fontsize=14)
plt.legend()
plt.tight_layout()
plt.savefig('time/time_plot.png')
plt.show()
////////////////////////////////////////////////////////////////////////////////
/// Данный пример занимается поиском простых чисел в заданном диапазоне.
/// Результом работы являются количество найденных чисел в указанном диапазоне.
///
/// Программа принимает на вход один аргумент:
///     - правую границу диапозона для поиска простых чисел
///
/// При изучении программы следует обратить внимание на следующие моменты:
/// 1. Определение количества создаваемых потоков с помощью статического метода
///    std::thread::hardware_concurrency().
/// 2. Создание потока и передача в него аргументов.
/// 3. Использование std::atomic для подсчета количества простых чисел.
/// 4. Ожидание завершения потоков (std::thread::join()).
/// 5. Использование std::chrono::system_clock для измрения длительности
///    выполнения участка кода.
///
/// С данным примером рекомендуется провести следующие эксперименты:
/// 1. Установить вручную разные значения для переменной N_THREADS: 1, 32, 256.
///    Обратить внимание как меняется время работы программы.
///
/// Текущая реализация программы не является оптимальной для данной задачи,
/// предназначена для демонстрации особенностей и имеет сложность O(N^(3/2))
///
/// Количество простых чисел до 1000     = 168
/// Количество простых чисел до 10000    = 1229
/// Количество простых чисел до 100000   = 9592
/// Количество простых чисел до 1000000  = 78498
/// Количество простых чисел до 10000000 = 664579
////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>

// Количество создаваемых потоков
const unsigned int N_THREADS = std::thread::hardware_concurrency();           

// количество найденных простых чисел в диапазоне [2, N]
std::atomic<unsigned long> n_prime_numbers(0);

////////////////////////////////////////////////////////////////////////////////
// Функция для проверки, является ли заданное число простым
////////////////////////////////////////////////////////////////////////////////
bool is_prime(unsigned long x)
{
	unsigned long last = (long)floor(sqrt(x));
	for (unsigned long i = 2; i <= last; ++i)
		if ((x % i) == 0) return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Функция потока для подсчета простых чисел в диапазоне [left, right)
////////////////////////////////////////////////////////////////////////////////
void countPrimes(unsigned long left, unsigned long right)
{
	for (unsigned long i = left; i < right; ++i)
	{
		if (is_prime(i))
			n_prime_numbers++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Основная программа
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	// время запуска программы
	auto t0 = std::chrono::system_clock::now();

	// определение размера решаемой задачи из параметров командной строки
	const unsigned long N = (argc == 2) ? atol(argv[1]) : 10000000;

	// количество чисел для проверки одним потоком
	const unsigned long step = (N - 2)/N_THREADS;

	// запускаем расчеты
	printf("Searching prime numbers in interval [%lu, %lu)\n", 2, N);

	// Создание требуемого (N_THREADS) числа потоков
	printf("Creating %u threads\n", N_THREADS);
	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < N_THREADS; ++i)
	{
		unsigned long left = 2 + i * step;
		unsigned long right = (i == N_THREADS - 1) ? N : left + step;
		threads.push_back(std::thread(countPrimes, left, right));
	}

	// Ждем завершения всех потоков
	for (auto& th : threads)
		th.join();

	// вывод количества найденых простых чисел в диапазоне
	printf("Found %lu prime numbers\n", n_prime_numbers.load());

	// время окончания работы программы
	auto t1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = t1-t0;

	// вывод затраченного времени
	printf("Execution time   : %.5f s\n", elapsed_seconds.count());	

	return 0;
}

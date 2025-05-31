/**
 * @file main.cpp
 * @brief задача написать программу, которая будет высчитывать значение интеграла через метод трапеций
 * Особенность этой задачи: использовать потоки(threads) для ускорения вычислений (нужно разделить задачу для вычисления части интеграла на каждый поток)
 *
 * на вход программе при запуске(это в argv[]) подается 3 числа: a, b, n, tn где [a, b] - интервал (целочисленный, неотрицательный и не больше 50 для задачи), n - количество разбиений, tn - threads number - количество потоков для подсчёта
 * примечание: n - количество разбиений в тестах будет явно нацело делиться на tn - количество потоков.
 *
 * на выход программе выведите значение интеграла через метод трапеций (вывод в стандартный поток std::cout, в конце выведите '\n'), вывод в поток с точностью до 4 знака
 * в функции trapezoidalIntegral есть переменная const std::function<double (double)> &f, при вызове подсчётов передайте ссылку на функцию из задания
 * реализовать подсчёт интеграла S(a, b) = (1+e^x)^0.5 dx
 * 
 *
 * литература:
 * https://ru.wikipedia.org/wiki/Метод_трапеций
 * https://habr.com/ru/articles/420867/
 */


#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <stdexcept>


double targetFunction(double x) {
    return std::sqrt(1 + std::exp(x));
}


double trapezoidalIntegral(double a, double b, int n, int tn, const std::function<double(double)> &f) {
    double h = (b - a) / static_cast<double>(n);
    std::vector<double> partialSums(tn, 0.0);
    std::vector<std::thread> threads;

    int chunkSize = n / tn;

    for (int t = 0; t < tn; ++t) {
        int startIdx = t * chunkSize;
        int endIdx = startIdx + chunkSize;

        threads.emplace_back([=, &partialSums, &f]() {
            double localSum = 0.0;
            for (int i = startIdx; i < endIdx; ++i) {
                double x0 = a + i * h;
                double x1 = a + (i + 1) * h;
                localSum += ((f(x0) + f(x1)) / 2.0)*h;
            }
            partialSums[t] = localSum;
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    double totalSum = std::accumulate(partialSums.begin(), partialSums.end(), 0.0);
    return totalSum * h;
}




int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            throw std::invalid_argument("Expected 4 arguments: a b n tn");
        }

        int a = std::stoi(argv[1]);
        int b = std::stoi(argv[2]);
        int n = std::stoi(argv[3]);
        int tn = std::stoi(argv[4]);


        if (a < 0 || b < 0 || a > 50 || b > 50 || a >= b) {
            throw std::invalid_argument("Нарушены границы отрезка [a, b]");
        }
        if (n <= 0 || tn <= 0 || n % tn != 0) {
            throw std::invalid_argument("n должно целочисленно делиться на tn");
        }

        
        double result = trapezoidalIntegral(a, b, n, tn, targetFunction);
        std::cout << std::fixed << std::setprecision(4) << result << '\n';
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
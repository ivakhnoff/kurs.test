#pragma once
#include <vector>
#include <iostream>
#include <limits>
#include <stdexcept> // Для исключений
#include <boost/numeric/conversion/cast.hpp> // Для проверки на переполнение

class calc {
public:
    uint32_t res; // Результат вычислений

    // Конструктор
    calc(std::vector<uint32_t> chisla);

    // Метод для получения результата
    uint32_t send_res();
};

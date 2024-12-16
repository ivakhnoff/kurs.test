#include "calc.h"
#include "error.h"
#include <boost/numeric/conversion/cast.hpp> // Для проверки на переполнение
#include <limits>
#include <stdexcept>

calc::calc(std::vector<uint32_t> chisla) {
    if (chisla.empty()) {
        throw no_crit_err("Должен быть хотя бы один элемент в векторе");
    }

    uint64_t temp_res = 0; // Используем uint64_t для промежуточных вычислений
    for (auto elem : chisla) {
        if (temp_res > std::numeric_limits<uint32_t>::max() - elem) {
            throw boost::numeric::positive_overflow(); // Переполнение
        }
        temp_res += elem;
    }

    if (temp_res > std::numeric_limits<uint32_t>::max()) {
        res = std::numeric_limits<uint32_t>::max(); // Устанавливаем максимальное значение
    } else {
        res = static_cast<uint32_t>(temp_res); // Преобразуем обратно в uint32_t
    }
}

uint32_t calc::send_res() {
    return res;
}

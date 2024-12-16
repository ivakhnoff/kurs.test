#include "error.h"
#include "base.h"
#include "interface.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void base::connect(string f) {
    // Проверка расширения файла
    if (f.find('.') == std::string::npos) {
        throw crit_err("НЕКОРРЕКТНЫЙ ФАЙЛ БД. КРИТИЧЕСКАЯ ОШИБКА");
    }

    ifstream rfile;
    rfile.open(f);
    if (!rfile.is_open()) {
        throw crit_err("НЕКОРРЕКТНЫЙ ФАЙЛ БД. КРИТИЧЕСКАЯ ОШИБКА");
    }

    string line, pass, login;
    int line_number = 0; // Для отслеживания номера строки

    while (getline(rfile, line)) {
        line_number++;

        // Проверка на пустую строку
        if (line.empty()) {
            continue; // Пропускаем пустые строки
        }

        // Удаляем пробелы в начале и конце строки
        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);

        size_t i = line.find(':');
        if (i == std::string::npos) {
            std::ostringstream error_msg;
            error_msg << "Формат строки неверен на строке " << line_number << "; требуется 'login:password'";
            throw crit_err(error_msg.str());
        }

        // Проверка на наличие нескольких разделителей
        if (line.find(':', i + 1) != std::string::npos) {
            std::ostringstream error_msg;
            error_msg << "Обнаружено несколько разделителей ':' на строке " << line_number;
            throw crit_err(error_msg.str());
        }

        std::string login = line.substr(0, i);
        std::string pass = line.substr(i + 1);

        // Удаляем пробелы в логине и пароле
        login.erase(0, login.find_first_not_of(' '));
        login.erase(login.find_last_not_of(' ') + 1);
        pass.erase(0, pass.find_first_not_of(' '));
        pass.erase(pass.find_last_not_of(' ') + 1);

        // Проверка на пустые значения
        if (login.empty() && !pass.empty()) {
            std::ostringstream error_msg;
            error_msg << "Присутствует только пароль без логина на строке " << line_number;
            throw crit_err(error_msg.str());
        }
        if (!login.empty() && pass.empty()) {
            std::ostringstream error_msg;
            error_msg << "Присутствует только логин без пароля на строке " << line_number;
            throw crit_err(error_msg.str());
        }

        // Проверка длины логина и пароля
        if (login.length() > 20) { // Пример ограничения на длину логина
            std::ostringstream error_msg;
            error_msg << "Длина логина превышает допустимое значение на строке " << line_number;
            throw crit_err(error_msg.str());
        }
        if (pass.length() > 20) { // Пример ограничения на длину пароля
            std::ostringstream error_msg;
            error_msg << "Длина пароля превышает допустимое значение на строке " << line_number;
            throw crit_err(error_msg.str());
        }

        // Добавление данных в базу
        data_base[login] = pass;
    }

    // Проверка, что база данных не пуста
    if (data_base.empty()) {
        throw crit_err("НЕКОРРЕКТНЫЙ ФАЙЛ БД. КРИТИЧЕСКАЯ ОШИБКА");
    }

    // Проверка наличия обязательного логина "user"
    if (!has_login("user")) {
        throw crit_err("Обязательный логин 'user' отсутствует в базе данных");
    }
}

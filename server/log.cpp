#include "log.h"
#include "error.h"
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <filesystem>

// Конструктор по умолчанию
logger::logger() : path_to_logfile("") {}

// Конструктор с параметром
logger::logger(const std::string& path) : path_to_logfile(path) {}

// Установка пути к файлу лога
int logger::set_path(const std::string& path_file) {
    if (path_file.empty() || path_file.find('.') == std::string::npos) {
        throw crit_err("Неверное расширение файла лога.");
    }
    std::filesystem::path p(path_file);
    if (!std::filesystem::exists(p.parent_path())) {
        throw crit_err("Указанный путь к директории не существует.");
    }
    path_to_logfile = path_file;
    return 0;
}

// Запись сообщения в лог
int logger::writelog(const std::string& message) {
    if (path_to_logfile.empty()) {
        throw crit_err("Путь к файлу лога не задан.");
    }

    std::ofstream logfile(path_to_logfile, std::ios_base::out | std::ios_base::app);
    if (!logfile.is_open()) {
        throw crit_err("Ошибка открытия файла лога.");
    }

    logfile << gettime() << ' ' << message << std::endl;
    return 0;
}

// Получение текущего времени
std::string logger::gettime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return std::string(buf);
}

// Получение пути к файлу лога (для тестирования)
std::string logger::get_path() const {
    return path_to_logfile;
}

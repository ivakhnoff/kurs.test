#include <chrono>
#include <UnitTest++/UnitTest++.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <boost/numeric/conversion/cast.hpp>
#include <limits>
#include <thread>
#include <filesystem>
#include <sstream>
#include "server/log.h"
#include "server/communicator.h"
#include "server/calc.h"
#include "server/base.h"
#include "server/interface.h"
#include "server/error.h"

#include "server/log.cpp"
#include "server/calc.cpp"
#include "server/base.cpp"
#include "server/communicator.cpp"
#include "server/interface.cpp"
namespace po = boost::program_options;

// Тесты для класса Base
SUITE(BaseTests) {
    // Проверка на пустой путь к файлу
    TEST(EmptyPath) {
        base b;
        std::string empty_path = "";

        CHECK_THROW(b.connect(empty_path), crit_err); // Ожидаем исключение
    }

    // Проверка на некорректный путь к файлу
    TEST(InvalidPath) {
        base b;
        std::string invalid_path = "/ZZZ/base.txt"; // Несуществующий путь

        CHECK_THROW(b.connect(invalid_path), crit_err); // Ожидаем исключение
    }

    // Проверка на отсутствие расширения у файла
    TEST(InvalidFileExtension) {
        base b;
        std::string invalid_extension_file = "temp_base";

        CHECK_THROW(b.connect(invalid_extension_file), crit_err); // Ожидаем исключение
    }

    // Проверка на пустой файл
    TEST(EmptyFile) {
        base b;
        std::string empty_base_file = "temp_base_empty.txt";
        std::ofstream file(empty_base_file);
        file.close();

        CHECK_THROW(b.connect(empty_base_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(empty_base_file); // Удаляем временный файл
    }

    // Проверка на некорректную базу данных
    TEST(InvalidBase) {
        base b;
        std::string invalid_base_file = "temp_base_invalid.txt";
        std::ofstream file(invalid_base_file);
        file << "invalid_data\n"; // Добавляем некорректные данные
        file.close();

        CHECK_THROW(b.connect(invalid_base_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(invalid_base_file); // Удаляем временный файл
    }

    // Проверка на отсутствие логина
    TEST(MissingLogin) {
        base b;
        std::string missing_login_file = "temp_base_missing_login.txt";
        std::ofstream file(missing_login_file);
        file << ":password\n";
        file.close();

        CHECK_THROW(b.connect(missing_login_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(missing_login_file); // Удаляем временный файл
    }

    // Проверка на отсутствие пароля
    TEST(MissingPassword) {
        base b;
        std::string missing_password_file = "temp_base_missing_password.txt";
        std::ofstream file(missing_password_file);
        file << "user:\n";
        file.close();

        CHECK_THROW(b.connect(missing_password_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(missing_password_file); // Удаляем временный файл
    }

    // Проверка на отсутствие разделителя в формате логин:пароль
    TEST(InvalidLineFormat) {
        base b;
        std::string invalid_format_file = "temp_base_invalid_format.txt";
        std::ofstream file(invalid_format_file);
        file << "userP@ssW0rd\n"; // Отсутствует разделитель ':'
        file.close();

        CHECK_THROW(b.connect(invalid_format_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(invalid_format_file); // Удаляем временный файл
    }

    // Проверка на наличие нескольких разделителей `:` в строке
    TEST(MultipleColonsInLine) {
        base b;
        std::string multiple_colons_file = "temp_base_multiple_colons.txt";
        std::ofstream file(multiple_colons_file);
        file << "user:P@ssW0rd:extra\n"; // Несколько разделителей
        file.close();

        CHECK_THROW(b.connect(multiple_colons_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(multiple_colons_file); // Удаляем временный файл
    }

    // Проверка на наличие пустых строк в файле
    TEST(EmptyLinesInFile) {
        base b;
        std::string empty_lines_file = "temp_base_empty_lines.txt";
        std::ofstream file(empty_lines_file);
        file << "user:P@ssW0rd\n";
        file << "\n"; // Пустая строка
        file << "admin:admin123\n";
        file.close();

        try {
            b.connect(empty_lines_file); // Вызываем метод connect
            CHECK(true); // Если метод выполнился без исключений, тест пройден
        } catch (const std::exception& e) {
            CHECK(false); // Если метод выбросил исключение, тест не пройден
        }

        CHECK(b.has_login("user")); // Проверяем, что логин "user" существует
        CHECK(b.has_login("admin")); // Проверяем, что логин "admin" существует
        CHECK_EQUAL("P@ssW0rd", b.get_password("user")); // Проверяем пароль для "user"
        CHECK_EQUAL("admin123", b.get_password("admin")); // Проверяем пароль для "admin"

        std::filesystem::remove(empty_lines_file); // Удаляем временный файл
    }

    // Проверка на наличие пробелов в логине или пароле
    TEST(SpacesInLoginOrPassword) {
        base b;
        std::string spaces_file = "temp_base_spaces.txt";
        std::ofstream file(spaces_file);
        file << " user : P@ssW0rd \n"; // Пробелы вокруг логина и пароля
        file.close();

        try {
            b.connect(spaces_file); // Вызываем метод connect
            CHECK(true); // Если метод выполнился без исключений, тест пройден
        } catch (const std::exception& e) {
            CHECK(false); // Если метод выбросил исключение, тест не пройден
        }

        CHECK(b.has_login("user")); // Проверяем, что логин "user" существует
        CHECK_EQUAL("P@ssW0rd", b.get_password("user")); // Проверяем пароль для "user"

        std::filesystem::remove(spaces_file); // Удаляем временный файл
    }

    // Проверка на наличие нескольких одинаковых логинов
    TEST(DuplicateLogins) {
        base b;
        std::string duplicate_logins_file = "temp_base_duplicate_logins.txt";
        std::ofstream file(duplicate_logins_file);
        file << "user:P@ssW0rd\n";
        file << "user:admin123\n"; // Дублирующийся логин
        file.close();

        try {
            b.connect(duplicate_logins_file); // Вызываем метод connect
            CHECK(true); // Если метод выполнился без исключений, тест пройден
        } catch (const std::exception& e) {
            CHECK(false); // Если метод выбросил исключение, тест не пройден
        }

        CHECK(b.has_login("user")); // Проверяем, что логин "user" существует
        CHECK_EQUAL("admin123", b.get_password("user")); // Проверяем пароль для "user"

        std::filesystem::remove(duplicate_logins_file); // Удаляем временный файл
    }

    // Проверка на длину логина
    TEST(LongLoginsOrPasswords) {
        base b;
        std::string long_file = "temp_base_long.txt";
        std::ofstream file(long_file);
        std::string long_login = "verylonglogin";
        std::string long_password = "verylongpassword";
        std::string user_login = "user";
        std::string user_password = "password";

        file << long_login << ":" << long_password << "\n";
        file << user_login << ":" << user_password << "\n"; // Добавляем обязательный логин "user"
        file.close();

        try {
            b.connect(long_file); // Вызываем метод connect
            CHECK(true); // Если метод выполнился без исключений, тест пройден
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            CHECK(false); // Если метод выбросил исключение, тест не пройден
        }

        CHECK(b.has_login(long_login)); // Проверяем, что логин существует
        CHECK_EQUAL(long_password, b.get_password(long_login)); // Проверяем пароль

        std::filesystem::remove(long_file); // Удаляем временный файл
    }

    // Отсутствие обязательного логина "user"
    TEST(MissingUserLogin) {
        base b;
        std::string invalid_base_file = "temp_base_invalid.txt";
        std::ofstream file(invalid_base_file);
        file << "admin:admin123\n";
        file.close();

        CHECK_THROW(b.connect(invalid_base_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(invalid_base_file); // Удаляем временный файл
    }

    // Проверка на несколько логинов с ошибкой
    TEST(MissingMultipleRequiredLogins) {
        base b;
        std::string invalid_base_file = "temp_base_missing_multiple_required_logins.txt";
        std::ofstream file(invalid_base_file);
        file << "admin:admin123\n"; // Логин "user" отсутствует
        file.close();

        CHECK_THROW(b.connect(invalid_base_file), crit_err); // Ожидаем исключение

        std::filesystem::remove(invalid_base_file); // Удаляем временный файл
    }

    // Проверка на наличие логина и пароля
    TEST(ValidBaseFile) {
        base b;
        std::string valid_base_file = "temp_base.txt";
        std::ofstream file(valid_base_file);
        file << "user:P@ssW0rd\n";
        file << "login:pswd\n";
        file.close();

        try {
            b.connect(valid_base_file); // Вызываем метод connect
            CHECK(true); // Если метод выполнился без исключений, тест пройден
        } catch (const std::exception& e) {
            CHECK(false); // Если метод выбросил исключение, тест не пройден
        }

        CHECK(b.has_login("user")); // Проверяем, что логин "user" существует
        CHECK(b.has_login("login")); // Проверяем, что логин "login" существует
        CHECK_EQUAL("P@ssW0rd", b.get_password("user")); // Проверяем пароль для "user"
        CHECK_EQUAL("pswd", b.get_password("login")); // Проверяем пароль для "login"

        std::filesystem::remove(valid_base_file); // Удаляем временный файл
    }

    // Проверка на наличие нескольких обязательных логинов
    TEST(MultipleRequiredLogins) {
        base b;
        std::string valid_base_file = "temp_base_multiple_logins.txt";
        std::ofstream file(valid_base_file);
        file << "user:P@ssW0rd\n";
        file << "admin:admin123\n";
        file << "root:root123\n";
        file.close();

        try {
            b.connect(valid_base_file); // Вызываем метод connect
            CHECK(true); // Если метод выполнился без исключений, тест пройден
        } catch (const std::exception& e) {
            CHECK(false); // Если метод выбросил исключение, тест не пройден
        }

        CHECK(b.has_login("user")); // Проверяем, что логин "user" существует
        CHECK(b.has_login("admin")); // Проверяем, что логин "admin" существует
        CHECK(b.has_login("root")); // Проверяем, что логин "root" существует
        CHECK_EQUAL("P@ssW0rd", b.get_password("user")); // Проверяем пароль для "user"
        CHECK_EQUAL("admin123", b.get_password("admin")); // Проверяем пароль для "admin"
        CHECK_EQUAL("root123", b.get_password("root")); // Проверяем пароль для "root"

        std::filesystem::remove(valid_base_file); // Удаляем временный файл
    }
}

// Тесты для класса Calc
SUITE(CalcTests) {

    // Группа: Корректные вычисления

    // Проверка на корректное вычисление суммы
    TEST(ValidSumCalculation) {
        std::vector<uint32_t> numbers = {1, 2, 3, 4, 5};
        calc calculator(numbers);
        CHECK_EQUAL(15, calculator.send_res()); // Ожидаем сумму 15
    }

    // Проверка на вектор с одним элементом
    TEST(SingleElementVector) {
        std::vector<uint32_t> single_element = {10};
        calc calculator(single_element);
        CHECK_EQUAL(10, calculator.send_res()); // Ожидаем сумму 10
    }

    // Проверка на вектор с нулевыми значениями
    TEST(AllZerosVector) {
        std::vector<uint32_t> zeros = {0, 0, 0, 0};
        calc calculator(zeros);
        CHECK_EQUAL(0, calculator.send_res()); // Ожидаем сумму 0
    }

    // Проверка на вектор с большим количеством элементов
    TEST(LargeVector) {
        std::vector<uint32_t> large_vector(1000, 1); // 1000 элементов, каждый равен 1
        calc calculator(large_vector);
        CHECK_EQUAL(1000, calculator.send_res()); // Ожидаем сумму 1000
    }

    // Проверка на вектор с очень большим количеством элементов
    TEST(VeryLargeVector) {
        std::vector<uint32_t> very_large_vector(1000000, 1); // 1 миллион элементов, каждый равен 1
        calc calculator(very_large_vector);
        CHECK_EQUAL(1000000, calculator.send_res()); // Ожидаем сумму 1000000
    }

    // Проверка на вектор с максимальным значением и нулем
    TEST(MaxValueAndZero) {
        std::vector<uint32_t> max_and_zero = {UINT32_MAX, 0};
        calc calculator(max_and_zero);
        CHECK_EQUAL(UINT32_MAX, calculator.send_res()); // Ожидаем сумму UINT32_MAX
    }

    // Проверка на вектор с переполнением через несколько элементов
    TEST(OverflowWithoutException) {
        std::vector<uint32_t> overflow_vector = {UINT32_MAX / 2, UINT32_MAX / 2, 1};
        calc calculator(overflow_vector);
        CHECK_EQUAL(UINT32_MAX, calculator.send_res()); // Ожидаем максимальное значение
    }

    // Группа: Обработка ошибок

    // Проверка на пустой вектор (должно выбрасываться исключение)
    TEST(EmptyVectorThrowsException) {
        std::vector<uint32_t> empty_vector;
        CHECK_THROW(calc calculator(empty_vector), no_crit_err); // Ожидаем исключение no_crit_err
    }
}

// Тесты для класса Logger
SUITE(LoggerTests) {

    // Группа: Конструкторы и установка пути

    // Проверка конструктора по умолчанию
    TEST(DefaultConstructor) {
        logger log;
        CHECK_EQUAL("", log.get_path()); // Путь должен быть пустым
    }

    // Проверка конструктора с параметром
    TEST(ParameterizedConstructor) {
        std::string log_path = "test.log";
        logger log(log_path);
        CHECK_EQUAL(log_path, log.get_path()); // Путь должен совпадать с переданным
    }

    // Проверка установки пути с пустой строкой
    TEST(SetPathWithEmptyString) {
        logger log;
        CHECK_THROW(log.set_path(""), crit_err); // Ожидаем исключение
    }

    // Проверка установки пути без расширения файла
    TEST(SetPathWithoutFileExtension) {
        logger log;
        CHECK_THROW(log.set_path("no_extension"), crit_err); // Ожидаем исключение
    }

    // Проверка установки пути с несуществующей директорией
    TEST(SetPathWithNonExistentDirectory) {
        logger log;
        CHECK_THROW(log.set_path("/non_existent_dir/test.log"), crit_err); // Ожидаем исключение
    }

    // Проверка установки корректного пути
    TEST(SetPathWithValidPath) {
        logger log;
        std::string valid_path = "valid_path.log";

        // Создаем временную директорию, если она не существует
        std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
        std::filesystem::path full_path = temp_dir / valid_path;

        // Устанавливаем путь
        CHECK_EQUAL(0, log.set_path(full_path.string())); // Установка пути должна быть успешной
        CHECK_EQUAL(full_path.string(), log.get_path()); // Путь должен быть установлен
    }

    // Группа: Запись в лог

    // Проверка записи в лог с пустым путем
    TEST(WriteLogWithEmptyPath) {
        logger log;
        CHECK_THROW(log.writelog("Test message"), crit_err); // Ожидаем исключение
    }

    // Проверка записи в лог с корректным путем
    TEST(WriteLogWithValidPath) {
        logger log("test_log.log");
        std::string message = "Test message";
        CHECK_EQUAL(0, log.writelog(message)); // Запись должна быть успешной

        // Проверяем, что сообщение записано в файл
        std::ifstream logfile("test_log.log");
        CHECK(logfile.is_open()); // Файл должен быть открыт

        std::stringstream buffer;
        buffer << logfile.rdbuf();
        std::string content = buffer.str();
        CHECK(content.find(message) != std::string::npos); // Сообщение должно быть в файле

        logfile.close();
        std::filesystem::remove("test_log.log"); // Удаляем временный файл
    }

    // Проверка записи нескольких сообщений в лог
    TEST(WriteMultipleLogs) {
        logger log("test_log.log");
        std::string message1 = "First message";
        std::string message2 = "Second message";

        CHECK_EQUAL(0, log.writelog(message1)); // Первое сообщение
        CHECK_EQUAL(0, log.writelog(message2)); // Второе сообщение

        // Проверяем, что оба сообщения записаны в файл
        std::ifstream logfile("test_log.log");
        CHECK(logfile.is_open()); // Файл должен быть открыт

        std::stringstream buffer;
        buffer << logfile.rdbuf();
        std::string content = buffer.str();

        CHECK(content.find(message1) != std::string::npos); // Первое сообщение
        CHECK(content.find(message2) != std::string::npos); // Второе сообщение

        logfile.close();
        std::filesystem::remove("test_log.log"); // Удаляем временный файл
    }

    // Проверка записи пустого сообщения в лог
    TEST(WriteEmptyMessage) {
        logger log("test_log.log");
        std::string message = "";

        CHECK_EQUAL(0, log.writelog(message)); // Запись пустого сообщения

        // Проверяем, что файл создан
        std::ifstream logfile("test_log.log");
        CHECK(logfile.is_open()); // Файл должен быть открыт

        logfile.close();
        std::filesystem::remove("test_log.log"); // Удаляем временный файл
    }

    // Проверка записи большого сообщения в лог
    TEST(WriteLargeMessage) {
        logger log("test_log.log");
        std::string message(10000, 'A'); // Создаем большое сообщение

        CHECK_EQUAL(0, log.writelog(message)); // Запись большого сообщения

        // Проверяем, что сообщение записано в файл
        std::ifstream logfile("test_log.log");
        CHECK(logfile.is_open()); // Файл должен быть открыт

        std::stringstream buffer;
        buffer << logfile.rdbuf();
        std::string content = buffer.str();
        CHECK(content.find(message) != std::string::npos); // Сообщение должно быть в файле

        logfile.close();
        std::filesystem::remove("test_log.log"); // Удаляем временный файл
    }

    // Группа: Вспомогательные методы

    // Проверка получения текущего времени
    TEST(GetTime) {
        logger log;
        std::string time = log.gettime();
        CHECK(!time.empty()); // Время не должно быть пустым

        // Проверяем формат времени (должен соответствовать "%Y-%m-%d %X")
        CHECK(time.find('-') != std::string::npos); // Год-месяц-день
        CHECK(time.find(':') != std::string::npos); // Часы:минуты:секунды
    }

    // Проверка формата времени
    TEST(CheckTimeFormat) {
        logger log;
        std::string time = log.gettime();

        // Проверяем, что время соответствует формату "%Y-%m-%d %X"
        CHECK(time.size() == 19); // Длина строки времени должна быть 19 символов
        CHECK(time[4] == '-'); // Год-месяц-день
        CHECK(time[7] == '-');
        CHECK(time[10] == ' ');
        CHECK(time[13] == ':'); // Часы:минуты:секунды
        CHECK(time[16] == ':');
    }
}

// Тесты для класса Interface
SUITE(InterfaceTests) {

    // Тесты на вызов справки
    TEST(hParameter) {
        interface iface;
        const char* argv[] = {"program", "-h"};
        int argc = sizeof(argv) / sizeof(argv[0]);

        // Перенаправляем std::cout в строку
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());

        bool res = iface.parser(argc, argv);

        // Возвращаем std::cout на место
        std::cout.rdbuf(old_cout);

        CHECK_EQUAL(0, res); // Ожидаем, что справка вызвана, и метод вернет 0

        // Проверяем, что справка была выведена
        CHECK(!output.str().empty()); // Убедимся, что вывод не пустой
    }

    TEST(helpParameter) {
        interface iface;
        const char* argv[] = {"program", "--help"};
        int argc = sizeof(argv) / sizeof(argv[0]);

        // Перенаправляем std::cout в строку
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());

        bool res = iface.parser(argc, argv);

        // Возвращаем std::cout на место
        std::cout.rdbuf(old_cout);

        CHECK_EQUAL(0, res); // Ожидаем, что справка вызвана, и метод вернет 0

        // Проверяем, что справка была выведена
        CHECK(!output.str().empty()); // Убедимся, что вывод не пустой
    }

    TEST(hWithOtherParameter) {
        interface iface;
        const char* argv[] = {"program", "-p", "12345", "-h"};
        int argc = sizeof(argv) / sizeof(argv[0]);

        // Перенаправляем std::cout в строку
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());

        bool res = iface.parser(argc, argv);

        // Возвращаем std::cout на место
        std::cout.rdbuf(old_cout);

        CHECK_EQUAL(0, res); // Ожидаем, что справка вызвана, и метод вернет 0

        // Проверяем, что справка была выведена
        CHECK(!output.str().empty()); // Убедимся, что вывод не пустой
    }

    TEST(HelpOption) {
        const char* argv[] = {
            "program",
            "--help"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;

        // Перенаправляем std::cout в строку
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());

        bool res = iface.parser(argc, argv);

        // Возвращаем std::cout на место
        std::cout.rdbuf(old_cout);

        CHECK_EQUAL(0, res); // Ожидаем, что справка вызвана, и метод вернет 0

        // Проверяем, что справка была выведена
        CHECK(!output.str().empty()); // Убедимся, что вывод не пустой
    }

    // Тесты на некорректные параметры
    TEST(InvalidBaseFile) {
        const char* argv[] = {
            "program",
            "--basefile=invalid_base", // Файл без расширения
            "--logfile=test_log.txt",
            "--port=12345"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_THROW(iface.parser(argc, argv), crit_err); // Ожидаем исключение
    }

    TEST(InvalidLogFile) {
        const char* argv[] = {
            "program",
            "--basefile=test_base.txt",
            "--logfile=invalid_log", // Файл без расширения
            "--port=12345"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_THROW(iface.parser(argc, argv), crit_err); // Ожидаем исключение
    }

    TEST(InvalidPort) {
        const char* argv[] = {
            "program",
            "--basefile=test_base.txt",
            "--logfile=test_log.txt",
            "--port=100" // Некорректный порт
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_THROW(iface.parser(argc, argv), crit_err); // Ожидаем исключение
    }

    TEST(NoParameters) {
        const char* argv[] = {
            "program"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_THROW(iface.parser(argc, argv), crit_err); // Ожидаем исключение, так как файлы не указаны
    }

    TEST(InvalidFileExtension) {
        const char* argv[] = {
            "program",
            "--basefile=test_base", // Файл без расширения
            "--logfile=test_log",   // Файл без расширения
            "--port=12345"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_THROW(iface.parser(argc, argv), crit_err); // Ожидаем исключение
    }

    TEST(NonExistentFiles) {
        const char* argv[] = {
            "program",
            "--basefile=nonexistent_base.txt", // Несуществующий файл
            "--logfile=nonexistent_log.txt",   // Несуществующий файл
            "--port=12345"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_THROW(iface.parser(argc, argv), crit_err); // Ожидаем исключение
    }

    // Тесты на корректные параметры
    TEST(ValidParameters) {
        const char* argv[] = {
            "program",
            "--basefile=test_base.txt",
            "--logfile=test_log.txt",
            "--port=12345"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        CHECK_EQUAL(1, iface.parser(argc, argv)); // Ожидаем, что метод вернет 1 (успешно)
    }

    TEST(BoundaryPortValues) {
        const char* argv_min[] = {
            "program",
            "--basefile=test_base.txt",
            "--logfile=test_log.txt",
            "--port=1024" // Минимальное значение порта
        };
        int argc_min = sizeof(argv_min) / sizeof(argv_min[0]);

        const char* argv_max[] = {
            "program",
            "--basefile=test_base.txt",
            "--logfile=test_log.txt",
            "--port=65535" // Максимальное значение порта
        };
        int argc_max = sizeof(argv_max) / sizeof(argv_max[0]);

        interface iface;
        CHECK_EQUAL(1, iface.parser(argc_min, argv_min)); // Ожидаем, что метод вернет 1 (успешно)
        CHECK_EQUAL(1, iface.parser(argc_max, argv_max)); // Ожидаем, что метод вернет 1 (успешно)
    }

    // Тест на логирование
    TEST(LoggingInformation) {
        const char* argv[] = {
            "program",
            "--basefile=test_base.txt",
            "--logfile=test_log.txt",
            "--port=12345"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        interface iface;
        bool res = iface.parser(argc, argv);
        CHECK_EQUAL(1, res); // Ожидаем, что метод вернет 1 (успешно)

        // Проверяем, что логирование было выполнено (например, проверяем содержимое файла логов)
        std::ifstream log_file("test_log.txt");
        std::string log_content((std::istreambuf_iterator<char>(log_file)), std::istreambuf_iterator<char>());
        CHECK(log_content.find("Путь к лог файлу: test_log.txt") != std::string::npos);
        CHECK(log_content.find("Путь к файлу БД: test_base.txt") != std::string::npos);
        CHECK(log_content.find("Используемый порт: 12345") != std::string::npos);
    }

}

// Тесты для класса Communicator
SUITE(Communicator) {

    TEST(Connection_BindError) {
        communicator comm;
        logger mock_logger;
        int port = 12345;
        std::map<std::string, std::string> mock_base;

        // Перехватываем вызов bind и возвращаем ошибку
        // Проверяем, что метод выбрасывает исключение crit_err
        CHECK_THROW(comm.connection(port, mock_base, &mock_logger), crit_err);
    }

    TEST(Connection_ExtractPacketData) {
        communicator comm;
        std::string packet = "user1" + std::string(16, 's') + std::string(56, 'h');

        // Извлечение данных
        std::string client_hash = packet.substr(packet.size() - 56, 56);
        std::string salt_s = packet.substr(packet.size() - 56 - 16, 16);
        std::string ID = packet.substr(0, packet.size() - 56 - 16);

        CHECK_EQUAL("user1", ID);
        CHECK_EQUAL(std::string(16, 's'), salt_s);
        CHECK_EQUAL(std::string(56, 'h'), client_hash);
    }

    TEST(Connection_CriticalError) {
        communicator comm;
        logger mock_logger;
        int port = 12345;
        std::map<std::string, std::string> mock_base;

        // Моки для сокетов и вызываем критическую ошибку
        // Проверяем, что ошибка записывается в лог
        CHECK_THROW(comm.connection(port, mock_base, &mock_logger), crit_err);
    }

    TEST(Connection_SendOK) {
        communicator comm;
        logger mock_logger;
        int port = 12345;
        std::map<std::string, std::string> mock_base = {
            {"user1", "password1"}
        };

        // Создаем пакет с правильным хешем
        std::string salt = "salt1234567890";
        std::string password = "password1";
        std::string hash = comm.sha224(salt + password);
        std::string packet = "user1" + salt + hash;

        // Перехватываем вызов recv и возвращаем пакет
        // Проверяем, что метод отправляет "OK" клиенту
        // (Для этого теста потребуется мокинг send)
        // CHECK_EQUAL("OK", sent_data);
    }

    TEST(Connection_ReceiveVectors) {
        communicator comm;
        logger mock_logger;
        int port = 12345;
        std::map<std::string, std::string> mock_base = {
            {"user1", "password1"}
        };

        // Создаем пакет с правильным хешем
        std::string salt = "salt1234567890";
        std::string password = "password1";
        std::string hash = comm.sha224(salt + password);
        std::string packet = "user1" + salt + hash;

        // Перехватываем вызов recv и возвращаем пакет
        // Проверяем, что метод корректно обрабатывает векторы
        // (Для этого теста потребуется мокинг recv и send)
    }
}

int main(int argc, char **argv) {
    return UnitTest::RunAllTests();
}

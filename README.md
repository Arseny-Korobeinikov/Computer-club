# Руководство по использованию 
## 1. Настройте своё окружение
### Установите CMake (если он ещё не установлен)
Перейдите на официальный сайт CMake:<br>

[CMake](https://cmake.org/download "Скачать можно тут")


На странице загрузки доступны версии для:<br>
Windows (.msi или .zip)<br>
macOS (.dmg или .tar.gz)<br>
Linux (.sh, .tar.gz или через пакетный менеджер)
## 2. Создайте проект с помощью ```CMake```
1. Создадим директорию в которой сбилдим проект:
```
mkdir build
cd build
```
2. Настройте сборку и соберите проект:
Универсальный вариант для всех ОС (если утановлен Ninja):
```
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
Вариант для Visual Studio:
```
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```
Вариант для Linux:
```
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
После успешной сборки в ```build/``` или в ```build/Release``` (в случае с Visual Studio) появятся исполняемые файлы ```computer_club.exe``` и ```tests.exe```.
## 3. Запуск тестов:
Вариант 1:<br>
В терминале папки ```build``` введите:<br>
```
ctest -C Release
```
Вариант 2:<br>
В терминале папки с испольняемым файлом ```tests.exe``` введите:<br>
```
tests.exe
```
Здесь содержатся юнит-тесты, а также тест, проверяющий корректность программы на разных входных данных (со всех файлов в папке Computer-Club/test/).
## 4. Запуск клиента:
В терминале папки с испольняемым файлом ```computer_club.exe``` введите:<br>
```
computer_club.exe path/to/test/test1.txt
```
Вместо path/to/test/test1.txt нужно ввести абсолютный или относительный путь до файла с входными данными.

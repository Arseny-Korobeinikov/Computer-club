cmake_minimum_required(VERSION 3.14)
project(ComputerClub)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Главный исполняемый файл (если он нужен отдельно)
add_executable(computer_club 
    computer_club.cpp
    sample.cpp
)

# Подключение GoogleTest через FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавляем исполняемый файл для тестов
add_executable(tests
    tests.cpp
    computer_club.cpp
)

# # Включаем папку с .h, если нужно
target_include_directories(tests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

# Линкуем Google Test
target_link_libraries(tests
  PRIVATE
    gtest
    gtest_main
)

# Определяем макрос с путём к тестам
target_compile_definitions(tests PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test")

# Включаем поддержку CTest
enable_testing()
add_test(NAME ComputerClubTests COMMAND tests)

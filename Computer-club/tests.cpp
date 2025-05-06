#include <gtest/gtest.h>
#include "include/computer_club.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

// Предполагается, что основная логика вынесена в эту функцию
int run_computer_club(const std::string &input_file_path, std::ostream &output_stream);

int countFilesInDirectory(const std::filesystem::path &directoryPath)
{
    int fileCount = 0;
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        { // Проверяем, является ли элемент файлом
            fileCount++;
        }
    }
    return fileCount;
}

TEST(ClassTimeTests, MakeFromString)
{
    std::string time_str = "15:30";
    Time a = Time::fromString(time_str);
    ASSERT_EQ(a.getHours(), 15);
    ASSERT_EQ(a.getMinutes(), 30);
}

TEST(ClassTimeTests, TimeToString)
{
    std::string time_str = "15:30";
    Time a = Time::fromString(time_str);
    ASSERT_EQ(a.toString(), "15:30");
}

TEST(ClassTimeTests, TimeToMinutes)
{
    std::string time_str = "15:30";
    Time a = Time::fromString(time_str);
    ASSERT_EQ(a.toMinutes(), 930);
}

TEST(ClassTimeTests, AddTime)
{
    std::string time_str1 = "15:30";
    std::string time_str2 = "2:30";
    Time a = Time::fromString(time_str1);
    Time b = Time::fromString(time_str2);
    Time c = a + b;
    ASSERT_EQ(c.getHours(), 18);
    ASSERT_EQ(c.getMinutes(), 0);
}

TEST(ClassTimeTests, SubtractionTime)
{
    std::string time_str1 = "15:30";
    std::string time_str2 = "2:30";
    Time a = Time::fromString(time_str1);
    Time b = Time::fromString(time_str2);
    Time c = a - b;
    ASSERT_EQ(c.getHours(), 13);
    ASSERT_EQ(c.getMinutes(), 0);
}

TEST(StructEventTests, EventToString)
{
    Event event = {Time::fromString("15:30"), 2, "Bobi Bi", 5, "Bad boy"};
    ASSERT_EQ(event.toString(), "15:30 2 Bobi Bi 5 Bad boy");
}

TEST(ClassComputerClubTests, OneClientCome)
{
    ComputerClub club(10, Time::fromString("9:00"), Time::fromString("19:00"), 450);
    Event event = {Time::fromString("15:30"), 1, "Bobi Bi"};
    club.processEvent(event);

    std::vector<Event> out = club.getOutputEvents();
    Event true_event = {Time::fromString("15:30"), 1, "Bobi Bi"};
    EXPECT_EQ(out[0].toString(), true_event.toString());
    EXPECT_EQ(out.size(), 1);
}

TEST(ClassComputerClubTests, OneClientComeAndSit)
{
    ComputerClub club(10, Time::fromString("9:00"), Time::fromString("19:00"), 450);
    Event event = {Time::fromString("15:30"), 1, "Bobi Bi"};
    club.processEvent(event);

    event = {Time::fromString("15:31"), 2, "Bobi Bi", 5};
    club.processEvent(event);

    std::vector<Event> out = club.getOutputEvents();
    Event true_event = {Time::fromString("15:30"), 1, "Bobi Bi"};
    EXPECT_EQ(out[0].toString(), true_event.toString());

    true_event = {Time::fromString("15:31"), 2, "Bobi Bi", 5};
    EXPECT_EQ(out[1].toString(), true_event.toString());

    EXPECT_EQ(out.size(), 2);
}

TEST(ClassComputerClubTests, OneClientNotComeAndSit)
{
    ComputerClub club(10, Time::fromString("9:00"), Time::fromString("19:00"), 450);
    Event event = {Time::fromString("15:31"), 2, "Bobi Bi"};
    club.processEvent(event);

    std::vector<Event> out = club.getOutputEvents();
    Event error_event = {Time::fromString("15:31"), 13, "", 0, "ClientUnknown"};
    EXPECT_EQ(out[1].toString(), error_event.toString());
}

TEST(ClassComputerClubTests, OneClientComeBeforeOpen)
{
    ComputerClub club(10, Time::fromString("9:00"), Time::fromString("19:00"), 450);
    Event event = {Time::fromString("08:30"), 1, "Bobi Bi", 5};

    club.processEvent(event);

    std::vector<Event> out = club.getOutputEvents();
    Event error_event = {Time::fromString("08:30"), 13, "", 0, "NotOpenYet"};
    EXPECT_EQ(out[1].toString(), error_event.toString());
}

TEST(FileTests, Test1)
{
    std::string path = std::string(TEST_DATA_DIR);
    int count_file = 0;
    try
    {
        count_file = countFilesInDirectory(path);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Ошибка при чтении директории во время теста: " << e.what() << std::endl;
    }

    for (int i = 1; i < (count_file / 2) + 1; ++i)
    {
        std::ostringstream output_stream;
        std::cout << std::string(TEST_DATA_DIR) + "/test" + std::to_string(i) + ".txt" << std::endl
                  << std::endl;
        int result = run_computer_club(std::string(TEST_DATA_DIR) + "/test" + std::to_string(i) + ".txt", output_stream);
        std::ifstream expected_file(std::string(TEST_DATA_DIR) + "/expected" + std::to_string(i) + ".txt");
        std::stringstream expected_stream;
        expected_stream << expected_file.rdbuf();

        ASSERT_EQ(output_stream.str(), expected_stream.str());
    }
}

#include <gtest/gtest.h>

class TestHMIMessage
{
public:
    explicit TestHMIMessage(uint8_t* data) : mData(data){};

    ~TestHMIMessage() = default;

    uint16_t dateYear()
    {
        return 2000 + (mData[19] / 2);
    }
    void setDateMonthAndYear(uint8_t month, uint16_t year)
    {
        int pastJuly   = month > 7 ? 1 : 0;
        mData[19]      = ((year - 2000) * 2) + pastJuly;
        int monthValue = (pastJuly ? month - 8 : month) << 5;
        mData[18]      = (mData[18] & 0x1F) | (monthValue & 0xE0);
    }
    uint8_t dateMonth()
    {
        return (mData[18] >> 5) + ((mData[19] % 2) * 8);
    }
    uint8_t dateDay()
    {
        return mData[18] & 0x1F;
    }
    void setDateDay(uint8_t day)
    {
        mData[18] = (mData[18] & 0xE0) | (day & 0x1F);
    }

private:
    uint8_t* mData;
};

TEST(DateAndTimeTestSuite, TestDate)
{
    uint8_t mTransferBuffer[40];
    TestHMIMessage message = TestHMIMessage(mTransferBuffer);
    for (int year = 2000; year <= 2025; ++year) {
        for (int month = 1; month <= 12; ++month) {
            int daysInMonth;
            if (month == 2) {
                daysInMonth = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
            } else if (month == 4 || month == 6 || month == 9 || month == 11) {
                daysInMonth = 30;
            } else {
                daysInMonth = 31;
            }
            for (int day = 1; day <= daysInMonth; ++day) {
                message.setDateDay(day);
                message.setDateMonthAndYear(month, year);

                ASSERT_EQ(message.dateDay(), day);
                ASSERT_EQ(message.dateMonth(), month);
                ASSERT_EQ(message.dateYear(), year);
            }
        }
    }
}
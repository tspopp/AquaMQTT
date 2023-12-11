#include <gtest/gtest.h>

#include <ostream>

static float parseSignedFloat(const unsigned char* bytes)
{
    return (float) (((short int) (bytes[1] << 8) | bytes[0]) / 10.0);
}

static void writeSignedFloat(const float floatValue, unsigned char* bytes)
{
    short int rawValue = floatValue * 100 / 10;
    bytes[1]           = (rawValue >> 8) & 0xFF;
    bytes[0]           = rawValue & 0xFF;
}

TEST(DHWDataTypeTestSuite, ParseFloatValues)
{
    unsigned char bytes1[] = { 0x00, 0x00 };  // 0.0
    unsigned char bytes2[] = { 0x01, 0x00 };  // 0.1
    unsigned char bytes3[] = { 0xC2, 0x01 };  // 45.0
    unsigned char bytes4[] = { 0xFF, 0xFF };  // -0.1
    unsigned char bytes5[] = { 0xFE, 0xFF };  // -0.2

    ASSERT_EQ(parseSignedFloat(bytes1), 0.0f);
    ASSERT_EQ(parseSignedFloat(bytes2), 0.1f);
    ASSERT_EQ(parseSignedFloat(bytes3), 45.0f);
    ASSERT_EQ(parseSignedFloat(bytes4), -0.1f);
    ASSERT_EQ(parseSignedFloat(bytes5), -0.2f);
}

TEST(DHWDataTypeTestSuite, WriteFloatValues)
{
    unsigned char result[2] = { 0 };

    unsigned char bytes1[] = { 0x00, 0x00 };  // 0.0
    unsigned char bytes2[] = { 0x01, 0x00 };  // 0.1
    unsigned char bytes3[] = { 0xC2, 0x01 };  // 45.0
    unsigned char bytes4[] = { 0xC3, 0x01 };  // 45.1
    unsigned char bytes5[] = { 0xC4, 0x01 };  // 45.2
    unsigned char bytes6[] = { 0xC5, 0x01 };  // 45.3
    unsigned char bytes7[] = { 0xFF, 0xFF };  // -0.1
    unsigned char bytes8[] = { 0xFE, 0xFF };  // -0.2

    writeSignedFloat(0.0f, result);
    ASSERT_EQ(result[0], bytes1[0]);
    ASSERT_EQ(result[1], bytes1[1]);

    writeSignedFloat(0.1f, result);
    ASSERT_EQ(result[0], bytes2[0]);
    ASSERT_EQ(result[1], bytes2[1]);

    writeSignedFloat(45.0f, result);
    ASSERT_EQ(result[0], bytes3[0]);
    ASSERT_EQ(result[1], bytes3[1]);

    writeSignedFloat(45.1f, result);
    ASSERT_EQ(result[0], bytes4[0]);
    ASSERT_EQ(result[1], bytes4[1]);

    writeSignedFloat(45.2f, result);
    ASSERT_EQ(result[0], bytes5[0]);
    ASSERT_EQ(result[1], bytes5[1]);

    writeSignedFloat(45.3f, result);
    ASSERT_EQ(result[0], bytes6[0]);
    ASSERT_EQ(result[1], bytes6[1]);

    writeSignedFloat(-0.1f, result);
    ASSERT_EQ(result[0], bytes7[0]);
    ASSERT_EQ(result[1], bytes7[1]);

    writeSignedFloat(-0.2f, result);
    ASSERT_EQ(result[0], bytes8[0]);
    ASSERT_EQ(result[1], bytes8[1]);
}
/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LibHidlTest"

#include <android-base/logging.h>
#include <gtest/gtest.h>
#include <hidl/HidlSupport.h>
#include <hidl/TaskRunner.h>
#include <vector>

#define EXPECT_ARRAYEQ(__a1__, __a2__, __size__) EXPECT_TRUE(isArrayEqual(__a1__, __a2__, __size__))
#define EXPECT_2DARRAYEQ(__a1__, __a2__, __size1__, __size2__) \
        EXPECT_TRUE(is2dArrayEqual(__a1__, __a2__, __size1__, __size2__))

template<typename T, typename S>
static inline bool isArrayEqual(const T arr1, const S arr2, size_t size) {
    for(size_t i = 0; i < size; i++)
        if(arr1[i] != arr2[i])
            return false;
    return true;
}

template<typename T, typename S>
static inline bool is2dArrayEqual(const T arr1, const S arr2, size_t size1, size_t size2) {
    for(size_t i = 0; i < size1; i++)
        for (size_t j = 0; j < size2; j++)
            if(arr1[i][j] != arr2[i][j])
                return false;
    return true;
}

class LibHidlTest : public ::testing::Test {
public:
    virtual void SetUp() override {
    }
    virtual void TearDown() override {
    }
};

TEST_F(LibHidlTest, StringTest) {
    using android::hardware::hidl_string;
    hidl_string s; // empty constructor
    EXPECT_STREQ(s.c_str(), "");
    hidl_string s1 = "s1"; // copy = from cstr
    EXPECT_STREQ(s1.c_str(), "s1");
    hidl_string s2("s2"); // copy constructor from cstr
    EXPECT_STREQ(s2.c_str(), "s2");
    hidl_string s3 = hidl_string("s3"); // move =
    EXPECT_STREQ(s3.c_str(), "s3");
    hidl_string s4(hidl_string(hidl_string("s4"))); // move constructor
    EXPECT_STREQ(s4.c_str(), "s4");
    hidl_string s5(std::string("s5")); // copy constructor from std::string
    EXPECT_STREQ(s5, "s5");
    hidl_string s6 = std::string("s6"); // copy = from std::string
    EXPECT_STREQ(s6, "s6");
    hidl_string s7(s6); // copy constructor
    EXPECT_STREQ(s7, "s6");
    hidl_string s8 = s7; // copy =
    EXPECT_STREQ(s8, "s6");
    char myCString[20] = "myCString";
    s.setToExternal(&myCString[0], strlen(myCString));
    EXPECT_STREQ(s, "myCString");
    myCString[2] = 'D';
    EXPECT_STREQ(s, "myDString");
    s.clear(); // should not affect myCString
    EXPECT_STREQ(myCString, "myDString");

    // casts
    s = "great";
    std::string myString = s;
    const char *anotherCString = s;
    EXPECT_EQ(myString, "great");
    EXPECT_STREQ(anotherCString, "great");

    // Comparisons
    const char * cstr1 = "abc";
    std::string string1(cstr1);
    hidl_string hs1(cstr1);
    const char * cstrE = "abc";
    std::string stringE(cstrE);
    hidl_string hsE(cstrE);
    const char * cstrNE = "ABC";
    std::string stringNE(cstrNE);
    hidl_string hsNE(cstrNE);
    EXPECT_TRUE(hs1  == hsE);
    EXPECT_FALSE(hs1 != hsE);
    EXPECT_TRUE(hs1  != hsNE);
    EXPECT_FALSE(hs1 == hsNE);
    EXPECT_TRUE(hs1  == cstrE);
    EXPECT_FALSE(hs1 != cstrE);
    EXPECT_TRUE(hs1  != cstrNE);
    EXPECT_FALSE(hs1 == cstrNE);
    EXPECT_TRUE(hs1  == stringE);
    EXPECT_FALSE(hs1 != stringE);
    EXPECT_TRUE(hs1  != stringNE);
    EXPECT_FALSE(hs1 == stringNE);
}

TEST_F(LibHidlTest, VecInitTest) {
    using android::hardware::hidl_vec;
    using std::vector;
    int32_t array[] = {5, 6, 7};
    vector<int32_t> v(array, array + 3);

    hidl_vec<int32_t> hv1 = v; // copy =
    EXPECT_ARRAYEQ(hv1, array, 3);
    EXPECT_ARRAYEQ(hv1, v, 3);
    hidl_vec<int32_t> hv2(v); // copy constructor
    EXPECT_ARRAYEQ(hv2, v, 3);

    vector<int32_t> v2 = hv1; // cast
    EXPECT_ARRAYEQ(v2, v, 3);

    hidl_vec<int32_t> v3 = {5, 6, 7}; // initializer_list
    EXPECT_EQ(v3.size(), 3ul);
    EXPECT_ARRAYEQ(v3, array, v3.size());
}

TEST_F(LibHidlTest, VecIterTest) {
    int32_t array[] = {5, 6, 7};
    android::hardware::hidl_vec<int32_t> hv1 = std::vector<int32_t>(array, array + 3);

    auto iter = hv1.begin();    // iterator begin()
    EXPECT_EQ(*iter++, 5);
    EXPECT_EQ(*iter, 6);
    EXPECT_EQ(*++iter, 7);
    EXPECT_EQ(*iter--, 7);
    EXPECT_EQ(*iter, 6);
    EXPECT_EQ(*--iter, 5);

    iter += 2;
    EXPECT_EQ(*iter, 7);
    iter -= 2;
    EXPECT_EQ(*iter, 5);

    iter++;
    EXPECT_EQ(*(iter + 1), 7);
    EXPECT_EQ(*(1 + iter), 7);
    EXPECT_EQ(*(iter - 1), 5);
    EXPECT_EQ(*iter, 6);

    auto five = iter - 1;
    auto seven = iter + 1;
    EXPECT_EQ(seven - five, 2);
    EXPECT_EQ(five - seven, -2);

    EXPECT_LT(five, seven);
    EXPECT_LE(five, seven);
    EXPECT_GT(seven, five);
    EXPECT_GE(seven, five);

    EXPECT_EQ(seven[0], 7);
    EXPECT_EQ(five[1], 6);
}

TEST_F(LibHidlTest, VecIterForTest) {
    using android::hardware::hidl_vec;
    int32_t array[] = {5, 6, 7};
    hidl_vec<int32_t> hv1 = std::vector<int32_t>(array, array + 3);

    int32_t sum = 0;            // range based for loop interoperability
    for (auto &&i: hv1) {
        sum += i;
    }
    EXPECT_EQ(sum, 5+6+7);

    for (auto iter = hv1.begin(); iter < hv1.end(); ++iter) {
        *iter += 10;
    }
    const hidl_vec<int32_t> &v4 = hv1;
    sum = 0;
    for (const auto &i : v4) {
        sum += i;
    }
    EXPECT_EQ(sum, 15+16+17);
}

TEST_F(LibHidlTest, VecEqTest) {
    android::hardware::hidl_vec<int32_t> hv1{5, 6, 7};
    android::hardware::hidl_vec<int32_t> hv2{5, 6, 7};
    android::hardware::hidl_vec<int32_t> hv3{5, 6, 8};

    // use the == and != operator intentionally here
    EXPECT_TRUE(hv1 == hv2);
    EXPECT_TRUE(hv1 != hv3);
}

TEST_F(LibHidlTest, ArrayTest) {
    using android::hardware::hidl_array;
    int32_t array[] = {5, 6, 7};

    hidl_array<int32_t, 3> ha(array);
    EXPECT_ARRAYEQ(ha, array, 3);
}

TEST_F(LibHidlTest, TaskRunnerTest) {
    using android::hardware::TaskRunner;
    TaskRunner tr;
    bool flag = false;
    tr.push([&] {
        usleep(1000);
        flag = true;
    });
    usleep(500);
    EXPECT_FALSE(flag);
    usleep(1000);
    EXPECT_TRUE(flag);
}

TEST_F(LibHidlTest, StringCmpTest) {
    using android::hardware::hidl_string;
    const char * s = "good";
    hidl_string hs(s);
    EXPECT_NE(hs.c_str(), s);

    EXPECT_TRUE(hs == s); // operator ==
    EXPECT_TRUE(s == hs);

    EXPECT_FALSE(hs != s); // operator ==
    EXPECT_FALSE(s != hs);
}

template <typename T>
void great(android::hardware::hidl_vec<T>) {}

TEST_F(LibHidlTest, VecCopyTest) {
    android::hardware::hidl_vec<int32_t> v;
    great(v);
}

TEST_F(LibHidlTest, StdArrayTest) {
    using android::hardware::hidl_array;
    hidl_array<int32_t, 5> array{(int32_t[5]){1, 2, 3, 4, 5}};
    std::array<int32_t, 5> stdArray = array;
    EXPECT_ARRAYEQ(array.data(), stdArray.data(), 5);
    hidl_array<int32_t, 5> array2 = stdArray;
    EXPECT_ARRAYEQ(array.data(), array2.data(), 5);
}

TEST_F(LibHidlTest, MultiDimStdArrayTest) {
    using android::hardware::hidl_array;
    hidl_array<int32_t, 2, 3> array;
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 3; j++) {
            array[i][j] = i + j + i * j;
        }
    }
    std::array<std::array<int32_t, 3>, 2> stdArray = array;
    EXPECT_2DARRAYEQ(array, stdArray, 2, 3);
    hidl_array<int32_t, 2, 3> array2 = stdArray;
    EXPECT_2DARRAYEQ(array, array2, 2, 3);
}

TEST_F(LibHidlTest, HidlVersionTest) {
    using android::hardware::hidl_version;
    hidl_version v1_0{1, 0};
    EXPECT_EQ(1, v1_0.get_major());
    EXPECT_EQ(0, v1_0.get_minor());
    hidl_version v2_0{2, 0};
    hidl_version v2_1{2, 1};
    hidl_version v2_2{2, 2};
    hidl_version v3_0{3, 0};
    hidl_version v3_0b{3,0};

    EXPECT_TRUE(v1_0 < v2_0);
    EXPECT_TRUE(v2_0 < v2_1);
    EXPECT_TRUE(v2_1 < v3_0);
    EXPECT_TRUE(v2_0 > v1_0);
    EXPECT_TRUE(v2_1 > v2_0);
    EXPECT_TRUE(v3_0 > v2_1);
    EXPECT_TRUE(v3_0 == v3_0b);
    EXPECT_TRUE(v3_0 <= v3_0b);
    EXPECT_TRUE(v2_2 <= v3_0);
    EXPECT_TRUE(v3_0 >= v3_0b);
    EXPECT_TRUE(v3_0 >= v2_2);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#include <string>
#include <vector>
#include <iostream>

// split utf8 string into words
// Chinese: single character is a word
// English: word separate by space
// other symbols: single character is a word
std::vector<std::string> get_utf8_words(const std::string& str) {
    std::vector<std::string> words;
    int i = 0;
    while (i < str.length()) {
        if ((str[i] & (1 << 7)) == 0) { // 0xxx xxxx
            std::string tmp;
            if ((str[i] >= 'a' && str[i] <= 'z') ||
                (str[i] >= 'A' && str[i] <= 'Z') || str[i] == '\'') {
                while (i < str.length() && (str[i] & (1 << 8)) == 0 &&
                       ((str[i] >= 'a' && str[i] <= 'z') ||
                        (str[i] >= 'A' && str[i] <= 'Z') || str[i] == '\'')) {
                    tmp += str[i];
                    i++;
                }
            } else {
                tmp = str.substr(i, 1);
                i++;
            }
            words.emplace_back(tmp);
        } else if ((str[i] & 0xe0) == 0xc0) { // 110x xxxx, 10xx xxxx
            words.emplace_back(str.substr(i, 2));
            i += 2;
        } else if ((str[i] & 0xf0) == 0xe0) { // 1110 xxxx, 10xx xxxx, 10xx xxxx
            words.emplace_back(str.substr(i, 3));
            i += 3;
        } else { // 1111 0xxx, 10xx xxxx, 10xx xxxx, 10xx xxxx
            words.emplace_back(str.substr(i, 4));
            i += 4;
        }
    }
    return words;
}

// full-width character to half-width
int full2half(const std::string& full_str, std::string& half_str) {

    for (size_t i = 0; i < full_str.size(); i++) {
        if (((full_str[i] & 0xF0) ^ 0xE0) == 0) {
            int old_char = (full_str[i] & 0xF) << 12 |
                           ((full_str[i + 1] & 0x3F) << 6 | (full_str[i + 2] & 0x3F));

            if (old_char == 0x3000) { // blank
                char new_char = 0x20;
                half_str += new_char;
            } else if (old_char == 0x3002) { // 。
                char new_char = 0x2E;
                half_str += new_char;
            } else if (old_char >= 0xFF01 && old_char <= 0xFF5E) { // full char
                char new_char = old_char - 0xFEE0;
                half_str += new_char;
            } else { // other 3 bytes char
                half_str += full_str[i];
                half_str += full_str[i + 1];
                half_str += full_str[i + 2];
            }

            i = i + 2;
        } else {
            half_str += full_str[i];
        }
    }
    return 0;
}

int half2full(const std::string& half_str, std::string& full_str) {
    for (size_t i = 0; i < half_str.size(); i++) {
        unsigned char ch = half_str[i];

        if (ch == 0x20) { // 半角空格 → 全角空格
            full_str += static_cast<char>(0xE3);
            full_str += static_cast<char>(0x80);
            full_str += static_cast<char>(0x80);
        } else if (ch >= 0x21 && ch <= 0x7E) { // 半角字符 → 全角字符
            int full_char = ch + 0xFEE0;
            full_str += static_cast<char>(0xEF);
            full_str += static_cast<char>(0xBC | ((full_char >> 6) & 0x03));
            full_str += static_cast<char>(0x80 | (full_char & 0x3F));
        } else { // 其他字符保持不变
            full_str += ch;
        }
    }
    return 0;
}

/******************** test functions ********************/

void test_get_utf8_words() {
    std::string str = "hello, 你好，世界！";
    auto words = get_utf8_words(str);
    for (const auto& word : words) {
        std::cout << word << std::endl;
    }
}

void test_full_to_half() {
    std::string full_str = "ｈｅｌｌｏ，　你好，世界！";
    // 13 full-width characters
    // std::cout << strlen(str.c_str()) << std::endl; // 39
    std::string half_str;
    full2half(full_str,half_str );
    std::cout << half_str << std::endl;
    std::string full_str1;
    half2full(half_str, full_str1);
    std::cout<<full_str1<<std::endl;
}

int main() {
    // test_get_utf8_words();
    test_full_to_half();
    return 0;
}
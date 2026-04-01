#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>

/**
 * @brief 尝试将字符串解析为整数
 * @param s 输入字符串
 * @param result 解析结果
 * @return 解析是否成功
 */
inline bool tryParseInt(const std::string& s, int& result) {
    try {
        if (s.empty()) return false;
        result = std::stoi(s);
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief 将字符串转换为小写
 * @param s 输入字符串
 * @return 小写字符串
 */
inline std::string toLower(const std::string& s) {
    std::string result = s;
    for (char& c : result) {
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
    }
    return result;
}

/**
 * @brief 分割字符串
 * @param s 输入字符串
 * @param delimiter 分隔符
 * @return 分割后的字符串数组
 */
inline std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

#endif // UTILS_H

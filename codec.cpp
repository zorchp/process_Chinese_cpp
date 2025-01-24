#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <iconv.h>

std::string convertEncoding(const std::string &input, const char *fromEncoding,
                            const char *toEncoding) {
    iconv_t cd = iconv_open(toEncoding, fromEncoding);
    if (cd == (iconv_t)-1) {
        throw std::runtime_error("Failed to open iconv");
    }

    size_t inBytesLeft = input.size();
    size_t outBytesLeft =
        inBytesLeft * 4; // Allocate enough space for the output
    char *inBuf = const_cast<char *>(input.c_str());
    std::string output(outBytesLeft, '\0');
    char *outBuf = &output[0];

    if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
        iconv_close(cd);
        throw std::runtime_error("Encoding conversion failed");
    }

    iconv_close(cd);
    output.resize(output.size() - outBytesLeft); // Adjust output size
    return output;
}

int utf8_to_gbk(const std::string &source, std::string &target);
int gbk_to_utf8(const std::string &source, std::string &target);

// return str contains chinese or not, str must be gbk-encoded

int charconv(const char *from, const char *to, const char *input, int inlen,
             char **output, int *outlen) {
    char *inbuf = NULL;
    char *outbuf = NULL;
    iconv_t cd;
    size_t result = 0;
    cd = iconv_open(to, from);
    if (cd == (iconv_t)(-1)) {
        *outlen = -1;
        *output = NULL;
        return -1;
    }
    if (inlen == 0) {
        inlen = strlen(input);
    }
    *outlen = 4 * inlen;
    inbuf = (char *)input;
    outbuf = (char *)calloc(1, *outlen + 1);
    size_t inleft = inlen;
    size_t outleft = *outlen;
    *output = outbuf;
#if (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 2)
    result = iconv(cd, &inbuf, &inleft, &outbuf, &outleft);
#else
    result = iconv(cd, (char **)&inbuf, &inleft, &outbuf, &outleft);
#endif
    iconv_close(cd);
    *outlen = *outlen - outleft;
    (*output)[*outlen] = 0;
    return inlen - inleft;
}

int utf8_to_gbk(const std::string &source, std::string &target) {
    char *output = NULL;
    int outlen = 0;
    int ret = 0;
    if (source.size()) {
        ret = charconv("UTF-8", "GBK", source.c_str(), source.size(), &output,
                       &outlen);
        if (ret < 0) {
            return -1;
        }
        target = output;
        free(output);
    }
    return 0;
}

int gbk_to_utf8(const std::string &source, std::string &target) {
    char *output = NULL;
    int outlen = 0;
    int ret = 0;
    if (source.size()) {
        ret = charconv("GBK", "UTF-8", source.c_str(), source.size(), &output,
                       &outlen);
        if (ret < 0) {
            return -1;
        }
        target = output;
        free(output);
    }
    return 0;
}

void t1() {
    try {
        std::string utf8Str = reinterpret_cast<const char *>(u8"你好，世界！");
        std::string gbkStr = convertEncoding(utf8Str, "UTF-8", "GBK");
        std::cout << "Converted to GBK: " << gbkStr << std::endl;

        std::string backToUtf8 = convertEncoding(gbkStr, "GBK", "UTF-8");
        std::cout << "Converted back to UTF-8: " << backToUtf8 << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void t2() {
    try {
        std::string utf8Str = reinterpret_cast<const char *>(u8"你好，世界！");
        std::string gbkStr;
        utf8_to_gbk(utf8Str, gbkStr);
        std::cout << "Converted to GBK: " << gbkStr << std::endl;

        std::string backToUtf8;
        gbk_to_utf8(gbkStr, backToUtf8);
        std::cout << "Converted back to UTF-8: " << backToUtf8 << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char const *argv[]) {
    // t1();
    t2();
    return 0;
}

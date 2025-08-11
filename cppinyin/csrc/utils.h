#ifndef CPPINYIN_CSRC_UTILS_H_
#define CPPINYIN_CSRC_UTILS_H_

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

namespace cppinyin {

#define CPY_ASSERT(cond, msg)                                                  \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::cerr << "Assertion failed: " << #cond << "\n"                       \
                << "Message: " << msg << "\n"                                  \
                << "File: " << __FILE__ << ", Line: " << __LINE__              \
                << std::endl;                                                  \
      std::abort();                                                            \
    }                                                                          \
  } while (0)

constexpr auto HEADER = "__kcppinyinw__";

size_t ReadUint32(std::istream &ifile, uint32_t *data);

size_t WriteUint32(std::ofstream &ofile, uint32_t data);

size_t ReadFloat(std::istream &ifile, float *data);

size_t WriteFloat(std::ofstream &ofile, float data);

size_t ReadString(std::istream &ifile, std::string *data);

size_t WriteString(std::ofstream &ofile, const std::string &data);

size_t ReadHeader(std::istream &ifile, std::string *data);

size_t WriteHeader(std::ofstream &ofile);

std::string RemoveNumberTone(const std::string &s);

} // namespace cppinyin

#endif // CPPINYIN_CSRC_UTILS_H_

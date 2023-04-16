#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <rapidjson/document.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

#define FATAL(...) do { fprintf(stderr, __VA_ARGS__); return -1;} while(0)

int mmap_file(const char* file, char** buf, size_t& buf_len) {
  struct stat fstat;
  if (stat(file, &fstat) != 0) {
    FATAL("error performing stat on file: %s. Reason: %s\n", file, strerror(errno));
  }
  if(!(fstat.st_mode & S_IFREG)) {
    FATAL("error %s is not a regular file\n", file);
  }
  buf_len = fstat.st_size;
  int fd = open(file, O_RDONLY);
  if (fd == -1) {
    FATAL("error opening file: %s. Reason: %s\n", file, strerror(errno));
  }
  void* mptr = mmap(nullptr, buf_len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (mptr == MAP_FAILED) {
    // Handle error mapping file
    close(fd);
    FATAL("error mmap-ing file: %s. Reason: %s\n", file, strerror(errno));
  }
  if (madvise(mptr, buf_len, MADV_SEQUENTIAL) != 0) {
    close(fd);
    munmap(buf, buf_len);
    FATAL("error performing madvise for file: %s. Reason: %s\n", file, strerror(errno));
  }
  close(fd);
  *buf = static_cast<char*>(mptr);
  return 0;
}

struct jshandler {

  jshandler(void):
    balance_flag(false),
    num_cust(0.0),
    avg_balance(0.0)
  {}

  bool Null() { return true; }
  bool Bool(bool b) { return true; }
  bool Int64(int64_t i) { return true; }
  bool Uint64(uint64_t u) { return true; }
  bool Double(double d) { return true; }
  bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) { return true; }
  bool String(const char* str, rapidjson::SizeType length, bool copy) { return true; }
  bool StartObject() { return true; }
  bool EndObject(rapidjson::SizeType memberCount) { return true; }
  bool StartArray() { return true; }
  bool EndArray(rapidjson::SizeType elementCount) { return true; }

  // mapping key balance -> avg calculation
  bool Key(const char* str, rapidjson::SizeType length, bool copy) {
    if (std::string(str, length) == "balance") {
      balance_flag = true;
    }
    return true;
  }
  bool Int(int i) { avg_more((int)i); return true; }
  bool Uint(unsigned u) { avg_more((int)u); return true; }

  void avg_more(int val) {
    ++num_cust;
    double balance = (double)(val);
    avg_balance = avg_balance + ((balance - avg_balance) / num_cust);
    balance_flag = false;
  }

  // next value is balance
  bool balance_flag;
  double num_cust;
  double avg_balance;

};

int main(int argc, char **argv) {
  char* buf = nullptr;
  size_t buf_len = 0;
  if (mmap_file(argv[1], &buf, buf_len) != 0) {
    return -1;
  }
  jshandler handler;
  rapidjson::Reader reader;
  rapidjson::StringStream ss(buf);
  rapidjson::ParseResult ok = reader.Parse(ss, handler);
  if (!ok) {
    FATAL("error parsing json. Reason[%d]: %s\n", (int)ok.Offset(), rapidjson::GetParseError_En(ok.Code()));
  }  
  printf("average balance: %.2f\n", handler.avg_balance);
  munmap(buf, buf_len);
  return 0;
}
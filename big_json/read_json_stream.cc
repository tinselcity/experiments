#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define USE_RAPID_JSON
//#define USE_SIMDJSON
#ifdef USE_RAPID_JSON
#if defined(__SSE4_2__)
#  define RAPIDJSON_SSE42
#elif defined(__SSE2__)
#  define RAPIDJSON_SSE2
#elif defined(__ARM_NEON)
#  define RAPIDJSON_NEON
#endif

#include <rapidjson/document.h>
#include "rapidjson/filereadstream.h"
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>
#endif
#ifdef USE_SIMDJSON
#include <simdjson.h>
#endif

#define FATAL(...) do { fprintf(stderr, __VA_ARGS__); return -1;} while(0)

#ifdef USE_RAPID_JSON
int test(const rapidjson::Document& js) {
  return 0;
}
#endif

int read_file(const char* file, char** buf, size_t& buf_len) {
  struct stat fstat;
  if (stat(file, &fstat) != 0) {
    FATAL("error performing stat on file: %s. Reason: %s\n", file, strerror(errno));
  }
  if(!(fstat.st_mode & S_IFREG)) {
    FATAL("error %s is not a regular file\n", file);
  }
  buf_len = fstat.st_size;
#ifdef USE_SIMDJSON
  *buf = (char*)malloc(buf_len + simdjson::SIMDJSON_PADDING);
#endif
#ifdef USE_RAPID_JSON
  *buf = (char*)malloc(buf_len);
#endif
  FILE* fp = fopen(file, "r");
  if (fp == NULL) {
    FATAL("error opening file: %s. Reason: %s\n", file, strerror(errno));
  }
  size_t read = fread(*buf, 1, (size_t)fstat.st_size, fp);
  if (read != (size_t)fstat.st_size) {
    FATAL("read[%lu] < requested[%lu]\n", read, buf_len);
  }
  fclose(fp);
  return 0;
}

int main(int argc, char **argv) {
  // use madvise sequential
#if 1
  char* buf = nullptr;
  size_t buf_len = 0;
  if (read_file(argv[1], &buf, buf_len) != 0) {
    return -1;
  }
  printf("done reading -waiting for input[p: %p, size: %lu]:\n", buf, buf_len);
  //getchar();
#endif
#ifdef USE_RAPID_JSON
#if 1
  rapidjson::Document js = rapidjson::Document();
  //rapidjson::ParseResult ok = js.ParseInsitu(buf);
  rapidjson::ParseResult ok = js.Parse(buf, buf_len);
  if (!ok) {
    FATAL("error parsing json. Reason[%d]: %s\n", (int)ok.Offset(), rapidjson::GetParseError_En(ok.Code()));
  }
#endif
#if 0
  FILE* fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    FATAL("error opening file: %s. Reason: %s\n", argv[1], strerror(errno));
  }
  char rbuf[65536];
  rapidjson::FileReadStream is(fp, rbuf, sizeof(rbuf));
  // Parse the JSON document
  rapidjson::Document js;
  rapidjson::ParseResult ok = js.ParseStream(is);
  if (!ok) {
    FATAL("error parsing json. Reason[%d]: %s\n", (int)ok.Offset(), rapidjson::GetParseError_En(ok.Code()));
  }
#endif
#endif
#ifdef USE_SIMDJSON
  printf("before read -waiting for input:\n");
  getchar();
  simdjson::ondemand::parser parser;
  //simdjson::padded_string json = simdjson::padded_string::load(argv[1]);
  simdjson::padded_string_view json(buf, buf_len, buf_len+simdjson::SIMDJSON_PADDING);
  printf("done reading -waiting for input:\n");
  getchar();
  auto doc = parser.iterate(json);
  auto varray = doc.find_field("vals").get_array();
 // requires simdjson 1.0 or better
  size_t count = varray.count_elements();
  printf("count: %lu\n", count);
#endif
  printf("done parsing -waiting for input:\n");
  getchar();
  printf("done...\n");
#if 1
  free(buf);
#endif
  return 0;
}

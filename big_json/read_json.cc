#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <rapidjson/document.h>
#include "rapidjson/filereadstream.h"
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

#define FATAL(...) do { fprintf(stderr, __VA_ARGS__); return -1;} while(0)

int main(int argc, char **argv) {
  FILE* fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    FATAL("error opening file: %s. Reason: %s\n", argv[1], strerror(errno));
  }
  char rbuf[65536];
  rapidjson::FileReadStream is(fp, rbuf, sizeof(rbuf));
  rapidjson::Document js;
  rapidjson::ParseResult ok = js.ParseStream(is);
  if (!ok) {
    FATAL("error parsing json. Reason[%d]: %s\n", (int)ok.Offset(), rapidjson::GetParseError_En(ok.Code()));
  }
  double num_cust = 0;
  double avg_balance = 0.0;
  for (auto && obj : js.GetObject()){
    if (std::string(obj.name.GetString()) != "customers") { continue;}
    for (auto && cust : obj.value.GetArray()) {
      for (auto && prop : cust.GetObject()) {
        if (std::string(prop.name.GetString()) == "balance") {
          ++num_cust;
          double balance = (double)(prop.value.GetInt());
          avg_balance = avg_balance + ((balance - avg_balance) / num_cust);
        }
      }
    }
  }
  fclose(fp);
  printf("average balance: %.2f\n", avg_balance);
  return 0;
}

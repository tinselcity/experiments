//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <string.h>
#include <stdlib.h>
#include "ndebug.h"
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
typedef struct mime {
    const char* ext;
    const char* type;
} mime_t;
static mime_t s_mime_dict[] = {
#include "_gen_mime_types.h"
};
//! ----------------------------------------------------------------------------
//! \details ext compare
//! \return  relative ordering between strings -used for bsearch
//! \param   s1 extension
//! \param   s2 extension
//! ----------------------------------------------------------------------------
static int _ext_compare(const void *s1, const void *s2) {
  const mime_t* e1 = s1;
  const mime_t* e2 = s2;
  return strcmp(e1->ext, e2->ext);
}
//! ----------------------------------------------------------------------------
//! \details file_ext
//! \return  file extension string or "" if none found
//! \param   str path/file to get ext part for
//! \note    https://stackoverflow.com/a/5309508
//! ----------------------------------------------------------------------------
static const char* _file_ext(const char *buf, size_t len) {
  const char* dot = (const char*)memrchr(buf, '.', len);
  if(!dot ||
     dot == buf) {
      return "";
  }
  return dot + 1;
}
//! ----------------------------------------------------------------------------
//! \details mime_type
//! \return  content type per mime or "" if none found for extension
//! \param   str path/file to get ext part for
//! \note    https://stackoverflow.com/a/5309508
//! ----------------------------------------------------------------------------
const char* mime_type(const char* buf, const size_t len) {
  // -------------------------------------------------------
  // get extension
  // -------------------------------------------------------
  const char* ext = _file_ext(buf, len);
  // -------------------------------------------------------
  // perform bsearch over sorted list of extensions
  // -------------------------------------------------------
  mime_t key = {
    .ext = ext,
    .type = ""
  };
  void* ret = bsearch(&key,
                      s_mime_dict,
                      ARRAY_SIZE(s_mime_dict),
                      sizeof(mime_t),
                      _ext_compare);
  if (!ret) {
    return "";
  }
  return ((mime_t*)(ret))->type;
}

#include <stddef.h>

void *
memchr (register const void *src_void, int c, size_t length)
{
  const unsigned char *src = (const unsigned char *)src_void;

  while (length-- > 0)
  {
    if (*src == c)
     return (void *)src;
    src++;
  }
  return NULL;
}

int
memcmp (const void *str1, const void *str2, size_t count)
{
  register const unsigned char *s1 = (const unsigned char*)str1;
  register const unsigned char *s2 = (const unsigned char*)str2;

  while (count-- > 0)
    {
      if (*s1++ != *s2++)
          return s1[-1] < s2[-1] ? -1 : 1;
    }
  return 0;
}

void
bcopy (const void *src, void *dest, size_t len)
{
  if (dest < src)
    {
      const char *firsts = (const char *) src;
      char *firstd = (char *) dest;
      while (len--)
        *firstd++ = *firsts++;
    }
  else
    {
      const char *lasts = (const char *)src + (len-1);
      char *lastd = (char *)dest + (len-1);
      while (len--)
        *lastd-- = *lasts--;
    }
}

void *
memcpy (void *out, const void *in, size_t length)
{
    bcopy(in, out, length);
    return out;
}

void *
memset (void *dest, register int val, register size_t len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

size_t
strnlen (const char *s, size_t maxlen)
{
  size_t i;

  for (i = 0; i < maxlen; ++i)
    if (s[i] == '\0')
      break;
  return i;
}

int
strncmp(const char *s1, const char *s2, register size_t n)
{
  register unsigned char u1, u2;

  while (n-- > 0)
    {
      u1 = (unsigned char) *s1++;
      u2 = (unsigned char) *s2++;
      if (u1 != u2)
        return u1 - u2;
      if (u1 == '\0')
        return 0;
    }
  return 0;
}

char *
strncpy(char *dst, const char *src, size_t n)
{
        if (n != 0) {
                char *d = dst;
                const char *s = src;

                do {
                        if ((*d++ = *s++) == 0) {
                                /* NUL pad the remaining n-1 bytes */
                                while (--n != 0)
                                        *d++ = 0;
                                break;
                        }
                } while (--n != 0);
        }
        return (dst);
}


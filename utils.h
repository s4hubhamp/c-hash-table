#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DA_INIT_CAP 256

typedef struct {
  char *items;
  // count of present elements inside items
  size_t count;
  // number of elements items can have
  // (Important: this is number of elements and not size in bytes)
  size_t capacity;
} String_Builder;

// Just points to some data of string builder, Does not own the memory
typedef struct {
  const char *data;
  size_t count;
} String_View;

typedef struct {
  String_View key;
  size_t value;
  bool occupied;
} FreqKV;

typedef struct {
  FreqKV *items;
  size_t count;
  size_t capacity;
} FreqKVs;

// Append one item to a dynamic array
#define da_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->count >= (da)->capacity) {                                       \
      (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2; \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
      assert((da)->items != NULL && "Buy more RAM lol");                       \
    }                                                                          \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

bool read_entire_file(const char *, String_Builder *);
void sb_append_buf(String_Builder *, char *, size_t);
String_View sv_chop_by_delim(String_View *, char);
String_View sv_chop_by_space(String_View *);
String_View sv_from_parts(const char *, size_t);
String_View sv_trim_left(String_View);
bool sv_eq(String_View, String_View);

char *get_sv_data(String_View *);

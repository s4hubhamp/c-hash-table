#include "utils.h"
#include <ctype.h>

bool read_entire_file(const char *path, String_Builder *sb) {
  FILE *f;
  f = fopen(path, "r");

  if (f == NULL) {
    printf("Could not open file for reading");
    return false;
  }

  size_t buf_size = 32 * 1024; // 32 kb
  char *buf = realloc(NULL, buf_size);

  if (buf == NULL) {
    printf("Unable to realloc, Buy more RAM!");
    return false;
  }

  size_t n_items_read = fread(buf, 1, buf_size, f);
  while (n_items_read > 0) {
    sb_append_buf(sb, buf, n_items_read);
    n_items_read = fread(buf, 1, buf_size, f);
  }

  if (ferror(f)) {
    printf("Could not read file");
    return false;
  }

  return true;
}

void sb_append_buf(String_Builder *sb, char *new_items,
                   size_t new_items_count) {
  if (sb->capacity < (sb->count + new_items_count)) {
    if (sb->capacity == 0) {
      sb->capacity = DA_INIT_CAP;
    }

    while (sb->capacity < (sb->count + new_items_count)) {
      sb->capacity *= 2;
    }

    sb->items = realloc(sb->items, sb->capacity * sizeof(*(sb->items)));
    if (sb->items == NULL) {
      printf("Unable to realloc, Buy more RAM!");
      return;
    }
  }
  memcpy(sb->items + sb->count, new_items,
         new_items_count * sizeof(*sb->items));
  sb->count += new_items_count;
}

// splits the string in two parts if delim is found
// returns the part before delim is found and then removes delim and returned
// part from original
String_View sv_chop_by_delim(String_View *sv, char delim) {
  size_t i = 0;
  while (i < sv->count && sv->data[i] != delim) {
    i++;
  }

  printf("%zu\n", i);

  String_View sv_before_delim = sv_from_parts(sv->data, i);

  printf("sv before delim %s", sv_before_delim.data);

  // delim is found inside of sv
  if (i < sv->count) {
    sv->count = sv->count - i + 1;
    sv->data = sv->data + i + 1; // move the pointer
  } else {
    sv->count = sv->count - i; // will be equal to 1
    sv->data = sv->data + i;   // points to the end character of sv
  }

  return sv_before_delim;
}

String_View sv_chop_by_space(String_View *sv) {

  size_t i = 0;
  while (i < sv->count && !isspace(sv->data[i])) {
    i += 1;
  }

  String_View sv_before_delim = sv_from_parts(sv->data, i);

  // delim is found inside of sv
  if (i < sv->count) {
    sv->count -= i + 1;
    sv->data += i + 1;
  } else {
    sv->count = sv->count - i;
    sv->data = sv->data + i;
  }

  return sv_before_delim;
}

String_View sv_trim_left(String_View sv) {
  size_t i = 0;
  while (i < sv.count && isspace(sv.data[i])) {
    i++;
  }

  return sv_from_parts(sv.data + i, sv.count - i);
}

String_View sv_from_parts(const char *data, size_t count) {
  String_View sv;
  sv.count = count;
  sv.data = data;
  return sv;
}

bool sv_eq(String_View a, String_View b) {
  if (a.count != b.count) {
    return false;
  }
  return memcmp(a.data, b.data, a.count) == 0;
}

char *get_sv_data(String_View *sv) {
  char *sv_data = malloc((sizeof(char) * sv->count) + 1);
  size_t i = 0;
  while (i < sv->count) {
    sv_data[i] = sv->data[i];
    i++;
  }
  sv_data[sv->count] = '\0';
  return sv_data;
}

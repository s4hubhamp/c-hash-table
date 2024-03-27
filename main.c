#include "utils.h"
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HASH_TABLE_CAPACITY 100 * 100

FreqKV *find_key(FreqKVs haystack, String_View needle) {
  for (size_t i = 0; i < haystack.count; i++) {
    if (sv_eq(haystack.items[i].key, needle)) {
      return &haystack.items[i];
    }
  }

  return NULL;
}

int compare_freqkv_count(const void *a, const void *b) {
  const FreqKV *f1 = a;
  const FreqKV *f2 = b;

  if (f1->value == f2->value) {
    return 0;
  }

  if (f1->value < f2->value) {
    return 1;
  }

  return -1;
}

double delta_secs(struct timespec *start, struct timespec *end) {
  double s = start->tv_sec + start->tv_nsec * 1e-9;
  double e = end->tv_sec + end->tv_nsec * 1e-9;
  return e - s;
}

// we don't care if it overflows
uint32_t dumb_hash(uint8_t *buf, size_t buf_size) {
  uint32_t result = 0;
  for (size_t i = 0; i < buf_size; i++) {
    result += (uint32_t)buf[i];
  }
  return result;
}

double clock_get_secs(void) {
  struct timespec ts = {0};
  int ret = clock_gettime(CLOCK_MONOTONIC, &ts);
  assert(ret == 0);
  return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Using linear search to check if key already exists
void naive_analysis(String_View content, const char *file_path) {
  printf("Analyzing %s with Linear search\n", file_path);
  printf("Size: %zu bytes\n", content.count);

  FreqKVs freqs = {0};

  double begin = clock_get_secs();
  while (content.count > 0) {
    content = sv_trim_left(content);
    String_View token = sv_chop_by_space(&content);

    FreqKV *found_key_value = find_key(freqs, token);

    if (found_key_value) {
      found_key_value->value += 1;
    } else {
      da_append(&freqs, ((FreqKV){.key = token, .value = 1}));
    }
  }

  double end = clock_get_secs();

  printf("  Tokens: %zu\n", freqs.count);

  qsort(freqs.items, freqs.count, sizeof(freqs.items[0]), compare_freqkv_count);

  printf("  Top 10 tokens: \n");
  for (size_t i = 0; i < 10; i++) {
    printf("    %zu: %s %zu\n", i, get_sv_data(&freqs.items[i].key),
           freqs.items[i].value);
  }

  printf("Elapsed time %.03lfs\n", end - begin);
}

#define hash_init(ht, cap)                                                     \
  do {                                                                         \
    (ht)->items = malloc(sizeof(*(ht)->items) * cap);                          \
    memset((ht)->items, 0, sizeof(*(ht)->items) * cap);                        \
    (ht)->count = 0;                                                           \
    (ht)->capacity = (cap);                                                    \
  } while (0)

uint32_t djb2(uint8_t *buf, size_t buf_size) {
  uint32_t hash = 5381;

  for (size_t i = 0; i < buf_size; ++i) {
    hash = ((hash << 5) + hash) + (uint32_t)buf[i]; /* hash * 33 + c */
  }

  return hash;
}

// Using hash table to check keys existance
bool hash_analysis(String_View content, const char *file_path) {
  printf("Analyzing %s with Hash Table\n", file_path);
  printf("Size: %zu bytes\n", content.count);

  double begin = clock_get_secs();

  FreqKVs ht = {0};
  hash_init(&ht, 1000 * 1000);

  for (; content.count > 0;) {
    content = sv_trim_left(content);
    String_View token = sv_chop_by_space(&content);

    uint32_t h = djb2((uint8_t *)token.data, token.count) % ht.capacity;

    for (size_t i = 0; i < ht.capacity && ht.items[h].occupied &&
                       !sv_eq(ht.items[h].key, token);
         ++i) {
      h = (h + 1) % ht.capacity;
    }

    if (ht.items[h].occupied) {
      if (!sv_eq(ht.items[h].key, token)) {
        printf("Table overflow\n");
        return false;
      }
      ht.items[h].value += 1;
    } else {
      ht.items[h].occupied = true;
      ht.items[h].key = token;
      ht.items[h].value = 1;
    }
  }

  FreqKVs freq = {0};
  for (size_t i = 0; i < ht.capacity; ++i) {
    if (ht.items[i].occupied) {
      da_append(&freq, ht.items[i]);
    }
  }

  double end = clock_get_secs();

  qsort(freq.items, freq.count, sizeof(freq.items[0]), compare_freqkv_count);

  printf("  Tokens: %zu tokens\n", freq.count);
  printf("  Top 10 tokens\n");
  for (size_t i = 0; i < 10; i++) {
    printf("    %zu: %s %zu\n", i, get_sv_data(&freq.items[i].key),
           freq.items[i].value);
  }
  printf("Elapsed time %.03lfs\n", end - begin);

  return true;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Please provide file path");
    return 1;
  }

  char *file_path = argv[1];
  String_Builder buf = {0};

  if (!read_entire_file(file_path, &buf)) {
    return 1;
  }

  printf("size of %s is %zu bytes\n", file_path, buf.count);

  String_View content = {.data = buf.items, .count = buf.count};

  naive_analysis(content, file_path);
  hash_analysis(content, file_path);

  return 0;
}

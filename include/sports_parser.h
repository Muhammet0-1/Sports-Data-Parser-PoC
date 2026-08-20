#ifndef SPORTS_PARSER_H
#define SPORTS_PARSER_H

#include <stddef.h>
#include <stdint.h>

#define SPORTS_PLAYER_NAME_CAPACITY 64U
#define SPORTS_RECORD_INPUT_MAX 256U

typedef struct {
    uint32_t player_id;
    char player_name[SPORTS_PLAYER_NAME_CAPACITY];
    uint32_t career_points;
} SportsPlayerRecord;

typedef enum {
    SPORTS_PARSE_OK = 0,
    SPORTS_PARSE_NULL_ARGUMENT,
    SPORTS_PARSE_INPUT_TOO_LONG,
    SPORTS_PARSE_FIELD_COUNT,
    SPORTS_PARSE_INVALID_ID,
    SPORTS_PARSE_INVALID_NAME,
    SPORTS_PARSE_INVALID_POINTS
} SportsParseStatus;

/*
 * Parse exactly input_length bytes; input need not be NUL-terminated.
 * On failure, *output is left unchanged. error_message is optional: pass NULL
 * or a zero size to suppress it. A non-NULL, non-zero buffer is always
 * NUL-terminated, including when the diagnostic is truncated.
 */
SportsParseStatus sports_parse_record(
    const char *input,
    size_t input_length,
    SportsPlayerRecord *output,
    char *error_message,
    size_t error_message_size
);

const char *sports_parse_status_string(SportsParseStatus status);

#endif

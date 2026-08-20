#include "sports_parser.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *data;
    size_t length;
} Span;

static void set_error(char *buffer, size_t size, const char *message) {
    if ((buffer != NULL) && (size > 0U)) {
        (void)snprintf(buffer, size, "%s", message);
    }
}

static bool is_ascii_space(unsigned char value) {
    return (value == (unsigned char)' ') || (value == (unsigned char)'\t');
}

static Span trim_span(Span span) {
    while ((span.length > 0U) && is_ascii_space((unsigned char)span.data[0])) {
        span.data += 1;
        span.length -= 1U;
    }
    while ((span.length > 0U) && is_ascii_space((unsigned char)span.data[span.length - 1U])) {
        span.length -= 1U;
    }
    return span;
}

static bool parse_uint32(Span span, uint32_t *value) {
    uint32_t accumulator = 0U;
    size_t index = 0U;

    span = trim_span(span);
    if ((span.length == 0U) || (value == NULL)) {
        return false;
    }
    for (index = 0U; index < span.length; ++index) {
        const unsigned char current = (unsigned char)span.data[index];
        if ((current < (unsigned char)'0') || (current > (unsigned char)'9')) {
            return false;
        }
        const uint32_t digit = (uint32_t)(current - (unsigned char)'0');
        if (accumulator > (UINT32_MAX - digit) / 10U) {
            return false;
        }
        accumulator = (accumulator * 10U) + digit;
    }
    *value = accumulator;
    return true;
}

static bool continuation(unsigned char value) {
    return (value & 0xC0U) == 0x80U;
}

static bool valid_utf8(const unsigned char *data, size_t length) {
    size_t index = 0U;

    while (index < length) {
        const unsigned char first = data[index];
        if (first <= 0x7FU) {
            if ((first < 0x20U) || (first == 0x7FU)) {
                return false;
            }
            index += 1U;
        } else if ((first >= 0xC2U) && (first <= 0xDFU)) {
            if ((index + 1U >= length) || !continuation(data[index + 1U])) {
                return false;
            }
            index += 2U;
        } else if ((first >= 0xE0U) && (first <= 0xEFU)) {
            if ((index + 2U >= length) || !continuation(data[index + 1U]) ||
                !continuation(data[index + 2U])) {
                return false;
            }
            if (((first == 0xE0U) && (data[index + 1U] < 0xA0U)) ||
                ((first == 0xEDU) && (data[index + 1U] > 0x9FU))) {
                return false;
            }
            index += 3U;
        } else if ((first >= 0xF0U) && (first <= 0xF4U)) {
            if ((index + 3U >= length) || !continuation(data[index + 1U]) ||
                !continuation(data[index + 2U]) || !continuation(data[index + 3U])) {
                return false;
            }
            if (((first == 0xF0U) && (data[index + 1U] < 0x90U)) ||
                ((first == 0xF4U) && (data[index + 1U] > 0x8FU))) {
                return false;
            }
            index += 4U;
        } else {
            return false;
        }
    }
    return true;
}

static bool parse_name(Span span, char destination[SPORTS_PLAYER_NAME_CAPACITY]) {
    span = trim_span(span);
    if ((span.length == 0U) || (span.length >= SPORTS_PLAYER_NAME_CAPACITY)) {
        return false;
    }
    if (!valid_utf8((const unsigned char *)span.data, span.length)) {
        return false;
    }
    (void)memcpy(destination, span.data, span.length);
    destination[span.length] = '\0';
    return true;
}

SportsParseStatus sports_parse_record(
    const char *input,
    size_t input_length,
    SportsPlayerRecord *output,
    char *error_message,
    size_t error_message_size
) {
    size_t separators[2] = {0U, 0U};
    size_t separator_count = 0U;
    size_t index = 0U;
    SportsPlayerRecord candidate = {0U, {0}, 0U};
    Span fields[3];

    if ((input == NULL) || (output == NULL)) {
        set_error(error_message, error_message_size, "input and output are required");
        return SPORTS_PARSE_NULL_ARGUMENT;
    }
    if (input_length > SPORTS_RECORD_INPUT_MAX) {
        set_error(error_message, error_message_size, "record exceeds the 256-byte limit");
        return SPORTS_PARSE_INPUT_TOO_LONG;
    }
    for (index = 0U; index < input_length; ++index) {
        if (input[index] == '\0') {
            set_error(error_message, error_message_size, "record contains an embedded NUL byte");
            return SPORTS_PARSE_FIELD_COUNT;
        }
        if (input[index] == ',') {
            if (separator_count >= 2U) {
                set_error(error_message, error_message_size, "record must contain exactly 3 fields");
                return SPORTS_PARSE_FIELD_COUNT;
            }
            separators[separator_count] = index;
            separator_count += 1U;
        }
    }
    if (separator_count != 2U) {
        set_error(error_message, error_message_size, "record must contain exactly 3 fields");
        return SPORTS_PARSE_FIELD_COUNT;
    }

    fields[0] = (Span){input, separators[0]};
    fields[1] = (Span){input + separators[0] + 1U, separators[1] - separators[0] - 1U};
    fields[2] = (Span){input + separators[1] + 1U, input_length - separators[1] - 1U};

    if (!parse_uint32(fields[0], &candidate.player_id) || (candidate.player_id == 0U)) {
        set_error(error_message, error_message_size, "player id must be an integer from 1 to 4294967295");
        return SPORTS_PARSE_INVALID_ID;
    }
    if (!parse_name(fields[1], candidate.player_name)) {
        set_error(error_message, error_message_size, "player name must be valid UTF-8 from 1 to 63 bytes");
        return SPORTS_PARSE_INVALID_NAME;
    }
    if (!parse_uint32(fields[2], &candidate.career_points)) {
        set_error(error_message, error_message_size, "career points must be an integer from 0 to 4294967295");
        return SPORTS_PARSE_INVALID_POINTS;
    }

    *output = candidate;
    set_error(error_message, error_message_size, "ok");
    return SPORTS_PARSE_OK;
}

const char *sports_parse_status_string(SportsParseStatus status) {
    switch (status) {
        case SPORTS_PARSE_OK:
            return "ok";
        case SPORTS_PARSE_NULL_ARGUMENT:
            return "null_argument";
        case SPORTS_PARSE_INPUT_TOO_LONG:
            return "input_too_long";
        case SPORTS_PARSE_FIELD_COUNT:
            return "field_count";
        case SPORTS_PARSE_INVALID_ID:
            return "invalid_id";
        case SPORTS_PARSE_INVALID_NAME:
            return "invalid_name";
        case SPORTS_PARSE_INVALID_POINTS:
            return "invalid_points";
        default:
            return "unknown";
    }
}

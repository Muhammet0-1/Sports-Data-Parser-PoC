#include "sports_parser.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static unsigned int failures = 0U;
static unsigned int checks = 0U;

#define CHECK(condition)                                                                      \
    do {                                                                                      \
        checks += 1U;                                                                         \
        if (!(condition)) {                                                                   \
            failures += 1U;                                                                   \
            (void)fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);       \
        }                                                                                     \
    } while (0)

static SportsParseStatus parse_text(const char *text, SportsPlayerRecord *record) {
    char error[160] = {0};
    return sports_parse_record(text, strlen(text), record, error, sizeof(error));
}

static void check_unchanged(const SportsPlayerRecord *record) {
    CHECK(record->player_id == 99U);
    CHECK(strcmp(record->player_name, "unchanged") == 0);
    CHECK(record->career_points == 88U);
}

static void test_valid_records(void) {
    SportsPlayerRecord record = {0U, {0}, 0U};

    CHECK(parse_text("23,Michael Jordan,32292", &record) == SPORTS_PARSE_OK);
    CHECK(record.player_id == 23U);
    CHECK(strcmp(record.player_name, "Michael Jordan") == 0);
    CHECK(record.career_points == 32292U);

    CHECK(parse_text("  1  ,  Alperen Şengün  ,  0  ", &record) == SPORTS_PARSE_OK);
    CHECK(record.player_id == 1U);
    CHECK(strcmp(record.player_name, "Alperen Şengün") == 0);
    CHECK(record.career_points == 0U);

    CHECK(parse_text("4294967295,Max,4294967295", &record) == SPORTS_PARSE_OK);
    CHECK(record.player_id == UINT32_MAX);
    CHECK(record.career_points == UINT32_MAX);

    CHECK(parse_text("7,Quote \" and slash \\,10", &record) == SPORTS_PARSE_OK);
    CHECK(strcmp(record.player_name, "Quote \" and slash \\") == 0);
}

static void test_field_validation(void) {
    SportsPlayerRecord record = {99U, "unchanged", 88U};

    CHECK(parse_text("1,only-two", &record) == SPORTS_PARSE_FIELD_COUNT);
    check_unchanged(&record);
    CHECK(parse_text("1,name,2,extra", &record) == SPORTS_PARSE_FIELD_COUNT);
    check_unchanged(&record);
    CHECK(parse_text("0,name,2", &record) == SPORTS_PARSE_INVALID_ID);
    check_unchanged(&record);
    CHECK(parse_text("-1,name,2", &record) == SPORTS_PARSE_INVALID_ID);
    CHECK(parse_text("4294967296,name,2", &record) == SPORTS_PARSE_INVALID_ID);
    CHECK(parse_text("42949672960,name,2", &record) == SPORTS_PARSE_INVALID_ID);
    CHECK(parse_text("999999999999999999999999999999999999,name,2", &record) == SPORTS_PARSE_INVALID_ID);
    CHECK(parse_text("abc,name,2", &record) == SPORTS_PARSE_INVALID_ID);
    CHECK(parse_text("1,,2", &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(parse_text("1,name,-1", &record) == SPORTS_PARSE_INVALID_POINTS);
    CHECK(parse_text("1,name,4294967296", &record) == SPORTS_PARSE_INVALID_POINTS);
    CHECK(parse_text("1,name,42949672960", &record) == SPORTS_PARSE_INVALID_POINTS);
    CHECK(parse_text("1,name,2x", &record) == SPORTS_PARSE_INVALID_POINTS);

    check_unchanged(&record);
}

static void test_length_and_byte_validation(void) {
    SportsPlayerRecord record = {0U, {0}, 0U};
    char long_name[80];
    char long_input[SPORTS_RECORD_INPUT_MAX + 2U];
    char maximum_input[SPORTS_RECORD_INPUT_MAX];
    char over_maximum_input[SPORTS_RECORD_INPUT_MAX + 1U];
    char boundary_name[SPORTS_PLAYER_NAME_CAPACITY];
    char boundary_record[SPORTS_PLAYER_NAME_CAPACITY + 8U];
    char oversized_name[SPORTS_PLAYER_NAME_CAPACITY + 1U];
    char oversized_record[SPORTS_PLAYER_NAME_CAPACITY + 9U];
    const char embedded_nul[] = {'1', ',', 'A', '\0', 'B', ',', '2'};
    const char invalid_utf8[] = {'1', ',', (char)0xC0, (char)0xAF, ',', '2', '\0'};
    const char overlong_three[] = {'1', ',', (char)0xE0, (char)0x80, (char)0x80, ',', '2', '\0'};
    const char surrogate[] = {'1', ',', (char)0xED, (char)0xA0, (char)0x80, ',', '2', '\0'};
    const char invalid_continuation[] = {'1', ',', (char)0xE2, 'A', (char)0xA1, ',', '2', '\0'};
    const char above_unicode_max[] = {
        '1', ',', (char)0xF4, (char)0x90, (char)0x80, (char)0x80, ',', '2', '\0'
    };
    const char valid_unicode_max[] = {
        '1', ',', (char)0xF4, (char)0x8F, (char)0xBF, (char)0xBF, ',', '2', '\0'
    };
    const char control_character[] = {'1', ',', 'A', '\n', 'B', ',', '2', '\0'};
    const char explicit_length_record[] = {'4', '2', ',', 'N', 'o', 'n', 'N', 'u', 'l', ',', '7'};

    (void)memset(long_name, 'A', sizeof(long_name));
    long_name[0] = '1';
    long_name[1] = ',';
    long_name[sizeof(long_name) - 3U] = ',';
    long_name[sizeof(long_name) - 2U] = '2';
    long_name[sizeof(long_name) - 1U] = '\0';
    CHECK(parse_text(long_name, &record) == SPORTS_PARSE_INVALID_NAME);

    (void)memset(boundary_name, 'B', sizeof(boundary_name) - 1U);
    boundary_name[sizeof(boundary_name) - 1U] = '\0';
    (void)snprintf(boundary_record, sizeof(boundary_record), "1,%s,2", boundary_name);
    CHECK(parse_text(boundary_record, &record) == SPORTS_PARSE_OK);
    CHECK(strlen(record.player_name) == SPORTS_PLAYER_NAME_CAPACITY - 1U);

    (void)memset(oversized_name, 'C', sizeof(oversized_name) - 1U);
    oversized_name[sizeof(oversized_name) - 1U] = '\0';
    (void)snprintf(oversized_record, sizeof(oversized_record), "1,%s,2", oversized_name);
    CHECK(parse_text(oversized_record, &record) == SPORTS_PARSE_INVALID_NAME);

    (void)memset(long_input, 'A', sizeof(long_input));
    CHECK(
        sports_parse_record(
            long_input,
            sizeof(long_input),
            &record,
            NULL,
            0U
        ) == SPORTS_PARSE_INPUT_TOO_LONG
    );
    (void)memset(maximum_input, 'A', sizeof(maximum_input));
    CHECK(
        sports_parse_record(
            maximum_input,
            sizeof(maximum_input),
            &record,
            NULL,
            0U
        ) == SPORTS_PARSE_FIELD_COUNT
    );
    (void)memset(over_maximum_input, 'A', sizeof(over_maximum_input));
    CHECK(
        sports_parse_record(
            over_maximum_input,
            sizeof(over_maximum_input),
            &record,
            NULL,
            0U
        ) == SPORTS_PARSE_INPUT_TOO_LONG
    );
    CHECK(
        sports_parse_record(
            embedded_nul,
            sizeof(embedded_nul),
            &record,
            NULL,
            0U
        ) == SPORTS_PARSE_FIELD_COUNT
    );
    CHECK(parse_text(invalid_utf8, &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(parse_text(overlong_three, &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(parse_text(surrogate, &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(parse_text(invalid_continuation, &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(parse_text(above_unicode_max, &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(parse_text(valid_unicode_max, &record) == SPORTS_PARSE_OK);
    CHECK(parse_text(control_character, &record) == SPORTS_PARSE_INVALID_NAME);
    CHECK(
        sports_parse_record(
            explicit_length_record,
            sizeof(explicit_length_record),
            &record,
            NULL,
            0U
        ) == SPORTS_PARSE_OK
    );
    CHECK(record.player_id == 42U);
    CHECK(strcmp(record.player_name, "NonNul") == 0);
    CHECK(record.career_points == 7U);
}

static void test_api_contract(void) {
    SportsPlayerRecord record = {0U, {0}, 0U};
    char error[4] = {'X', 'X', 'X', 'X'};
    char one_byte_error[1] = {'X'};
    char zero_size_error[1] = {'X'};

    CHECK(sports_parse_record(NULL, 0U, &record, error, sizeof(error)) == SPORTS_PARSE_NULL_ARGUMENT);
    CHECK(error[sizeof(error) - 1U] == '\0');
    CHECK(sports_parse_record(NULL, 0U, &record, one_byte_error, sizeof(one_byte_error)) == SPORTS_PARSE_NULL_ARGUMENT);
    CHECK(one_byte_error[0] == '\0');
    CHECK(sports_parse_record(NULL, 0U, &record, zero_size_error, 0U) == SPORTS_PARSE_NULL_ARGUMENT);
    CHECK(zero_size_error[0] == 'X');
    CHECK(sports_parse_record(NULL, 0U, &record, NULL, 128U) == SPORTS_PARSE_NULL_ARGUMENT);
    CHECK(sports_parse_record("1,A,2", 5U, NULL, NULL, 0U) == SPORTS_PARSE_NULL_ARGUMENT);
    CHECK(strcmp(sports_parse_status_string(SPORTS_PARSE_OK), "ok") == 0);
    CHECK(strcmp(sports_parse_status_string((SportsParseStatus)99), "unknown") == 0);
}

int main(void) {
    test_valid_records();
    test_field_validation();
    test_length_and_byte_validation();
    test_api_contract();

    if (failures != 0U) {
        (void)fprintf(stderr, "%u of %u checks failed\n", failures, checks);
        return 1;
    }
    (void)printf("All %u parser checks passed\n", checks);
    return 0;
}

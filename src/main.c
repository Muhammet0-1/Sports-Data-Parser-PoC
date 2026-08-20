#include "sports_parser.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static void print_json_string(const char *value) {
    const unsigned char *cursor = (const unsigned char *)value;
    (void)putchar('"');
    while (*cursor != '\0') {
        if ((*cursor == (unsigned char)'"') || (*cursor == (unsigned char)'\\')) {
            (void)putchar('\\');
        }
        (void)putchar((int)*cursor);
        cursor += 1;
    }
    (void)putchar('"');
}

static void print_usage(const char *program) {
    (void)fprintf(stderr, "Usage: %s '<player_id>,<player_name>,<career_points>'\n", program);
}

int main(int argc, char *argv[]) {
    SportsPlayerRecord record = {0U, {0}, 0U};
    char error_message[160] = {0};
    SportsParseStatus status;

    if ((argc == 2) && ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0))) {
        print_usage(argv[0]);
        return 0;
    }
    if (argc != 2) {
        print_usage(argv[0]);
        return 2;
    }
    status = sports_parse_record(argv[1], strlen(argv[1]), &record, error_message, sizeof(error_message));
    if (status != SPORTS_PARSE_OK) {
        (void)fprintf(
            stderr,
            "parse error [%s]: %s\n",
            sports_parse_status_string(status),
            error_message
        );
        return 1;
    }

    (void)printf(
        "{\"career_points\":%" PRIu32 ",\"player_id\":%" PRIu32 ",\"player_name\":",
        record.career_points,
        record.player_id
    );
    print_json_string(record.player_name);
    (void)puts("}");
    return 0;
}

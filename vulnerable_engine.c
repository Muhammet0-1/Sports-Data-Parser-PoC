/*
 * Deliberately unsafe local training example.
 *
 * Direct compilation is blocked. The Makefile enables this source only with
 * AddressSanitizer and UndefinedBehaviorSanitizer so the overflow is detected
 * instead of being presented as an exploitation primitive.
 */

#ifndef SPORTSDATA_ENABLE_UNSAFE_LAB
#error "Use `make lab`; direct compilation of the unsafe example is intentionally blocked."
#endif

#include <stdio.h>
#include <string.h>

static void demonstrate_unsafe_copy(const char *input) {
    char player_name[16];
    (void)strcpy(player_name, input); /* Intentionally unsafe: sanitizer teaching case. */
    (void)printf("unsafe parser accepted: %s\n", player_name);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        (void)fprintf(stderr, "Usage: %s <local-training-input>\n", argv[0]);
        return 2;
    }
    demonstrate_unsafe_copy(argv[1]);
    return 0;
}

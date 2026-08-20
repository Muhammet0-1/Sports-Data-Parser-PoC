CC ?= cc
CPPFLAGS := -Iinclude
WARNINGS := -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 -Werror
CFLAGS ?= -std=c11 -O2
HARDENING := -fstack-protector-strong -D_FORTIFY_SOURCE=3 -fPIE
LDFLAGS ?=
HARDEN_LDFLAGS := -Wl,-z,relro,-z,now,-z,noexecstack -pie
SANITIZERS := -fsanitize=address,undefined -fno-omit-frame-pointer
SANITIZER_LDFLAGS := $(SANITIZERS) -Wl,-z,noexecstack
BUILD_DIR := build
PARSER_SOURCE := src/sports_parser.c
PARSER_HEADER := include/sports_parser.h
APP := $(BUILD_DIR)/sports-parser
COMPAT_APP := $(BUILD_DIR)/secure_app
TEST_APP := $(BUILD_DIR)/test-parser
SANITIZED_TEST_APP := $(BUILD_DIR)/test-parser-sanitized
LAB_APP := $(BUILD_DIR)/vulnerable-lab

.PHONY: all clean lab lab-check sanitize secure_app test

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): src/main.c $(PARSER_SOURCE) $(PARSER_HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) $(HARDENING) src/main.c $(PARSER_SOURCE) $(LDFLAGS) $(HARDEN_LDFLAGS) -o $@

secure_app: $(COMPAT_APP)

$(COMPAT_APP): secure_patch.c src/main.c $(PARSER_SOURCE) $(PARSER_HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) $(HARDENING) secure_patch.c $(PARSER_SOURCE) $(LDFLAGS) $(HARDEN_LDFLAGS) -o $@

$(TEST_APP): tests/test_parser.c $(PARSER_SOURCE) $(PARSER_HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) $(HARDENING) tests/test_parser.c $(PARSER_SOURCE) $(LDFLAGS) $(HARDEN_LDFLAGS) -o $@

test: $(APP) $(TEST_APP)
	$(TEST_APP)
	$(APP) '23,Michael Jordan,32292'
	test "$$($(APP) '7,Quote " and slash \\,10')" = '{"career_points":10,"player_id":7,"player_name":"Quote \" and slash \\\\"}'
	! $(APP) '0,Invalid,100'

$(SANITIZED_TEST_APP): tests/test_parser.c $(PARSER_SOURCE) $(PARSER_HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -std=c11 -O1 -g $(WARNINGS) $(SANITIZERS) tests/test_parser.c $(PARSER_SOURCE) $(SANITIZER_LDFLAGS) -o $@

sanitize: $(SANITIZED_TEST_APP)
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(SANITIZED_TEST_APP)

$(LAB_APP): vulnerable_engine.c | $(BUILD_DIR)
	$(CC) -std=c11 -O1 -g -DSPORTSDATA_ENABLE_UNSAFE_LAB $(SANITIZERS) vulnerable_engine.c $(SANITIZER_LDFLAGS) -o $@

lab: $(LAB_APP)
	@printf '%s\n' 'Unsafe example built with ASan/UBSan. Run `make lab-check` for a controlled detection.'

lab-check: $(LAB_APP)
	@log='$(BUILD_DIR)/lab-check.log'; \
	trap '$(RM) "$$log"' EXIT HUP INT TERM; \
	set +e; \
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 $(LAB_APP) 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' >"$$log" 2>&1; \
	status=$$?; \
	set -e; \
	cat "$$log"; \
	test "$$status" -ne 0; \
	grep -Fq 'ERROR: AddressSanitizer: stack-buffer-overflow' "$$log"; \
	grep -Fq 'vulnerable_engine.c' "$$log"
	@printf '%s\n' 'Sanitizer rejected the deliberately unsafe copy as expected.'

clean:
	$(RM) -r -- $(BUILD_DIR)

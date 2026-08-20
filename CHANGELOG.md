# Changelog

## [1.0.0] - 2026-08-20

### Added

- Reusable C11 parser API with explicit input lengths and stable status codes.
- Strict numeric, field-count, length, control-character, and UTF-8 validation.
- JSON command-line interface and 67-check native C test suite.
- Hardened GCC/Clang builds plus ASan/UBSan regression targets.
- CI, security policy, contribution guide, editor configuration, and MIT license.

### Changed

- Reframed the repository as a synthetic local secure-coding laboratory.
- Replaced the original standalone patch with a reusable parser implementation.
- Restricted the deliberately unsafe example behind a compile-time guard and sanitizer-only
  Makefile target.

### Removed

- Disabled stack-protector and executable-stack build flags.
- Claims tying the synthetic code to a real organization or validated production vulnerability.
- Privilege-manipulation and exploitation-oriented demonstration text.

## [0.1.0]

- Initial vulnerable/patched buffer-copy demonstration.

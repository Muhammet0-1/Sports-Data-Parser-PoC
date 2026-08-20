# Contributing

## Local checks

Run both supported compiler paths when they are available:

```bash
make clean
make CC=gcc test sanitize
make clean
make CC=clang test sanitize
```

## Expectations

- Keep production code free of intentionally unsafe functions and disabled compiler protections.
- Preserve explicit lengths, bounded copies, overflow checks, and failure-without-partial-output.
- Add a regression check for every parser rule or bug fix.
- Keep tests offline, deterministic, and independent from real sports services or datasets.
- Do not add exploit payloads, real proprietary code, personal data, credentials, or claims that
  cannot be substantiated by repository evidence.
- Deliberately unsafe examples must remain isolated, compile-time guarded, sanitizer-enabled, and
  excluded from the default build.

Open a focused pull request describing the behavior change, threat model, compiler versions, and
the commands used for validation.

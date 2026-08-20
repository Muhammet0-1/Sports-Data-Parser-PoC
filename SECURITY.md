# Security Policy

## Reporting

Use GitHub private vulnerability reporting when available. Otherwise contact the repository owner
privately before publishing details. Include the affected commit, compiler, platform, minimal input,
observed sanitizer output, and expected behavior.

Do not test against third-party sports systems, include proprietary code/data, or attach weaponized
payloads. This repository is a synthetic local lab and is not authorization to assess any external
service.

## Boundaries

The maintained parser processes only caller-provided in-memory bytes. It does not fetch remote data,
open files, execute commands, authenticate to services, or persist results.

The root `vulnerable_engine.c` file is intentionally defective for local teaching. Direct compilation
is blocked; supported builds require `make lab`, which enables ASan and UBSan. It is excluded from the
default build and must never be linked into production targets.

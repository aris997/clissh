# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

clissh is a C command-line utility that parses SSH config files and presents an interactive numbered menu to select and connect to a host via SSH.

## Build

```
make          # compile with gcc -Wall
make clean    # remove binary
```

The binary is named `clissh`. No external dependencies beyond the C standard library.

## Architecture

Single-file C program (`clissh.c`):

- **`struct config_item`** — holds a parsed host name (32-char buffer). An array of up to 128 items stores all parsed hosts.
- **`start_with()`** — string prefix matcher used to identify `Host` lines in SSH config.
- **`read_config()`** — reads an SSH config file line-by-line, extracts hostnames from `Host` directives, populates the config array, and prints a numbered menu.
- **`main()`** — accepts `-f <file>` (defaults to `config` in CWD), `--edit` (opens config in `$EDITOR`), and `-h`/`--help`. Calls `read_config()`, takes user input for host selection, and runs `/usr/bin/ssh <host>` via `system()`.

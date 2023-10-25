#pragma once

// Copyright 2023 Owen Wheary

// ANSI code definitions gotten from this Stack Overflow post:
// https://stackoverflow.com/a/3219471
#define ANSI_ESCAPE "\x1b["
#define ANSI_COLOR_RED ANSI_ESCAPE "31m"
#define ANSI_COLOR_YELLOW ANSI_ESCAPE "33m"
#define ANSI_COLOR_GREEN ANSI_ESCAPE "32m"
#define ANSI_COLOR_BLUE ANSI_ESCAPE "34m"
#define ANSI_COLOR_MAGENTA ANSI_ESCAPE "35m"
#define ANSI_COLOR_CYAN ANSI_ESCAPE "36m"
#define ANSI_COLOR_RESET ANSI_ESCAPE "0m"

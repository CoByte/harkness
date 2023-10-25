// Copyright 2023 Owen Wheary

#include "./colors.h"
#include <stdio.h>

int main() {
  printf(ANSI_COLOR_RED "AAAA\n");
  printf(ANSI_COLOR_YELLOW "AAAA\n");
  printf(ANSI_COLOR_GREEN "AAAA\n");
  printf(ANSI_COLOR_BLUE "AAAA\n");
  printf(ANSI_COLOR_MAGENTA "AAAA\n");
  printf(ANSI_COLOR_CYAN "AAAA\n");
  printf(ANSI_COLOR_RESET "AAAA\n");

  return 0;
}

#include <stdbool.h>

enum OptionCallbackResult {
  OPTION_CALLBACK_RESULT_OK,
  /** Option executed, and failed, abort parsing */
  OPTION_CALLBACK_RESULT_ERROR,
  /** Option executed, but no next options should be executed, only parsed */
  OPTION_CALLBACK_RESULT_OK_STOP,
};

struct OptionDef {
  char *name;
  char short_option;
  char *long_option;
  bool has_arg;
  char *description;
  enum OptionCallbackResult (*callback)(const char *value);
};

struct ParseOptionsResult {
  int optionsCount;
  int argcLeft;
  bool shouldContinue;
  char **remainingArgv;
};

struct CliOptions {
  struct OptionDef *optionDefs;
  int count;
};

struct ParseOptionsResult parse_options(int argc, char **argv,
                                        struct CliOptions cliOptions);
void print_help(char *command, struct CliOptions cliOptions);

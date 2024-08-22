#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli-parser.h"

struct ParseOptionsResult parse_options(int argc, char **argv,
                                        const struct CliOptions cliOptions) {
  struct ParseOptionsResult result = {
      .optionsCount = 0, .argcLeft = argc, .shouldContinue = true};

  const struct OptionDef *currentOptionDef = NULL;
  while (result.argcLeft > 0) {
    char *arg = argv[0];
    result.argcLeft--;
    argv++;
    if (currentOptionDef != NULL && currentOptionDef->has_arg) {
      if (result.shouldContinue) {
        enum OptionCallbackResult callbackResult =
            currentOptionDef->callback(arg);
        if (callbackResult == OPTION_CALLBACK_RESULT_ERROR) {
          exit(2);
        } else if (callbackResult == OPTION_CALLBACK_RESULT_OK_STOP) {
          result.shouldContinue = false;
        }
      }
      currentOptionDef = NULL;
      continue;
    }
    const bool isLong = strncmp(arg, "--", 2) == 0;
    const bool hasDash = arg[0] == '-';
    for (int i = 0; i < cliOptions.count; i++) {
      const struct OptionDef *optionDef = &cliOptions.optionDefs[i];
      if (isLong) {
        if (optionDef->name != NULL && strcmp(arg + 2, optionDef->name) == 0) {
          currentOptionDef = optionDef;
          break;
        }
      } else if (hasDash && strlen(arg) == 2) {
        if (arg[1] == optionDef->short_option) {
          currentOptionDef = optionDef;
          break;
        }
      }
    }

    if (currentOptionDef != NULL) {
      if (!currentOptionDef->has_arg) {
        if (result.shouldContinue) {
          enum OptionCallbackResult callbackResult =
              currentOptionDef->callback(NULL);
          if (callbackResult == OPTION_CALLBACK_RESULT_ERROR) {
            exit(2);
          } else if (callbackResult == OPTION_CALLBACK_RESULT_OK_STOP) {
            result.shouldContinue = false;
          }
        }
        currentOptionDef = NULL;
      }

      continue;
    }

    if (arg[0] == '-' && strlen(arg) != 1) {
      fprintf(stderr, "Unrecognized option \"%s\"\n", arg);
      exit(2);
    } else {
      argv--;
      result.argcLeft++;
      break;
    }
  }

  if (currentOptionDef != NULL) {
    fprintf(stderr, "Missing value for option \"%s\"\n",
            currentOptionDef->name);

    exit(2);
  }

  result.remainingArgv = argv;

  return result;
}

void print_help(char *command, const struct CliOptions cliOptions) {
  printf("Usage: %s [options] file\n", command);
  printf("Options:\n");

  int longestOption = 0;

  for (int i = 0; i < cliOptions.count; i++) {
    const struct OptionDef *option = &cliOptions.optionDefs[i];
    // 3 is for `-${short_name} ` and 2 is for `  ` printed
    // before the option
    int optionLength = 3 + 2;

    if (option->long_option != NULL) {
      // 2 is for `--`, 3 is for `-${short_name} ` and 2 is for `  ` printed
      optionLength += strlen(option->long_option) + 2;
    }

    // ` ` and `value`
    if (option->has_arg)
      optionLength += 1 + 5;

    if (optionLength > longestOption)
      longestOption = optionLength;
  }

  int padLength = longestOption + 2;

  for (size_t i = 0; i < cliOptions.count; i++) {
    const struct OptionDef option = cliOptions.optionDefs[i];
    printf("  ");

    int toPad = padLength;

    if (option.short_option) {
      printf("-%c", option.short_option);
      toPad -= 2;
    }

    if (option.long_option != NULL) {
      if (option.short_option) {
        printf(", ");
        toPad -= 2;
      }
      printf("--%s", option.long_option);
      toPad -= strlen(option.long_option) + 2;
    }

    if (option.has_arg) {
      printf(" value");
      toPad -= 6;
    }

    printf("%*s", toPad, " ");
    printf("%s\n", option.description);
  }
}

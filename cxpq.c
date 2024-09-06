#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./parsers/xpath.h"
#include "cli-parser.h"
#include "query-executor.h"
#include "query.h"
#include "util.h"
#include "xml-parser.h"

#define VERSION "0.1"

enum OptionCallbackResult helpHandler(const char *value);
enum OptionCallbackResult printVersion(const char *value) {
  printf("Cxpq version " VERSION "\n");

  return OPTION_CALLBACK_RESULT_OK_STOP;
}

char *queryTypes[] = {"xpath"};

char *queryType = NULL;
const char *rawQuery = NULL;

enum OptionCallbackResult setQueryType(const char *value) {
  for (size_t i = 0; i < sizeof(queryTypes) / sizeof(queryTypes[0]); i++) {
    if (strlen(value) == strlen(queryTypes[i]) &&
        strcmp(value, queryTypes[i]) == 0) {
      queryType = queryTypes[i];

      return OPTION_CALLBACK_RESULT_OK;
    }
  }

  PRINT_ERROR("Invalid query type %s\n", value);

  return OPTION_CALLBACK_RESULT_ERROR;
}

enum OptionCallbackResult setQuery(const char *value) {
  rawQuery = value;

  return OPTION_CALLBACK_RESULT_OK;
}

struct OptionDef optionDefsStatic[] = {
    {
        .name = "help",
        .short_option = 'h',
        .long_option = "help",
        .has_arg = false,
        .description = "Print this help message and exit.",
        .callback = &helpHandler,
    },
    {
        .name = "version",
        .short_option = 'v',
        .long_option = "version",
        .has_arg = false,
        .description = "Print version and exits.",
        .callback = &printVersion,
    },
    {
        .name = "query-type",
        .short_option = 'Q',
        .has_arg = true,
        .description = "Set query type to use. Possible values are: [xpath]",
        .callback = &setQueryType,
    },
    {
        .name = "query",
        .short_option = 'q',
        .long_option = "query",
        .has_arg = true,
        .description =
            "Query type to use. Make sure to set query type using -Q.",
        .callback = &setQuery,
    },
};

struct CliOptions cliOptions = {.optionDefs = optionDefsStatic,
                                .count = sizeof(optionDefsStatic) /
                                         sizeof(optionDefsStatic[0])};

char *command;

enum OptionCallbackResult helpHandler(const char *value) {
  print_help(command, cliOptions);

  return OPTION_CALLBACK_RESULT_OK_STOP;
}

FILE *redirectStdin() {
  FILE *file = tmpfile();
  if (!file) {
    perror("Failed to create tempfile");
    return NULL;
  }

  char chunk[1024];
  while (fgets(chunk, 1024, stdin)) {
    fputs(chunk, file);
  }

  rewind(file);

  return file;
}

int main(int argc, char *argv[]) {
  command = argv[0];
  int argcLeft = argc - 1;
  argv++;

  if (argcLeft == 0) {
    helpHandler(NULL);
    PRINT_ERROR("No file was provided\n");
    return 1;
  }

  struct ParseOptionsResult result = parse_options(argcLeft, argv, cliOptions);

  if (!result.shouldContinue)
    return 1;

  if (!rawQuery && queryType) {
    PRINT_ERROR("Missing query, but queryType \"%s\" was provided, use \"-q "
                "query\" or \"--query query\" to provide query.\n",
                queryType);
    return 1;
  }
  if (rawQuery && !queryType) {
    PRINT_ERROR("Missing queryType for query, assuming \"%s\"\n",
                queryTypes[0]);
    queryType = queryTypes[0];
  }

  if (result.argcLeft != 1) {
    if (result.argcLeft == 0)
      PRINT_ERROR("No file was provided\n");
    if (result.argcLeft > 1)
      PRINT_ERROR("Multiple files provided\n");

    return 1;
  }

  Query *query = NULL;
  if (rawQuery) {
    if (strcmp(queryType, "xpath") == 0) {
      query = parseXpath(rawQuery);
    }

    if (!query) {
      PRINT_ERROR("Query could not be parsed\n");
      return 1;
    }
  }

  const char *fileName = result.remainingArgv[0];
  FILE *file;
  if (strlen(fileName) == 1 && fileName[0] == '-') {
    FILE *newFile = redirectStdin();
    if (!newFile) {
      perror("Failed to redirect stdin");
    }
    file = newFile;
  } else {
    file = fopen(fileName, "r");
  }
  if (!file) {
    PRINT_ERROR("Failed to load file %s\n", fileName);
    return 1;
  }

  XMLDocument *document = parseXML(file);
  XMLElementNode *root =
      document->rootIndex >= 0
          ? (XMLElementNode *)document->nodes->nodes[document->rootIndex]
          : NULL;

  fclose(file);

  if (!root) {
    freeXMLDocument(document);
    PRINT_ERROR("Failed to parse provided XML file, nodes %p\n",
                document->nodes->nodes);

    if (query)
      freeQuery(query);
    return 2;
  } else {
    if (!query) {
      printf("Root node %s (%lu)\n", root->tag->value, document->rootIndex);

      printXMLDocument(document);
    } else {
      NodeCollection *result = executeQuery(document, query);

      printNodeCollection(result, true);

      freeNodeCollection(result);

      freeQuery(query);
    }

    freeXMLDocument(document);
  }

  return 0;
}

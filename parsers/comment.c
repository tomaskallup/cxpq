#include "comment.h"

#include "../util.h"

XMLCommentNode *parseComment(FILE *file) {
  XMLCommentNode *commentNode = (XMLCommentNode *)initNode(COMMENT);
  char firstChar = fgetc(file);

  if (firstChar == '-') {
    printCurrentLineMarked(file);
    PRINT_ERROR("Comments cannot start with `-`\n");
    return false;
  }

  stringAppendChar(commentNode->content, firstChar);

  String *end = stringCreateEmpty();

  char currentChar;
  while ((currentChar = fgetc(file))) {

    if (currentChar == '-' || currentChar == '>') {
      stringAppendChar(end, currentChar);

      if (stringEqualChars(end, "-->")) {
        freeString(end);
        return commentNode;
      }
    } else {
      if (end->length > 0) {
        stringAppend(commentNode->content, end);

        freeString(end);

        end = stringCreateEmpty();
      }

      stringAppendChar(commentNode->content, currentChar);
    }
  }

  printCurrentLineMarked(file);
  PRINT_ERROR("Missing closing tag for comment\n");

  freeString(end);
  freeXMLNode((XMLNode *)commentNode);

  return NULL;
}

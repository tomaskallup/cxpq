#include "node.h"
#include "dtd.h"
#include "element.h"
#include "processing-instruction.h"

#include "../util.h"

XMLNode *parseNode(FILE *file, XMLNode *parent) {
  switch (fgetc(file)) {
  case '!': {
    if (parent != NULL) {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("DTD can only be at the top level of the document\n");

      return NULL;
    }
    PRINT_DEBUG("Found DTD\n");
    return (XMLNode *)parseDTD(file);
  }
  case '?': {
    // Processing instruction or prolog
    PRINT_DEBUG("Found processing instruction\n");
    return (XMLNode *)parseProcessingInstruction(file);
  }
  default: {
    // Element
    fseek(file, -1, SEEK_CUR);
    return (XMLNode *)parseElement(file);
  }
  }
}

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsers/dtd.h"
#include "parsers/node.h"
#include "util.h"

void skipBOM(FILE *file) {
  unsigned char c[4];

  size_t nbytes = fread(c, sizeof(char), sizeof(c), file);

  int to_seek = -1;

  if (nbytes >= 2) {
    if (memcmp(c, "\xFE\xFF", 2) == 0) {
      PRINT_DEBUG("UTF-16BE\n");
      to_seek = nbytes - 2;
    } else if (memcmp(c, "\xFF\xFE", 2) == 0) {
      PRINT_DEBUG("UTF-16LE\n");
      to_seek = nbytes - 2;
    } else if (memcmp(c, "\xFE\xFF", 2) == 0) {
      PRINT_DEBUG("UTF-16BE\n");
      to_seek = nbytes - 2;
    } else if (memcmp(c, "\xFF\xFE", 2) == 0) {
      PRINT_DEBUG("UTF-16LE\n");
      to_seek = nbytes - 2;
    } else if (nbytes >= 3) {
      if (memcmp(c, "\xEF\xBB\xBF", 3) == 0) {
        PRINT_DEBUG("UTF-8\n");
        to_seek = nbytes - 3;
      } else if (nbytes == 4) {
        if (memcmp(c, "\xFF\xFE\x00\x00", 4) == 0) {
          PRINT_DEBUG("UTF-32LE\n");
          to_seek = 0;
        } else if (memcmp(c, "\x00\x00\xFE\xFF", 4) == 0) {
          PRINT_DEBUG("UTF-32BE\n");
          to_seek = 0;
        }
      }
    }
  }

  if (to_seek == -1) {
    PRINT_DEBUG("No BOM detected\n");
    /* Probably not a BOM */
    rewind(file);
  } else if (to_seek > 0)
    fseek(file, -to_seek, SEEK_CUR);
}

XMLDocument *parseXML(FILE *file) {
  skipBOM(file);
  skipWhitespaces(file);

  XMLDocument *document = malloc(sizeof(XMLDocument));
  document->root = NULL;
  document->meta = NULL;
  document->metaSize = 0;

  XMLNode *currentNode = NULL;

  while (fgetc(file) == '<') {
    currentNode = parseNode(file, NULL);

    if (currentNode != NULL && currentNode->type == ELEMENT) {
      XMLElementNode *elementNode = (XMLElementNode *)currentNode;
      if (document->root == NULL) {
        document->root = elementNode;
      } else {
        PRINT_ERROR("Found multiple root elements\n");
        freeXMLTree(currentNode);
      }
    } else {
      document->metaSize += 1;
      document->meta = realloc(document->meta, sizeof(XMLNode *) * document->metaSize);
      document->meta[document->metaSize - 1] = currentNode;
    }

    if (currentNode == NULL)
      break;

    skipWhitespaces(file);
  }

  return document;
}

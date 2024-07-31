#include <stdio.h>

#include "types.h"

XMLNode *parseRootNode(FILE *file);
XMLNode *parseXML(FILE *file);
void printXMLTree(XMLNode *root, int depth);

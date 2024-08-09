#include <stdio.h>

#include "types.h"

XMLDocument *parseXML(FILE *file);
void printXMLTree(XMLNode *root, int depth);

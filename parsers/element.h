#include "../util.h"

bool parseCloseTag(FILE *file, XMLElementNode *node);
bool parseElementContent(FILE *file, XMLElementNode *node);
bool parseAttributeValue(FILE *file, Attribute *attr);
Attribute *parseAttribute(FILE *file);
XMLElementNode *parseElement(FILE *file);

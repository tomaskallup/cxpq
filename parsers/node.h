#include "../util.h"

bool parseCloseNode(FILE *file, XMLNode *node);
bool parseNodeContent(FILE *file, XMLNode *node);
bool parseAttributeValue(FILE *file, Attribute *attr);
Attribute *parseAttribute(FILE *file);
XMLNode *parseNode(FILE *file);

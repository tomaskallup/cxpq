#include "../../../string.h"
#include "../../../types.h"

typedef struct AttributeQueryFixtures {
  String bookString;
  String book2String;
  String langString;
  String enString;
  String czString;
  XMLElementNode book2Node;
  XMLElementNode bookNodeWithChild;
} AttributeQueryFixtures;

AttributeQueryFixtures attributeQueryFixtures = {
    .bookString = (String){"book", 4, 4},
    .book2String = (String){"book2", 5, 5},
    .langString = (String){"lang", 4, 4},
    .enString = (String){"en", 2, 2},
    .czString = (String){"cz", 2, 2},
    .book2Node =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &attributeQueryFixtures.book2String,
            &attributeQueryFixtures.book2String,
            NULL,
            &(Attribute *){&(Attribute){&attributeQueryFixtures.langString,
                                        &attributeQueryFixtures.enString, false}},
            1,
            false,
        },
    .bookNodeWithChild =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &attributeQueryFixtures.bookString,
            &attributeQueryFixtures.bookString,
            &(NodeCollection){
                &(XMLNode *){(XMLNode *)&attributeQueryFixtures.book2Node}, 1, 1},
            &(Attribute *){&(Attribute){&attributeQueryFixtures.langString,
                                        &attributeQueryFixtures.czString, false}},
            1,
            false,
        },
};

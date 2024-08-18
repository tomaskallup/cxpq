#include "../../../string.h"
#include "../../../types.h"

typedef struct ElementNameQueryFixtures {
  String bookString;
  String book2String;
  String wildcardString;
  XMLElementNode bookNode;
  XMLElementNode book2Node;
  XMLElementNode bookNodeWithChild;
  XMLElementNode book2NodeWithNestedBook2Node;
} ElementNameQueryFixtures;

ElementNameQueryFixtures elementNameQueryFixtures = {
    .bookString = (String){"book", 4, 4},
    .book2String = (String){"book2", 5, 5},
    .wildcardString = (String){"*", 1, 1},
    .bookNode =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &elementNameQueryFixtures.bookString,
            &elementNameQueryFixtures.bookString,
            NULL,
            NULL,
            0,
            false,
        },

    .book2Node =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &elementNameQueryFixtures.book2String,
            &elementNameQueryFixtures.book2String,
            NULL,
            NULL,
            0,
            false,
        },
    .book2NodeWithNestedBook2Node =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &elementNameQueryFixtures.book2String,
            &elementNameQueryFixtures.book2String,
            &(NodeCollection){
                &(XMLNode *){(XMLNode *)&elementNameQueryFixtures.book2Node}, 1,
                1},
            NULL,
            0,
            false,
        },
    .bookNodeWithChild =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &elementNameQueryFixtures.bookString,
            &elementNameQueryFixtures.bookString,
            &(NodeCollection){
                &(XMLNode *){(XMLNode *)&elementNameQueryFixtures.book2Node}, 1,
                1},
            NULL,
            0,
            false,
        },
};

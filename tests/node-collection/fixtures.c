#include "../../string.h"
#include "../../types.h"

typedef struct NodeCollectionFixtures {
  String testString;
  String test2String;
  XMLElementNode testNode;
  XMLElementNode testNode2;
  XMLElementNode test2Node;
} NodeCollectionFixtures;


NodeCollectionFixtures nodeCollectionFixtures = {
    .testString = (String){"test", 4, 4},
    .test2String = (String){"test2", 5, 5},
    .testNode =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &nodeCollectionFixtures.testString,
            &nodeCollectionFixtures.testString,
            NULL,
            NULL,
            0,
            false,
        },
    .testNode2 =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &nodeCollectionFixtures.testString,
            &nodeCollectionFixtures.testString,
            NULL,
            NULL,
            0,
            false,
        },
    .test2Node =
        (XMLElementNode){
            ELEMENT,
            NULL,

            &nodeCollectionFixtures.test2String,
            &nodeCollectionFixtures.test2String,
            NULL,
            NULL,
            0,
            false,
        },
};

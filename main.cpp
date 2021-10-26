#include <iostream>
#include "Sexpression.h"
#include <SDocument.h>

int main() {
    std::string str("(hi there (what a cool (little list) parser) (library))");

    auto s = SDocument::parse(str);
    s["hi"]->addChild("str");
    std::cout << s.toString() << std::endl;

    return 0;
}


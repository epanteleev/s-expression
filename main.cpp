#include <iostream>
#include "Sexpression.h"
#include <SDocument.h>

int main() {
    std::string str("(hi there (what a cool (little list) parser) ) (hi library) (to (something))");

    for (int i = 0; i < 10000; i++) {
        auto s = SDocument::parse(str);
        s.findChild("hi")->addChild("str");

        auto response = s.query("/hi/what");
        std::cout << response.front()->toString() << std::endl;
        std::cout << s.toString() << std::endl;

        auto response2 = s.query("[$name=hi]");
        std::cout << response2.front()->toString() << std::endl;
    }
    return 0;
}
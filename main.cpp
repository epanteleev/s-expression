#include <iostream>
#include "Sexpression.h"
#include <SDocument.h>

int main() {
    std::string str("(hi there (what a cool (little list) parser) ) (hi library)  (hi 2e) (to (something))");

    auto s = SDocument::parse(str);
    s.findChild("hi")->addChild("str");

    auto response = s.query("/hi/what");
    std::cout << response.front()->toString() << std::endl;
    std::cout << s.toString() << std::endl;

    auto response2 = s.query("[$range={1, 2}]");
    std::cout << response2.front()->toString() << std::endl;

    std::cout << (*response2.begin() + 1)->toString() << std::endl;

    return 0;
}
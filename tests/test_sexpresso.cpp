#include <string>
#include <SDocument.h>
#include "Sexpression.h"
#include "lest.hpp"

const lest::test specification[] = {
        CASE("Equality") {
            std::string str("(hi there (what a cool (little list) parser) (library))");

            auto s = SDocument::parse(str);

            Sexpression outer = Sexpression::make("hi");
            outer.addChild("there");

            Sexpression what = Sexpression::make("what");
            what.addChild("a");
            what.addChild("cool");

            Sexpression little = Sexpression::make("little");
            little.addChild("list");
            what.addChild(std::move(little));

            what.addChild("parser");

            outer.addChild(std::move(what));

            Sexpression libholder = Sexpression::make("library");

            outer.addChild(std::move(libholder));

            EXPECT(s == outer);
            EXPECT(str == s.toString());
        },
        CASE("Inequality") {
            std::string astr("(this (one is nothing))");
            std::string bstr("(like the (other))");

            auto a = SDocument::parse(astr);
            auto b = SDocument::parse(bstr);
            EXPECT(a != b);
        },
        CASE("Inequality1") {
            std::string astr("(this (one (is) (nothing)))");
            std::string bstr("(this (one (is) (nothing)))");

            auto a = Sexpression::parse(astr);
            auto b = Sexpression::parse(bstr);
            EXPECT(a == b);
        },
        CASE("StringLiteral") {
            auto s = Sexpression::parse("(data \"hello world\" hehe)");
            EXPECT(s.begin()->name() == "hello world");
        },
        CASE("getChild") {
            auto s = Sexpression::parse("(my (a (name me) (age 2)) (b (name you) (age 1)))");
            EXPECT(*s.getChild("a/name") == (Sexpression::parse("(name me)")));
            EXPECT(*s.getChild("a/age") == (Sexpression::parse("(age 2)")));
            EXPECT(*s.getChild("a") == (Sexpression::parse("(a (name me) (age 2))")));

            EXPECT(*s.getChild("b/name") == (Sexpression::parse("(name you)")));
            EXPECT(*s.getChild("b/age") == (Sexpression::parse("(age 1)")));
            EXPECT(*s.getChild("b") == (Sexpression::parse("(b (name you) (age 1))")));

            EXPECT(s.getChild("this/does/not/even/exist/dummy") == nullptr);
        },
        CASE("Argumentiterator") {
            auto s = Sexpression::parse("(hi (data 1 2 3 \"helloo there mate\"; comment\n sup))");
            Sexpression yup = Sexpression::make("data2");
            for (auto &&arg: *s.getChild("data")) {
                yup.addChild(std::move(arg));
            }
            EXPECT(yup.toString() == "(data2 1 2 3 \"helloo there mate\" sup)");
        },
        CASE("CreatePath") {
            Sexpression s1 = Sexpression::make("wow");
            auto pth = std::string{"this/is/cool"};
            auto c = s1.getChild(pth);
            EXPECT(c == nullptr);
            auto& t = s1.createPath(pth);
            EXPECT(s1.toString() == "(wow (this (is (cool))))");
            EXPECT(t == (*s1.getChild(pth)));
        }
};

int main(int argc, char *argv[]) {
    if ( int failures = lest::run( specification, argc, argv ) )
        return failures;

    std::cout << "All tests passed\n", EXIT_SUCCESS;
    return 0;
}
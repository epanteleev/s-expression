#include <string>
#include "sexpresso.hpp"
#include "lest.hpp"

const lest::test specification[] = {
        CASE("EmptyString") {
            std::string str{};
            EXPECT(str.empty());
            auto s = Sexpression::parse(str);

            EXPECT(s.isSexp());
            EXPECT(!s.isString());
            EXPECT(s.m_sexp.empty());
            EXPECT(s.childCount() == 0);
            EXPECT(s.isNil());
        },
        CASE("Empty_root") {
            std::string str("()");
            auto s = Sexpression::parse(str);
            EXPECT(s.isSexp());
            EXPECT(s.m_str.empty());
            EXPECT(s.childCount() == 1);
            EXPECT(s.childCount() == 1);
            EXPECT(s.m_sexp[0].isSexp());
            EXPECT(s.m_sexp[0].m_sexp.empty());
            EXPECT(s.toString() == "()");
        },
        CASE("multipleEmptyRoot") {
            auto str = std::string{"()\n() ()"};
            auto s = Sexpression::parse(str);

            EXPECT(s.childCount() == 3);

            for (auto &&sc: s.m_sexp) {
                EXPECT(sc.isNil());
            }

            EXPECT(s.toString() == "() () ()");
        },
        CASE("Equality") {
            std::string str("hi there (what a cool (little list) parser) (library)");

            auto s = Sexpression::parse(str);

            Sexpression outer{};
            outer.addChild("hi");
            outer.addChild("there");

            Sexpression what{};
            what.addChild("what");
            what.addChild("a");
            what.addChild("cool");

            Sexpression little{};
            little.addChild("little");
            little.addChild("list");
            what.addChild(std::move(little));

            what.addChild("parser");

            outer.addChild(std::move(what));

            Sexpression libholder{};
            libholder.addChild("library");

            outer.addChild(std::move(libholder));

            EXPECT(s == outer);
            EXPECT(str == s.toString());
        },
        CASE("Inequality") {
            std::string astr("this (one is nothing)");
            std::string bstr("like the (other)");

            auto a = Sexpression::parse(astr);
            auto b = Sexpression::parse(bstr);
            EXPECT(a != b);
        },
        CASE("StringLiteral") {
            auto s = Sexpression::parse("\"hello world\" hehe");
            EXPECT(s.m_sexp[0].m_str == "hello world");
        },
        CASE("getChild") {
            auto s = Sexpression::parse("(my (a (name me) (age 2)) (b (name you) (age 1)))");
            EXPECT(*s.getChild("my/a/name") == (Sexpression::parse("name me")));
            EXPECT(*s.getChild("my/a/age") == (Sexpression::parse("age 2")));
            EXPECT(*s.getChild("my/a") == (Sexpression::parse("a (name me) (age 2)")));

            EXPECT(*s.getChild("my/b/name") == (Sexpression::parse("name you")));
            EXPECT(*s.getChild("my/b/age") == (Sexpression::parse("age 1")));
            EXPECT(*s.getChild("my/b") == (Sexpression::parse("b (name you) (age 1)")));

            EXPECT(s.getChild("this/does/not/even/exist/dummy") == nullptr);
        },
        CASE("Argumentiterator") {
            auto s = Sexpression::parse("(hi (data 1 2 3 \"helloo there mate\"; comment\n sup))");
            Sexpression yup{};
            for (auto &&arg: *s.getChild("hi/data")) {
                yup.addChild(std::move(arg));
            }
            EXPECT(yup.toString() == "1 2 3 \"helloo there mate\" sup");
        },
        CASE("CreatePath") {
            Sexpression s1{};
            auto pth = std::string{"wow/this/is/cool"};
            auto c = s1.getChild(pth);
            EXPECT(c == nullptr);
            c = &(s1.createPath(pth));
            EXPECT(s1.toString() == "(wow (this (is (cool))))");
            EXPECT(c == (s1.getChild(pth)));
        },
        CASE("AddExpression") {
            Sexpression s{};
            auto &p = s.createPath("oh/my/god");
            p.addExpression("(r 0) (g 0) (b 23)");
            auto b = s.getChild("oh/my/god/b");
            EXPECT(b != nullptr);
            EXPECT(b->toString() == "b 23");
            EXPECT(s.toString() == "(oh (my (god (r 0) (g 0) (b 23))))");
        },
        CASE("invalidGetChildByPath") {
            Sexpression s{};
            auto &p = s.createPath("a/b/c");
            p.addExpression("(r 0) (g 0) (b 23)");
            auto y = s.getChild("a/b/c/y");
            EXPECT(y == nullptr);

            auto d = s.getChild("a/b/c/y/b");
            EXPECT(d == nullptr);
        },
        CASE("toStringwithcomma") {
            std::string str("(a ((b ,(c d))))");
            auto s = Sexpression::parse(str);
            EXPECT(s.toString() == "(a ((b , (c d))))");
        },
        CASE("incorrectSyntax") {
            EXPECT_THROWS(Sexpression::parse("(((todo))"));
            EXPECT_THROWS(Sexpression::parse("((td)))"));

            EXPECT_THROWS(Sexpression::parse("(((\"abs))"));
        }
};

int main(int argc, char *argv[]) {
    return lest::run(specification, argc, argv);
}
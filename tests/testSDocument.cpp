#define lest_FEATURE_AUTO_REGISTER 1
#include <detail/Lexer.h>
#include <SDocument.h>
#include "lest.hpp"

#define CASE( name ) lest_CASE( testlist, name )
#undef SCENARIO
#define SCENARIO( sketch ) lest_SCENARIO(testlist, sketch )

static lest::tests testlist;

CASE("Empty_root") {
    EXPECT_THROWS(SDocument::parse("()"));
}
CASE("multipleEmptyRoot") {
    EXPECT_THROWS(SDocument::parse("()\n() ()"));
}

CASE("message with comma") {
    std::string str("(a ((b ,(c d))))");
    EXPECT_THROWS(SDocument::parse(str));
}

CASE("incorrectSyntax") {
    EXPECT_THROWS(SDocument::parse("(((todo))"));
    EXPECT_THROWS(SDocument::parse("((td)))"));

    EXPECT_THROWS(SDocument::parse("(((\"abs))"));
}

CASE("string sugar") {
    SDocument doc = "(a (a1 ee) (a2 bb))"_s;
    EXPECT(doc.findChild("a") != doc.end());
}

CASE("parse with comment") {
    auto doc = SDocument::parse("(a \n\n fffd dfgd) (b sdf ffd) ; todo \n");
    EXPECT(doc.findChild("a") != doc.end());
    EXPECT(doc.findChild("b") != doc.end());
    EXPECT(doc.findChild("bc") == doc.end());
}

CASE("parse '\\n' in string end") {
    std::string str("(a (r (c d)))\n");
    EXPECT_NO_THROW(SDocument::parse(str));
}

int main(int argc, char *argv[]) {
    if (int failures = lest::run(testlist, argc, argv))
        return failures;

    std::cout << "All tests passed\n", EXIT_SUCCESS;
    return 0;
}
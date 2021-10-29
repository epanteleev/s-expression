#define lest_FEATURE_AUTO_REGISTER 1
#include <Lexer.h>
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
SCENARIO("trivial s-document") {
    GIVEN("parse trivial string") {
        const auto str = "(a1 (r 32) (g 77) (b 127))"
                         "(a2 (r 62) (g 67) (b 627))";
        auto doc = SDocument::parse(str);
        WHEN("check s expressions") {
            const auto a1 = doc["a1"];
            const auto a2 = doc["a2"];
            THEN("check a1") {
                EXPECT(a1->is<SKind::SEXP>());
                EXPECT(a1->name() == "a1");
            }
            THEN("check a2") {
                EXPECT(a2->is<SKind::SEXP>());
                EXPECT(a2->name() == "a2");
            }
        }
        WHEN("check iterators") {
            const auto a1 = doc.begin();
            const auto a2 = doc.begin() + 1;
                THEN("check a1") {
                    EXPECT(a1->is<SKind::SEXP>());
                    EXPECT(a1->name() == "a1");
                }
                THEN("check a2") {
                    EXPECT(a2->is<SKind::SEXP>());
                    EXPECT(a2->name() == "a2");
                }
        }
    }
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

int main(int argc, char *argv[]) {
    if (int failures = lest::run(testlist, argc, argv))
        return failures;

    std::cout << "All tests passed\n", EXIT_SUCCESS;
    return 0;
}
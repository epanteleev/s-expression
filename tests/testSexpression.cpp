#define lest_FEATURE_AUTO_REGISTER 1
#include <SDocument.h>
#include "lest.hpp"

#define CASE( name ) lest_CASE( testlist, name )
#undef SCENARIO
#define SCENARIO( sketch ) lest_SCENARIO(testlist, sketch )

static lest::tests testlist;

CASE("Create simple s-expr") {
    auto s1 = Sexpression::make("a1");
    EXPECT(s1.toString() == "(a1)");
}

CASE("Create s-expr") {
    SDocument doc{};

    auto s1 = Sexpression::make("a1");
    EXPECT(s1.name() == "a1");
    s1.addChild("1");
    s1.addChild("2");
    EXPECT(s1.toString() == "(a1 1 2)");
}

CASE("Create s-expr 2") {
    SDocument doc{};

    auto s1 = Sexpression::make("a1");
    s1["b1"]["c1"].addChild("1");
    EXPECT(s1.toString() == "(a1 (b1 (c1 1)))");
}

CASE("Create s-expr 3") {
    SDocument doc{};

    auto s1 = Sexpression::make("a1");
    auto s2 = Sexpression::make("b1");
    s2.addChild("1");
    s1.addChild(std::move(s2));

    EXPECT(s1.toString() == "(a1 (b1 1))");
}

int main(int argc, char *argv[]) {
    if (int failures = lest::run(testlist, argc, argv))
        return failures;

    std::cout << "All tests passed\n", EXIT_SUCCESS;
    return 0;
}
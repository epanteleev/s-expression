#include <detail/Lexer.h>
#include "lest.hpp"

using namespace detail::lexer;

const lest::test testlist[] = {
    SCENARIO("correct simple") {
        GIVEN("String") {
            Lexer lex("(\n)");

            WHEN("first paren") {
                THEN("expect paren") {
                    EXPECT(lex.is<Tok::OPEN_PAREN>());
                    EXPECT(lex.lines() == 0);
                    EXPECT(lex.pos() == 1);
                    lex.skipSpaces();
                    EXPECT(lex.is<Tok::CLOSE_PAREN>());
                    EXPECT(lex.lines() == 1);
                    EXPECT(lex.pos() == 1);
                    EXPECT(!lex.is<Tok::OPEN_PAREN>());
                }
            }
        }
    },
    SCENARIO("correct simple with escape sequence") {
        GIVEN("String") {
            Lexer lex(R"(("\n"))");
            WHEN("first paren") {
                THEN("expect paren") {
                    EXPECT(lex.is<Tok::OPEN_PAREN>());
                    EXPECT(lex.lines() == 0);
                    EXPECT(lex.pos() == 1);
                    EXPECT(lex.is<Tok::STRING_LITERAL>());
                    EXPECT(lex.peek<Tok::STRING_LITERAL>() == "\n");
                    EXPECT(lex.lines() == 0);
                    EXPECT(lex.pos() == 5);
                    EXPECT(!lex.eof());
                    EXPECT(lex.is<Tok::CLOSE_PAREN>());
                    EXPECT(lex.lines() == 0);
                    EXPECT(lex.pos() == 6);
                }
            }
        }
    },
    SCENARIO("semicolon correct handle") {
        GIVEN( "String with ';'" ) {
            Lexer lex("abc;dac\n(ss)");
            const auto str = lex.peek<Tok::STRING>();
            WHEN("String 'abc'") {
                THEN("'abc") {
                    EXPECT(str == "abc");
                    EXPECT(lex.is<Tok::SEMICOLON>());
                }
                WHEN("String 'dac'") {
                    THEN("'dac'") {
                        EXPECT(lex.peek<Tok::STRING>() == "dac");
                        EXPECT(lex.lines() == 0);
                        EXPECT(lex.pos() == 7);
                    }
                }
            }
            WHEN("is comment") {
                lex.skipComment();
                THEN("correct line is pos") {
                    EXPECT(lex.lines() == 1);
                    EXPECT(lex.pos() == 0);
                }
            }
        }
    },
    CASE("strings seq 0") {
        Lexer lex("\"abc\" dac");
        EXPECT(lex.is<Tok::STRING_LITERAL>());
        EXPECT(lex.peek<Tok::STRING_LITERAL>() == "abc");
        lex.skipSpaces();
        EXPECT(lex.is<Tok::STRING>());
    },
    CASE("strings seq 1") {
        Lexer lex("\"abc\"dac");
        EXPECT(lex.peek<Tok::STRING_LITERAL>() == "abc");
        EXPECT(lex.is<Tok::STRING>());
    },
    CASE("strings seq 2") {
        Lexer lex("abc\"dac\"");
        EXPECT(lex.peek<Tok::STRING>() == "abc");
        EXPECT(lex.is<Tok::STRING_LITERAL>());
    },
    CASE("strings seq 3") {
        Lexer lex("abc;dac");
        EXPECT(lex.peek<Tok::STRING>() == "abc");
        EXPECT(lex.is<Tok::SEMICOLON>());
        EXPECT(lex.peek<Tok::STRING>() == "dac");
    },
    CASE("strings seq 4") {
        Lexer lex("/abc/dac/cas");
        EXPECT(lex.is<Tok::SLASH>());
        EXPECT(lex.peek<Tok::STRING>() == "abc");
        EXPECT(lex.is<Tok::SLASH>());
        EXPECT(lex.peek<Tok::STRING>() == "dac");
        EXPECT(lex.is<Tok::SLASH>());

        EXPECT(lex.peek<Tok::STRING>() == "cas");
    },
    CASE("parse integer") {
        Lexer lex("123");
        EXPECT(lex.is<Tok::INTEGER>());
        EXPECT(lex.peek<Tok::INTEGER>() == 123);
        EXPECT(lex.lines() == 0);
        EXPECT(lex.pos() == 3);
    },
    CASE("parse negative integer") {
        Lexer lex("-321");
        EXPECT(lex.is<Tok::INTEGER>());
        EXPECT(lex.peek<Tok::INTEGER>() == -321);
        EXPECT(lex.lines() == 0);
        EXPECT(lex.pos() == 4);
    },
    CASE("no integer") {
        Lexer lex("-321sdf");
        EXPECT(!lex.is<Tok::INTEGER>());
    }
};

int main(int argc, char *argv[]) {
    if ( int failures = lest::run( testlist, argc, argv ) )
        return failures;

    std::cout << "All tests passed\n", EXIT_SUCCESS;
    return 0;
}
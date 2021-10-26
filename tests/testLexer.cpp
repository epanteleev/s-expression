#include <Lexer.h>
#include "lest.hpp"


const lest::test testlist[] = {
    SCENARIO("correct simple") {
        GIVEN("String") {
            Lexer lex("(\n)");

            WHEN("first paren") {
                THEN("expect paren") {
                    EXPECT(lex.is<Tok::OPEN_PAREN>());
                    EXPECT(lex.lines() == 0);
                    EXPECT(lex.pos() == 0);
                }
            }
            WHEN("second paren") {
                lex.get();
                lex.skipSpaces();
                THEN("expect close paren") {
                    EXPECT(lex.is<Tok::CLOSE_PAREN>());
                    EXPECT(lex.lines() == 1);
                    EXPECT(lex.pos() == 0);
                }
                THEN("incorrect") {
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
                    EXPECT(lex.pos() == 0);
                }
                WHEN("get string") {
                    lex.get();
                    THEN("line break") {
                        EXPECT(lex.is<Tok::STRING_LITERAL>());
                        EXPECT(lex.peek<Tok::STRING_LITERAL>() == "\n");
                        EXPECT(lex.lines() == 0);
                        EXPECT(lex.pos() == 5);
                    }
                }
                WHEN("close paren") {
                    THEN("close paren") {
                        EXPECT(!lex.eof());
                        EXPECT(lex.is<Tok::CLOSE_PAREN>());
                        EXPECT(lex.lines() == 0);
                        EXPECT(lex.pos() == 5);
                    }
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
                    lex.get();
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
        lex.get();
        EXPECT(lex.peek<Tok::STRING>() == "dac");
    },
    CASE("strings seq 4") {
        Lexer lex("/abc/dac/cas");
        EXPECT(lex.is<Tok::SLASH>());
        lex.get();
        EXPECT(lex.peek<Tok::STRING>() == "abc");
        EXPECT(lex.is<Tok::SLASH>());
        lex.get();
        EXPECT(lex.peek<Tok::STRING>() == "dac");
        EXPECT(lex.is<Tok::SLASH>());
        lex.get();

        EXPECT(lex.peek<Tok::STRING>() == "cas");
    },
};

int main(int argc, char *argv[]) {
    if ( int failures = lest::run( testlist, argc, argv ) )
        return failures;

    std::cout << "All tests passed\n", EXIT_SUCCESS;
    return 0;
}
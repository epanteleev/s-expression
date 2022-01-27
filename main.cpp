#include <iostream>
#include "Sexpression.h"
#include <SDocument.h>

int main(int args, char** argv) {
    if (args < 2) {
        std::cerr << "Usage: " <<
            std::endl << argv[0] << " <s-expr file> " << std::endl;
        return 1;
    }
    std::filesystem::path path = argv[1];
    auto s = SDocument::load(path);
    std::cout << "Load successfully.\n\n" << s.dump() << std::endl;

    while (!std::cin.eof()) {
        std::string query;

        std::cout <<">> ";
        std::getline(std::cin, query, '\n');

        std::cout <<"query: " << query << std::endl;
        try {
            auto responce = s.query(query);
            if (responce.empty()) {
                std::cout << "<empty>" << std::endl;

            } else {
                for (std::size_t n = 0; auto& i: responce) {
                    std::cout << '[' << n << "] " << i->toString() << std::endl;
                    n += 1;
                }
            }
        } catch (std::runtime_error& error) {
            std::cerr <<"Error detected: " << error.what() << std::endl;
        }

    }
    return 0;
}
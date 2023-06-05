//
//  LexicalAnalyzer.h
//  Compiler_lab01
//
//  Created by Blotenko on 25.04.2023.
//

#ifndef LexicalAnalyzer_h
#define LexicalAnalyzer_h

#include <stdio.h>
#include <vector>
#include <string>

#include "Token.hpp"


class LexicalAnalyzer {
public:
    LexicalAnalyzer();
    virtual ~LexicalAnalyzer();
    std::vector<Token> tokenize(const std::string str);
    std::vector<Token> analyze(std::string code);
    std::vector<Token> getTokens();
    void setTokens(std::vector<Token> tk);
    std::vector<Range> getTokensByType(TokenType type);
    void writeColoredTextToFile(const std::string& text, const std::vector<Range>& ranges, const std::string& filename);
private:
    std::vector<Token> tokens;
};

#endif /* LexicalAnalyzer_h */

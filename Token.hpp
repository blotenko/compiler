//
//  Token.hpp
//  Compiler_lab01
//
//  Created by Blotenko on 19.05.2023.
//

#ifndef Token_hpp
#define Token_hpp

#include <stdio.h>
#include <iostream>

enum class TokenType {
    Identifier,
    Keyword,
    Operator,
    NumericLiteral,
    StringLiteral,
    SymbolicConstant,
    Punctuation,
    PreprocessorDirective,
    Comment,
    Error 
};


struct Range
{
    int location;
    int lenght;
    Range(size_t loc, unsigned long len) : location((int)loc), lenght((int)len) {}
};


struct Token {
    TokenType type;
    std::string value;
    size_t line;
    Range range;
};




#endif /* Token_hpp */

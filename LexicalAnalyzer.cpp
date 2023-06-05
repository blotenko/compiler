//
//  LexicalAnalyzer.c
//  Compiler_lab01
//
//  Created by Blotenko on 25.04.2023.
//

#include "LexicalAnalyzer.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <fstream>

LexicalAnalyzer::LexicalAnalyzer() {
}

LexicalAnalyzer::~LexicalAnalyzer() {
}

std::string tokenTypeToString(TokenType type)
{
    switch (type) {
        case TokenType::Identifier:
            return "Identifier";
            break;
        case TokenType::Keyword:
            return "Keyword";
            break;
        case TokenType::Operator:
            return "Operator";
            break;
        case TokenType::NumericLiteral:
            return "NumericLiteral";
            break;
        case TokenType::StringLiteral:
            return "StringLiteral";
            break;
        case TokenType::SymbolicConstant:
            return "SymbolicConstant";
            break;
        case TokenType::Punctuation:
            return "Punctuation";
            break;
        case TokenType::PreprocessorDirective:
            return "PreprocessorDirective";
            break;
        case TokenType::Comment:
            return "Comment";
            break;
            
        default:
            return "Error";
            break;
    }
}

bool isOperator(const std::string& tokenStr) {
    static const std::set<std::string> operators = {
        "+", "-", "*", "%", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "|=", "^=", "&&=", "||=", "."
    };

    return operators.find(tokenStr) != operators.end();
}

const std::set<std::string> keywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int",
    "long", "register", "return", "short", "signed", "sizeof", "static", "struct",
    "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
    "self", "super", "synthesize", "nonatomic", "retain", "strong",
    "weak", "copy", "readonly", "readwrite", "class", "main", "init",
    "protocol",  "import", "include", "nullable", "instancetype", "assign", "NSString", "NSInteger", "alloc", "NSLog", "NSObject"
};

const std::set<std::string> prepDer = {
    "#import", "#ifdef", "#define", "#pragma", "#ifndef", "#if", "#else", "#elif", "#endif", "@property", "interface", "end", "implementation", "synthesize", "dynamic", "protocol", "class", "import", "property", "autoreleasepool"
};

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isIdentifierChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || isDigit(c);
}

bool isNumericLiteral(const std::string& str) {
    bool hasDigit = false;
    bool hasDecimalPoint = false;
    for (char c : str) {
        if (isDigit(c)) {
            hasDigit = true;
        } else if (c == '.' && !hasDecimalPoint) {
            hasDecimalPoint = true;
        } else {
            return false;
        }
    }
    return hasDigit;
}

bool isStringLiteral(const std::string& str) {
    return str.size() >= 2 && str.front() == '@'&& str[1] == '"' && str.back() == '"';
}


bool isSymbolicConstant(const std::string& str) {
    return str.size() >= 2 && str.front() == '@' && str.back() != '@' && !isNumericLiteral(str.substr(1));
}

bool isPunctuation(std::string c) {
    if (c.size() > 1)
    {
        return false;
    }
    char ch = c[0];
    return std::ispunct(ch) && !isOperator(c) && ch != '"' && ch != '/' && ch != '#';
}

bool isValidIdentifier(const std::string& str) {
    if (str.empty() || std::isdigit(str[0])) {
        return false;
    }
    for (char c : str) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    return true;
}


TokenType getTokenType(const std::string& tokenStr) {

    if (keywords.find(tokenStr) != keywords.end()) {
        return TokenType::Keyword;
    }
    
    if (isOperator(tokenStr)) {
        return TokenType::Operator;
    }

    if (isNumericLiteral(tokenStr)) {
        return TokenType::NumericLiteral;
    }

    if (isStringLiteral(tokenStr)) {
        return TokenType::StringLiteral;
    }
    
    if (prepDer.find(tokenStr) != prepDer.end()) {
        return TokenType::PreprocessorDirective;
    }

    if (isSymbolicConstant(tokenStr)) {
        return TokenType::SymbolicConstant;
    }

    if (isPunctuation(tokenStr)) {
        return TokenType::Punctuation;
    }

    if (isValidIdentifier(tokenStr)) {
        return TokenType::Identifier;
    }

    return TokenType::Error;
}

std::vector<Token> LexicalAnalyzer::tokenize(const std::string str) {
    std::vector<Token> tokens;
    std::string currentToken;
    size_t line = 1;
    std::string stringLiterals;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        
        if (c == '\n') {
            if (!currentToken.empty()) {
                
                tokens.push_back({getTokenType(currentToken), currentToken, line, Range(i-currentToken.length(), currentToken.length())});
                currentToken.clear();
            }
            line++;
        }
         else if (isspace(c) || c == ' ') {
            if (!currentToken.empty()) {
                tokens.push_back({getTokenType(currentToken), currentToken, line, Range(i-currentToken.length(), currentToken.length())});
                //tokens.push_back({TokenType::Punctuation, std::string(1,' '), line});
                currentToken.clear();
            }
     
        } else if (isOperator(std::string(1,c)) && currentToken != "/" ) {
            if ( !tokens.empty() && tokens.back().value == "<")
            {
                tokens.push_back({TokenType::Identifier, currentToken, line, Range(i-currentToken.length(), currentToken.length())});
                currentToken.clear();
            }
           else if (!currentToken.empty()) {
                tokens.push_back({getTokenType(currentToken), currentToken, line, Range(i-currentToken.length(), currentToken.length())});
                currentToken.clear();
            }

            // Handle multi-character operators
            if (i < str.length() - 1 && isOperator(std::string(1,str[i + 1]))) {
                std::string op(1, c);
                op += str[i + 1];
                tokens.push_back({getTokenType(op), op, line, Range(i, op.length())});
                i++;
            } else {
                tokens.push_back({getTokenType(std::string(1, c)), std::string(1, c), line, Range(i,1)});
            }
        }
        else if (isPunctuation(std::string(1,c)) && c != '.'){
            if (currentToken.empty()) {
                currentToken += c;
                 tokens.push_back({getTokenType(currentToken), currentToken, line, Range(i, currentToken.length())});
                 currentToken.clear();
                //continue;
             }
            else
            {
                tokens.push_back({getTokenType(currentToken), currentToken, line, Range(i-currentToken.length(), currentToken.length())});
                currentToken.clear();
                tokens.push_back({getTokenType(std::string(1,c)), std::string(1,c), line, Range(i, 1)});
              // continue;
            }
        } else if (c == '"' || c == '\'') {
           
            currentToken.clear();
            std::string literal = "";
            literal += "'";
            i++;
            while (i < str.length() && str[i] != c) {
                if (str[i] == '\\') {
                    if (i == str.length() - 1) {
                        tokens.push_back({TokenType::Error, "Unterminated escape sequence", line, Range(0, 0)});
                        return tokens;
                    }

                    literal += str[i];
                    i++;
                }
                literal += str[i];
                i++;
            }
            if (i == str.length()) {
                tokens.push_back({TokenType::Error, "Unterminated string or character literal", line, Range(0, 0)});
                return tokens;
            }
            literal += c;
            tokens.push_back({TokenType::StringLiteral, literal, line, Range(i-literal.length(), literal.length())});
            
        }
        else if(c == '/')
        {
           
             if (currentToken == "/"){
                currentToken.clear();
                std::string literal = "//";
                i++;
                while (i < str.length() && str[i] != '\n') {
                    if (str[i] == '\\') {
                        if (i == str.length() - 1) {
                            tokens.push_back({TokenType::Error, "Unterminated escape sequence", line, Range(0,0)});
                            return tokens;
                        }

                        literal += str[i];
                        i++;
                    }
                    literal += str[i];
                    i++;
                }
                //literal += c;
                tokens.push_back({TokenType::Comment, literal, line, Range(i-literal.length(), literal.length())});
            }
             else
             {
                 currentToken += c;
             }
        }
        else if(c == '*' && currentToken == "/")
        {
            currentToken.clear();
            std::string literal = "/*";
            i++;
            while (i < str.length() && (str[i] != '*' && str[i+1] != '/')) {
                if (str[i] == '\\') {
                    if (i == str.length() - 1) {
                        tokens.push_back({TokenType::Error, "Unterminated escape sequence", line, Range(0,0)});
                        return tokens;
                    }

                    literal += str[i];
                    i++;
                }
                literal += str[i];
                i++;
            }
            i++;
            literal += "*/";
            tokens.push_back({TokenType::Comment, literal, line, Range(i-literal.length(), literal.length())});
     
        }
        else if(c == '#' && currentToken.empty())
        {
            std::string literal = "#";
            i++;
            while (i < str.length() && (str[i] != ' ')) {
                literal += str[i];
                i++;
            }
            if (prepDer.find(literal) != prepDer.end()) {
                tokens.push_back({TokenType::PreprocessorDirective, literal, line, Range(i-literal.length(), literal.length())});
            }
            //i++;
        }
        else {
            currentToken += c;
        }
        
    }
    if (!currentToken.empty()) {
        tokens.push_back({getTokenType(currentToken), currentToken, line, Range(str.length() - currentToken.length(), currentToken.length())});
    }

    return tokens;
}

std::vector<Token> LexicalAnalyzer::analyze(std::string code) {
    auto tokens = tokenize(code);

    std::cout<< "Token\t\tType\t\tValue\tLine\n";
    for (const auto& token : tokens) {
        if (token.value != " ")
        {
            std::cout<< "\t" << tokenTypeToString(token.type) << "\t\t" << token.value << "\t\t" << token.line << "\n";
        }
    }

    return tokens;
}

std::vector<Token> LexicalAnalyzer::getTokens() {
    return tokens;
}

void LexicalAnalyzer::setTokens(std::vector<Token> tk) {
    tokens = tk;
}

std::vector<Range> LexicalAnalyzer::getTokensByType(TokenType type)
{
    std::cout<<std::endl;
    std::cout<<"Token by type -----------  " + tokenTypeToString(type)<< std::endl;
    std::cout<<std::endl;
    
    std::vector<Range> resultVector;
    for (const auto& token : getTokens())
    {
        if (token.type == type)
        {
            std::cout<< "\t" << tokenTypeToString(token.type) << "\t\t" << token.value << "\t\t" << token.line << "\n";
            resultVector.push_back(token.range);
        }
    }
    return resultVector;
}


void LexicalAnalyzer::writeColoredTextToFile(const std::string& text, const std::vector<Range>& ranges, const std::string& filename) {
    std::ofstream outputFile(filename);

    int currentPosition = 0;
    outputFile << "<html><body><pre>";

    for (const auto& range : ranges) {

        outputFile << text.substr(currentPosition, range.location - currentPosition);

        outputFile << "<span style=\"color: green\">" << text.substr(range.location, range.lenght) << "</span>";

        currentPosition = range.location + range.lenght;
    }

    outputFile << text.substr(currentPosition);

    outputFile << "</pre></body></html>";

    outputFile.close();
}



//
//  SyntaxAnalizer.hpp
//  Compiler_lab01
//
//  Created by Blotenko on 19.05.2023.
//

#ifndef SyntaxAnalizer_hpp
#define SyntaxAnalizer_hpp

#include <stdio.h>
#include <iostream>
#include "Token.hpp"
#include <vector>

struct Node {
    std::string name;
    std::vector<Node*> children;
};

class SyntaxAnalyzer {
public:
    
    SyntaxAnalyzer(const std::vector<Token>& tkns);
    
    
    bool parseInterfaceDeclaration(Node* node, int& currentIndex);
    
    bool parsePropertyDeclaration(Node* parentNode, int& currentIndex);
    
    bool parseFullMethodDeclaration02(Node* parentNode, int& currentIndex);

    bool parseFullMethodDeclaration(Node* parentNode, int& currentIndex);
    
    bool parseMethodDeclaration(Node* parentNode, int& currentIndex);
    
    Node* parseDeclaration();
    
    bool parseImplementationDeclaration(Node* node, int& currentIndex);

private:
    std::vector<Token> tokens;
    int currentIndex;
    Node* root;
};

#endif /* SyntaxAnalizer_hpp */

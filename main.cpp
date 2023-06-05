//
//  main.cpp
//  Compiler_lab01
//
//  Created by Blotenko on 25.04.2023.
//

#include <iostream>
#include <vector>
#include "LexicalAnalyzer.h"
#include "SyntaxAnalizer.hpp"
#include <fstream>

void print(Node* root, std::ofstream& outputFile, int indentationLevel = 0) {
    if (root == nullptr) {
         return;
     }
     
     // Add indentation based on the level in the tree
     for (int i = 0; i < indentationLevel; i++) {
         outputFile << "&nbsp;&nbsp;&nbsp;&nbsp;";
     }
     
     outputFile << root->name;
     
     if (!root->children.empty()) {
         outputFile << "<br>";
         outputFile << " {" << "<br>";
         for (Node* child : root->children) {
             // Increase the indentation level for child nodes
             print(child, outputFile, indentationLevel + 1);
         }
         
         // Add indentation before the closing brace
         for (int i = 0; i < indentationLevel; i++) {
             outputFile << "&nbsp;&nbsp;&nbsp;&nbsp;";
         }
         
         outputFile << "}" << "<br>";
     } else {
         outputFile << "<br>";
     }
}

int main(int argc, const char * argv[]) {
   
    
    std::string s;
    std::string sTotal;

    std::ifstream in;
    in.open("/Users/blotenko/Downloads/sourceCodeForTesting.txt");

    while(!in.eof()) {
        getline(in, s);
        sTotal += s + "\n";
        
    }
    in.close();
    
    LexicalAnalyzer lexer;
    std::vector<Token> tokens = lexer.analyze(sTotal);
    lexer.setTokens(tokens);
    
    /*
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
     */
    
    lexer.writeColoredTextToFile(sTotal, lexer.getTokensByType(TokenType::Keyword), "/Users/blotenko/Desktop/output.html");
    

    LexicalAnalyzer lexer2;
    SyntaxAnalyzer analyzer(lexer2.analyze(sTotal));
    Node* astRoot = analyzer.parseDeclaration();
    
    std::ofstream outputFile("/Users/blotenko/Desktop/output.html");

    print(astRoot, outputFile);
//    if (astRoot)
//    {
//        std::cout << "Root Node: " << astRoot->name << std::endl;
//        for (const auto& child : astRoot->children)
//        {
//            std::cout << "Child Node: " << child->name << std::endl;
//        }
//    }

    return 0;
}



/*
 если нашли ошибку то переходим на новую строку
 
 
 
 -InterfaceDeclaration - (RootNode)
    только пробелы
     Identifier: "Person"
        пробелы и :
     Superclass: "NSObject"
        до конца строки могут быть пробелы <> , другие keyObject
 
     -DeclarationList
        -PropertyDeclaration
         Identifier: "name"
         Type: "NSString"
         Attribute: "nonatomic, strong"
        -PropertyDeclaration
         Identifier: "age"
         Type: "NSInteger"
         Attribute: "nonatomic, assign"
        -MethodDeclaration
         ReturnType: "void"
         Selector: "sayHello"
         ParameterList: None
 
 
 
 
 
 
 Child Node: InterfaceDeclaration
 Root Node: InterfaceDeclaration
 Child Node: Person
 Child Node: NSObject
 Child Node: property
 Root Node: property
 Child Node: nonatomic attr
 Child Node: strong attr
 Child Node: NSString type
 Child Node: name
 Child Node: nonatomic
 Child Node: assign
 Child Node: NSInteger
 
 
 
 -ImplementationDeclaration
     Identifier: "Person"
     DeclarationList
        -MethodDeclaration
             ReturnType: "instancetype"
             Selector: "initWithName:age:"
             ParameterList
                 -ParameterDeclaration
                 Identifier: "name"
                 Type: "NSString"
                 -ParameterDeclaration
                 Identifier: "age"
                 Type: "NSInteger"
 
 
 
 
 -InterfaceDeclaration -> Identifier + Superclass +(if has DeclarationList) + @end
 
 
 
 -ImplementationDeclaration -> Identifier +(if has DeclarationList) + @end !!!!!
 
 
 - PropertyDeclaration -> Identifier + Type + (is has Attribute)
 
 - "-"/"+"/keyword then "()"/keyword MethodDeclaration -> ReturnType + Selector + (is has ParameterList) // тут сложнее
 
 -ParameterDeclaration -> Identifier + Type
 
 
 
 */

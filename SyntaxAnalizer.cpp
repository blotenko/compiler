//
//  SyntaxAnalizer.cpp
//  Compiler_lab01
//
//  Created by Blotenko on 19.05.2023.
//

#include "SyntaxAnalizer.hpp"
#include <fstream>
#include <sstream>



SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tkns)
{
    tokens = tkns;
    currentIndex = 0;
    if (currentIndex < tokens.size())
    {
        root = new Node();
       
    }
}

Node* makeErrorNode(std::string errorName)
{
    Node* errorNode = new Node();
    errorNode->name = errorName;
    return errorNode;
}

bool SyntaxAnalyzer::parsePropertyDeclaration(Node* parentNode, int& currentIndex)
{
    Token token = tokens[currentIndex];
    Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
    int countForComplite = 0;
  
    while (currentIndex < tokens.size() && (token.value != ";") && (token.value != "-") && (tokens[currentIndex].type != TokenType::PreprocessorDirective))
    {
         if (token.type == TokenType::Keyword && (lastToken.value == "(" || lastToken.value == ","))
        {
            Node* attrNode = new Node();
            attrNode->name = token.value + " attr";
            parentNode->children.push_back(attrNode);
        }
        else if (token.type == TokenType::Keyword && lastToken.value == ")")
        {
            Node* typeNode = new Node();
            typeNode->name = token.value + " type";
            parentNode->children.push_back(typeNode);
            countForComplite++;
        }
        else if (token.type == TokenType::Identifier)
        {
            Node* idNode = new Node();
            idNode->name = token.value;
            parentNode->children.push_back(idNode);
            countForComplite++;
        }
        lastToken = token;
        token = tokens[++currentIndex];
    }
    if (countForComplite!=2)
    {
        parentNode->children.push_back(makeErrorNode("Not all elements for propery declaration"));
    }
        
    if ((tokens[currentIndex].value != ";" && currentIndex+1 == tokens.size()) || (tokens[currentIndex].value == "-") || (tokens[currentIndex].type == TokenType::PreprocessorDirective))
    {
        parentNode->children.push_back(makeErrorNode("No ; for propery declaration"));
        currentIndex--;
    }
    return true;
}

bool SyntaxAnalyzer::parseFullMethodDeclaration02(Node* methodNode, int& currentIndex) {
    Token token = tokens[++currentIndex];
    Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
    int countForCompile = 0;
    int countForSelector = 0;
    int countForId = 0;
  
    if (token.type == TokenType::Keyword)
    {
        Node* returnTypeNode = new Node();
        returnTypeNode->name = "ReturnType";
        Node* node = new Node();
        node->name = token.value;
        returnTypeNode->children.push_back(node);
        methodNode->children.push_back(returnTypeNode);
        countForCompile++;
    }
    else
    {
        methodNode->children.push_back(makeErrorNode("Not ReturnType for method declaration"));
    }
    currentIndex++;
    
    while (currentIndex < tokens.size() && (token.value != "{"))
    {
        //Selector
        if (token.type == TokenType::Identifier && countForCompile && tokens[currentIndex+1].value == ":")
        {
            Node* selectorNode = new Node();
            selectorNode->name = "selector";
            Node* node = new Node();
            node->name = token.value;
            selectorNode->children.push_back(node);
            methodNode->children.push_back(selectorNode);
            countForSelector++;
        }
        else if (token.type == TokenType::Identifier && !countForCompile && tokens[currentIndex+1].value == ":")
        {
            methodNode->children.push_back(makeErrorNode("Erorr in method declaration"));
        }
         else if (token.type == TokenType::Identifier) {
            Node* selectorNode = new Node();
            selectorNode->name = "Identifier";
            Node* node = new Node();
            node->name = token.value;
            selectorNode->children.push_back(node);
            methodNode->children.push_back(selectorNode);
             countForId++;
        }
        
        else if (token.value == "(" && !methodNode->children.empty()) {
            currentIndex++;
            token = tokens[currentIndex];
            if (token.value == ")") {
                // No parameters
                currentIndex++;
            } else {
                Node* parameterListNode = new Node();
                parameterListNode->name = "ParameterList";
                methodNode->children.push_back(parameterListNode);
                
                while (token.value != ")") {
                    if (token.type == TokenType::Keyword) {
                        Node* parameterNode = new Node();
                        parameterNode->name = "Parameter";
                        Node* node = new Node();
                        node->name = token.value;
                        parameterNode->children.push_back(node);
                        parameterListNode->children.push_back(parameterNode);
                     
                       
                        if (token.value == ":") {
                            currentIndex++;
                            token = tokens[currentIndex];
                            if (token.type == TokenType::Identifier) {
                                Node* node = new Node();
                                node->name = token.value;
                                parameterNode->children.push_back(node);
                                
                            }
                        }
                    }
                    lastToken = token;
                    token = tokens[++currentIndex];
                }
                currentIndex++;
            }
        }
        lastToken = token;
        token = tokens[++currentIndex];
    }
       
        if (token.value == "{") {
            
            if ( countForId != countForSelector)
            {
                methodNode->children.push_back(makeErrorNode("Erorr in method declaration"));
            }
            
            currentIndex++;
            token = tokens[currentIndex];
            
            Node* methodBodyNode = new Node();
            methodBodyNode->name = "MethodBody";
            methodNode->children.push_back(methodBodyNode);
            
            Node* assignmentNode = new Node();
            assignmentNode->name = "AssignmentStatement";
            
            Node* keyFunctionDecl = new Node();
            keyFunctionDecl->name = "function";
            
            while (currentIndex < tokens.size() && tokens[currentIndex].value != "}")
            {
                
                    token = tokens[currentIndex];
                
                    if (token.type == TokenType::Keyword && token.value == "return")
                    {
                        Node* assignmentNode = new Node();
                        assignmentNode->name = "Return statement";
                        
                        if (tokens[currentIndex+1].type == TokenType::Keyword  || tokens[currentIndex+1].type == TokenType::Identifier)
                        {
                            Node* rNode = new Node();
                            rNode->name = tokens[currentIndex+1].value;
                            assignmentNode->children.push_back(rNode);
                            methodBodyNode->children.push_back(assignmentNode);
                            currentIndex+=2;
                            break;
                        }
                        else
                        {
                            return false;
                        }
                    }
                  
                    else if (token.type == TokenType::Keyword )
                    {
                        if (assignmentNode->children.size() == 0 && tokens[currentIndex+1].value == "=")
                        {
                            Node* leftOperandNode = new Node();
                            leftOperandNode->name = "LeftOperand";
                            Node* node = new Node();
                            node->name = token.value;
                            leftOperandNode->children.push_back(node);
                            assignmentNode->children.push_back(leftOperandNode);
                        }
                        else if (assignmentNode->children.size() == 1)
                        {
                            Node* rightOperandNode = new Node();
                            rightOperandNode->name = "RightOperand";
                            Node* node2 = new Node();
                            if (tokens[currentIndex+1].type == TokenType::Keyword  || tokens[currentIndex+1].type ==TokenType::Identifier)
                            {
                                node2->name =tokens[currentIndex].value + " " + tokens[currentIndex+1].value;
                                currentIndex++;
                            }
                            else
                            {
                                node2->name =tokens[currentIndex].value;
                            }
                            
                            rightOperandNode->children.push_back(node2);
                            assignmentNode->children.push_back(rightOperandNode);
                            methodBodyNode->children.push_back(assignmentNode);
                            assignmentNode = new Node();
                        }
                        else if (assignmentNode->children.size() == 0 && keyFunctionDecl->children.size()==0 && tokens[currentIndex+1].value == "(")
                        {
                            Node* node = new Node();
                            node->name = token.value;
                            Node* functionName = new Node();
                            functionName->name = "Function Name";
                            functionName->children.push_back(node);
                            keyFunctionDecl->children.push_back(functionName);
                            functionName = new Node();
                            currentIndex+=1;
                             if (tokens[currentIndex+1].type == TokenType::Identifier || tokens[currentIndex+1].type == TokenType::NumericLiteral)
                             {
                                 Node* node = new Node();
                                 node->name = tokens[currentIndex+1].value;
                                 Node* functionBode = new Node();
                                 functionBode->name = "Function body";
                                 functionBode->children.push_back(node);
                                 keyFunctionDecl->children.push_back(functionBode);
                                 methodBodyNode->children.push_back(keyFunctionDecl);
                                 functionBode = new Node();
                                 keyFunctionDecl = new Node();
                                 currentIndex++;
                             }
                            else if ( tokens[currentIndex+2].type == TokenType::StringLiteral)
                            {
                                Node* node = new Node();
                                node->name = tokens[currentIndex+2].value;
                                Node* functionBode = new Node();
                                functionBode->name = "Function body";
                                functionBode->children.push_back(node);
                                keyFunctionDecl->children.push_back(functionBode);
                                methodBodyNode->children.push_back(keyFunctionDecl);
                                functionBode = new Node();
                                keyFunctionDecl = new Node();
                                currentIndex+=2;
                            }
                            else
                            {
                                std::cout<<"Error"<<std::endl;
                            }
                        }
                    }
                
                    else if (token.type == TokenType::Identifier)
                    {
                        if (assignmentNode->children.size() == 0 && tokens[currentIndex+1].value == "=")
                        {
                            Node* leftOperandNode = new Node();
                            leftOperandNode->name = "LeftOperand";
                            Node* node = new Node();
                            node->name = token.value;
                            leftOperandNode->children.push_back(node);
                            assignmentNode->children.push_back(leftOperandNode);
                        }
                        else if (assignmentNode->children.size() == 1)
                        {
                            Node* rightOperandNode = new Node();
                            rightOperandNode->name = "RightOperand";
                            Node* node2 = new Node();
                            node2->name =tokens[currentIndex].value;
                            rightOperandNode->children.push_back(node2);
                            assignmentNode->children.push_back(rightOperandNode);
                            methodBodyNode->children.push_back(assignmentNode);
                            assignmentNode = new Node();
                        }
                    }
                    else if (token.type == TokenType::NumericLiteral)
                    {
                        if (assignmentNode->children.size() == 1)
                        {
                            Node* rightOperandNode = new Node();
                            rightOperandNode->name = "RightOperand";
                            Node* node2 = new Node();
                            node2->name =tokens[currentIndex].value;
                            rightOperandNode->children.push_back(node2);
                            assignmentNode->children.push_back(rightOperandNode);
                            methodBodyNode->children.push_back(assignmentNode);
                            assignmentNode = new Node();
                        }
                    }
                    else if (token.type == TokenType::StringLiteral)
                    {
                        if (assignmentNode->children.size() == 1)
                        {
                            Node* rightOperandNode = new Node();
                            rightOperandNode->name = "RightOperand";
                            Node* node2 = new Node();
                            node2->name =tokens[currentIndex].value;
                            rightOperandNode->children.push_back(node2);
                            assignmentNode->children.push_back(rightOperandNode);
                            methodBodyNode->children.push_back(assignmentNode);
                            assignmentNode = new Node();
                        }
                    }
                lastToken = token;
                token = tokens[++currentIndex];
                }
        }
    return false;
}

bool SyntaxAnalyzer::parseFullMethodDeclaration(Node* parentNode, int& currentIndex)
{
    Token token = tokens[currentIndex];
    Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
  
    while (currentIndex < tokens.size() || (tokens[currentIndex].value != "}"))
    {
        Token token = tokens[currentIndex];
        Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
      
        while (currentIndex < tokens.size() || (tokens[currentIndex].value != ";"))
        {
             if (token.type == TokenType::Keyword && (lastToken.value == "("))
            {
                Node* attrNode = new Node();
                attrNode->name = token.value + "Type";
                parentNode->children.push_back(attrNode);
            }
            else if (token.type == TokenType::Identifier)
            {
                Node* idNode = new Node();
                idNode->name = token.value;
                parentNode->children.push_back(idNode);
            }
            else if (token.type == TokenType::Punctuation && token.value == "{")
            {
                Node* idNode = new Node();
                idNode->name = "function inner";
              //  parseStringCode(idNode, currentIndex);
                parentNode->children.push_back(idNode);
            }
            lastToken = token;
            token = tokens[++currentIndex];
        }
    }
    
    return true;
}

bool SyntaxAnalyzer::parseMethodDeclaration(Node* parentNode, int& currentIndex)
{
    Token token = tokens[currentIndex];
    Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
    int countForComplite = 0;
        while (currentIndex < tokens.size() && (token.value != ";") && (token.value != "-") && (tokens[currentIndex].type != TokenType::PreprocessorDirective))
        {
             if (token.type == TokenType::Keyword && (lastToken.value == "("))
            {
                Node* attrNode = new Node();
                attrNode->name = token.value + " returnType";
                parentNode->children.push_back(attrNode);
                countForComplite++;
            }
            else if (token.type == TokenType::Identifier)
            {
                Node* idNode = new Node();
                idNode->name = token.value;
                parentNode->children.push_back(idNode);
                countForComplite++;
            }
            lastToken = token;
            token = tokens[++currentIndex];
        }
    
    if (countForComplite!=2)
    {
        parentNode->children.push_back(makeErrorNode("Not all elements for method declaration"));
    }
    if ((tokens[currentIndex].value != ";" && currentIndex+1 == tokens.size()) || (tokens[currentIndex].value == "-") || (tokens[currentIndex].type == TokenType::PreprocessorDirective))
    {
        parentNode->children.push_back(makeErrorNode("No ; for methood declaration"));
        currentIndex--;
    }
    return true;
}

bool SyntaxAnalyzer::parseInterfaceDeclaration(Node* parentNode, int& currentIndex)
{
    Token token = tokens[currentIndex];
    Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
    int countForComplite = 0;
    while (currentIndex < tokens.size() && (tokens[currentIndex].value != "end") && (tokens[currentIndex].value != "implementation"))
    {
        if (token.type == TokenType::Identifier)
        {
            if (!countForComplite)
            {
                Node* interfaceIden = new Node();
                interfaceIden->name = token.value;
                parentNode->children.push_back(interfaceIden);
                lastToken = token;
                countForComplite++;
            }
            else
            {
                parentNode->children.push_back(makeErrorNode("waste Identifier in interface declaration"));
            }
        }
        else if (token.type == TokenType::Keyword && !parentNode->children.empty() && lastToken.type == TokenType::Punctuation)
        {
                Node* interfaceIden = new Node();
                interfaceIden->name = token.value;
                parentNode->children.push_back(interfaceIden);
                lastToken = token;
        }
        else if (token.type == TokenType::PreprocessorDirective)
        {
            if (countForComplite)
            {
                Node* decList = new Node();
                decList->name = token.value;
                parsePropertyDeclaration(decList, ++currentIndex);
                parentNode->children.push_back(decList);
                lastToken = token;
            }
            else if(token.value == "property" && !countForComplite)
            {
                parentNode->children.push_back(makeErrorNode("No Identifier for interface declaration"));
                Node* decList = new Node();
                decList->name = token.value;
                parsePropertyDeclaration(decList, ++currentIndex);
                parentNode->children.push_back(decList);
                lastToken = token;
            }
        }
        else if (token.value == "-")
        {
            if (parentNode->children.size() >= 2)
            {
                Node* decList = new Node();
                decList->name = "methodDeclaration";
                parseMethodDeclaration(decList, ++currentIndex);
                parentNode->children.push_back(decList);
                lastToken = token;
            }
            else
            {
                parentNode->children.push_back(makeErrorNode("waste methodDeclaration  in interface declaration"));
            }
        }
    
        lastToken = token;
        token = tokens[++currentIndex];
    }
    if (tokens[currentIndex].value != "end")
    {
        parentNode->children.push_back(makeErrorNode("No @end for interface"));
    }
    return true;
}


bool SyntaxAnalyzer::parseImplementationDeclaration(Node* parentNode, int& currentIndex)
{
    Token token = tokens[currentIndex];
    Token lastToken = {TokenType::Error, "", 0, Range(0,0)};
    
    if (token.type == TokenType::Identifier)
    {
       
        Node* interfaceIden = new Node();
        interfaceIden->name = token.value;
        parentNode->children.push_back(interfaceIden);
        lastToken = token;
        
    }
    else
    {
        parentNode->children.push_back(makeErrorNode("No Identifier for implementation"));
    }
    
    while (currentIndex < tokens.size() && (tokens[currentIndex].value != "end"))
    {
    
        if (token.value == "-")
        {
            Node* decList = new Node();
            decList->name = "methodDeclaration";
            parseFullMethodDeclaration02(decList, ++currentIndex);
            parentNode->children.push_back(decList);
            lastToken = token;
        }
        
        lastToken = token;
        currentIndex++;
        if (currentIndex < tokens.size())
        {
            token = tokens[currentIndex];
        }
       
    }
    if (tokens[currentIndex].value != "end" && currentIndex == tokens.size())
    {
        parentNode->children.push_back(makeErrorNode("No @end for implementation"));
    }
    return true;
}

Node* SyntaxAnalyzer::parseDeclaration() {
    if (currentIndex >= tokens.size()) {
        return nullptr;
    }

    Token token = tokens[currentIndex];
    root->name = "Main Root Node";
    while (currentIndex < tokens.size()){
        
        if (token.type == TokenType::PreprocessorDirective && token.value == "interface")
        {
            Node* interfaceNode = new Node();
            interfaceNode->name = "InterfaceDeclaration";
            parseInterfaceDeclaration(interfaceNode, ++currentIndex);
            root->children.push_back(interfaceNode);
            token = tokens[currentIndex];
        }
        if (token.type == TokenType::PreprocessorDirective && token.value == "implementation")
        {
            Node* interfaceNode = new Node();
            interfaceNode->name = "ImplementationDeclaration";
            parseImplementationDeclaration(interfaceNode, ++currentIndex);
            root->children.push_back(interfaceNode);
        }
        currentIndex++;
        if (tokens.size() > currentIndex)
        {
            token = tokens[currentIndex];
        }
    }

    return root;
}




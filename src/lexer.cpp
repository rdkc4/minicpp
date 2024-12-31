#include "../include/lexer.hpp"

Lexer::Lexer(const std::string& input) : input(input), position(0), lineNumber(1), prevLineLen(0){}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LEXICAL CHECK
//tokenization of entire file
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::tokenize(){
    while(position < input.size()){
                char curr = input[position];

                if(std::isspace(curr)){
                    ++position;
                    if(curr == '\n'){
                        ++lineNumber;
                        prevLineLen = position;
                    }
                }
                else if(std::isalpha(curr)){
                    std::string id = getID();
                    if(isKeyword(id)){
                        tokens.push_back(Token(keywords.at(id), id, lineNumber, position - prevLineLen));
                    }
                    else{
                        tokens.push_back(Token(TokenType::_ID, id, lineNumber, position - prevLineLen));
                    }
                }
                else if(std::isdigit(curr)){
                    tokens.push_back(Token(TokenType::_LITERAL, getLiteral(), lineNumber, position - prevLineLen));
                }
                else if((curr == '-' || curr == '+') && position < input.size()-1 && std::isdigit(input[position+1]) 
                        && !tokens.empty() && tokens[tokens.size()-1].type != TokenType::_LITERAL && 
                        tokens[tokens.size()-1].type != TokenType::_ID && tokens[tokens.size()-1].type != TokenType::_RPAREN){
                    
                    std::string val = "";
                    if(curr == '-'){
                        val += curr;
                    }
                    ++position;
                    val += getLiteral();
                    tokens.push_back(Token(TokenType::_LITERAL, val, lineNumber, position - prevLineLen));
                }
                else if(curr == '/' && position < input.size() - 1 && input[position + 1] == '/'){
                    singleLineComment();
                }
                else if(curr == '/' && position < input.size() - 1 && input[position + 1] == '*'){
                    multiLineComment();
                }
                else if(isBitwiseOperator(curr)){
                    tokens.push_back(Token(TokenType::_BITWISE, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(isAritOperator(curr)){
                    tokens.push_back(Token(TokenType::_AROP, std::string(1, curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == '=' && (position+1 >= input.size() || input[position+1] != '=')){
                    tokens.push_back(Token(TokenType::_ASSIGN, std::string(1, curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(isRelOperator(curr)){
                    tokens.push_back(Token(TokenType::_RELOP, getRelOperator(), lineNumber, position - prevLineLen));
                }
                else if(curr == '('){
                    tokens.push_back(Token(TokenType::_LPAREN, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == ')'){
                    tokens.push_back(Token(TokenType::_RPAREN, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == '{'){
                    tokens.push_back(Token(TokenType::_LBRACKET, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == '}'){
                    tokens.push_back(Token(TokenType::_RBRACKET, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == ','){
                    tokens.push_back(Token(TokenType::_COMMA, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == ';'){
                    tokens.push_back(Token(TokenType::_SEMICOLON, std::string(1,curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else if(curr == ':'){
                    tokens.push_back(Token(TokenType::_COLON, std::string(1, curr), lineNumber, position - prevLineLen));
                    ++position;
                }
                else{
                    tokens.push_back(Token(TokenType::_INVALID, std::string(1,curr), lineNumber, position - prevLineLen));
                    throw std::runtime_error("Line " + std::to_string(lineNumber) + ", Column " + std::to_string(position - prevLineLen) 
                        + ": LEXICAL ERROR at char '" + std::string(1, curr) + "'");
                }
            }
            tokens.push_back(Token(TokenType::_EOF, "", lineNumber, position - prevLineLen));
            //printTokens(tokens);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// MOVING ON TO THE NEXT TOKEN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
Token Lexer::nextToken(){
    if(tokens.empty()){
        tokenize();
    }
    if(_nextToken >= tokens.size()){
            return Token(TokenType::_EOF, "", lineNumber, position - prevLineLen);
    }
    return tokens[_nextToken++];
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETRIEVAL OF NEXT TOKEN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
Token Lexer::peekAtNext(){
    if(tokens.empty()){
        tokenize();
    }
    if(_nextToken >= tokens.size()){
        return Token(TokenType::_EOF, "", lineNumber, position - prevLineLen);
    }
    return tokens[_nextToken];
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAYING ALL TOKENS (debbuging purpose)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::printTokens(const std::vector<Token>& tokens) const{
    for(const auto& token: tokens){
        std::cout << "Token: type - " + tokenTypeToString.at(token.type);      
        if(token.type != TokenType::_EOF && token.type != TokenType::_INVALID){
            std::cout << "\t| value - " << token.value;
        }
        std::cout << "\n";
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of ID token ([A-Za-z0-9_])
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string Lexer::getID(){
    size_t start = position;
    while(position < input.size() && (std::isalnum(input[position]) || input[position] == '_')){
        ++position;
    }
    return input.substr(start, position - start);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of Literal token ([-]?[0-9][0-9]*[u]?)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string Lexer::getLiteral(){
    size_t start = position;
    while(position < input.size() && std::isdigit(input[position])){
        ++position;
    }
    if(position < input.size() && input[position] == 'u'){
        ++position;
    }
    return input.substr(start, position - start);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for predefined tokens
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isKeyword(const std::string& value) const{
    return keywords.find(value) != keywords.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// arithmetic operator checking
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isAritOperator(char curr) const{
    return arithmeticOperators.find(std::string(1, curr)) != arithmeticOperators.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// bitwise operator checking
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isBitwiseOperator(char curr) const{
    return bitwiseOperators.find(std::string(1, curr)) != bitwiseOperators.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// relational operator checking
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isRelOperator(char curr) const{
    return relationalOperators.find(std::string(1,curr)) != relationalOperators.end() || curr == '=';
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of rleational operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string Lexer::getRelOperator(){
    std::string op(1, input[position++]);
    if(position < input.size() && (op == "=" || op == "!" || op == "<" || op == ">")){
        char next = input[position];
        if((op == "=" || op == "!" || op == "<" || op == ">") && next == '='){
            op+= next;
            ++position;
        }
    }
    return op;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMMENTS - // single-line
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::singleLineComment(){
    position += 2;
    while(position < input.size() && input[position] != '\n'){
        ++position;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMMENTS - /* multi-line */
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::multiLineComment(){
    size_t startLine = lineNumber;
    size_t startPosition = position - prevLineLen;

    position += 2;
    while(position < input.size()-1 && input[position]!='*' && input[position+1]!='/'){
        if(input[position] == '\n'){
            ++lineNumber;
            prevLineLen = position;
        }
        ++position;
    }
    if(input[position] == '*' && input[position + 1] == '/'){
        position += 2;
    }
    else{
        std::cerr << "Line " << lineNumber << ", Column " << position - prevLineLen << ": SYNTAX ERROR - multi-line comment starting at " 
            << "line " << startLine << ", column " << startPosition << " is not closed\n";
    }
}
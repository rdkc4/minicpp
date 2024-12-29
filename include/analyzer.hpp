#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <unordered_set>

#include "ASTree.hpp"
#include "scope_manager.hpp"

class Analyzer{
    public:
        
        Analyzer(ScopeManager& scopeManager);

        void semanticCheck(std::shared_ptr<ASTree> root);

    private:
        
        ScopeManager scopeManager;
        std::string activeFunction;
        bool returned;
        
        void functionCheck(std::shared_ptr<ASTree> node);

        void parameterCheck(std::shared_ptr<ASTree> node);

        void bodyCheck(std::shared_ptr<ASTree> node);

        void checkVariables(std::shared_ptr<ASTree> node);

        void checkStatements(std::shared_ptr<ASTree> node);

        void checkStatement(std::shared_ptr<ASTree> node);

        void checkIfStatement(std::shared_ptr<ASTree> node);

        void checkWhileStatement(std::shared_ptr<ASTree> node);

        void checkForStatement(std::shared_ptr<ASTree> node);

        void checkDoWhileStatement(std::shared_ptr<ASTree> node);

        void checkSwitchStatement(std::shared_ptr<ASTree> node);

        void checkSwitchStatementInt(std::shared_ptr<ASTree> node);

        void checkSwitchStatementUnsigned(std::shared_ptr<ASTree> node);

        void checkCompoundStatement(std::shared_ptr<ASTree> node);

        void checkAssignmentStatement(std::shared_ptr<ASTree> node);

        void checkReturnStatement(std::shared_ptr<ASTree> node);

        void checkNumericalExpression(std::shared_ptr<ASTree> node);

        Types getNumericalExpressionType(std::shared_ptr<ASTree> node);

        void checkRelationalExpression(std::shared_ptr<ASTree> node);

        void checkID(std::shared_ptr<ASTree> node);

        void checkLiteral(std::shared_ptr<ASTree> node);

        void checkFunctionCall(std::shared_ptr<ASTree> node);
    
        void checkArgument(std::shared_ptr<ASTree> node);

};

#endif
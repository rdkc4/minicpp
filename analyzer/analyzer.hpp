#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include "../common/abstract-syntax-tree/ASTree.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"

class Analyzer{
    public:
        
        Analyzer(ScopeManager& scopeManager);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point for semantic analysis
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void semanticCheck(std::shared_ptr<ASTree> root);

    private:
        
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // scope manager - detection of undefined/redefined variables, type checking
        // active function - function that is being analyzed at the moment
        // returned - check whether function returns what is expected
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        ScopeManager scopeManager;
        std::string activeFunction;
        bool returned;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // function
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void checkFunction(std::shared_ptr<ASTree> node);

        void checkParameter(std::shared_ptr<ASTree> node);

        void checkBody(std::shared_ptr<ASTree> node);

        void checkConstruct(std::shared_ptr<ASTree> node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        void checkVariable(std::shared_ptr<ASTree> node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        void checkStatement(std::shared_ptr<ASTree> node);

        void checkIfStatement(std::shared_ptr<ASTree> node);

        void checkWhileStatement(std::shared_ptr<ASTree> node);

        void checkForStatement(std::shared_ptr<ASTree> node);

        void checkDoWhileStatement(std::shared_ptr<ASTree> node);

        void checkSwitchStatement(std::shared_ptr<ASTree> node);

        template<typename T>
        void checkSwitchStatementCases(std::shared_ptr<ASTree> node);

        void checkCompoundStatement(std::shared_ptr<ASTree> node);

        void checkAssignmentStatement(std::shared_ptr<ASTree> node);

        void checkReturnStatement(std::shared_ptr<ASTree> node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void checkNumericalExpression(std::shared_ptr<ASTree> node);

        Types getNumericalExpressionType(std::shared_ptr<ASTree> node);

        void checkRelationalExpression(std::shared_ptr<ASTree> node);

        void checkID(std::shared_ptr<ASTree> node);

        void checkLiteral(std::shared_ptr<ASTree> node);

        void checkFunctionCall(std::shared_ptr<ASTree> node);
    
        void checkArgument(std::shared_ptr<ASTree> node);

};

#endif

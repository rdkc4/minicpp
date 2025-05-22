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
        void semanticCheck(ASTree* root);

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
        void checkFunction(ASTree* node);

        void checkParameter(ASTree* node);

        void checkBody(ASTree* node);

        void checkConstruct(ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        void checkVariable(ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        void checkStatement(ASTree* node);

        void checkIfStatement(ASTree* node);

        void checkWhileStatement(ASTree* node);

        void checkForStatement(ASTree* node);

        void checkDoWhileStatement(ASTree* node);

        void checkSwitchStatement(ASTree* node);

        template<typename T>
        void checkSwitchStatementCases(ASTree* node);

        void checkCompoundStatement(ASTree* node);

        void checkAssignmentStatement(ASTree* node);

        void checkReturnStatement(ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void checkNumericalExpression(ASTree* node);

        Types getNumericalExpressionType(ASTree* node);

        void checkRelationalExpression(ASTree* node);

        void checkID(ASTree* node);

        void checkLiteral(ASTree* node);

        void checkFunctionCall(ASTree* node);
    
        void checkArgument(ASTree* node);

};

#endif

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
        void semanticCheck(const ASTree* root);

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
        void checkFunction(const ASTree* node);

        void checkParameter(ASTree* node);

        void checkBody(const ASTree* node);

        void checkConstruct(const ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        void checkVariable(const ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        void checkStatement(const ASTree* node);

        void checkIfStatement(const ASTree* node);

        void checkWhileStatement(const ASTree* node);

        void checkForStatement(const ASTree* node);

        void checkDoWhileStatement(const ASTree* node);

        void checkSwitchStatement(const ASTree* node);

        template<typename T>
        void checkSwitchStatementCases(const ASTree* node);

        void checkCompoundStatement(const ASTree* node);

        void checkAssignmentStatement(const ASTree* node) const;

        void checkReturnStatement(const ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void checkNumericalExpression(ASTree* node) const;

        Types getNumericalExpressionType(ASTree* node) const;

        void checkRelationalExpression(const ASTree* node) const;

        void checkID(ASTree* node) const;

        void checkLiteral(const ASTree* node) const;

        void checkFunctionCall(ASTree* node) const;
    
        void checkArgument(const ASTree* node) const;

};

#endif

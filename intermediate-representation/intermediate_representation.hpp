#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <string>

#include "../common/intermediate-representation-tree/IRTree.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"

class IntermediateRepresentation{

    public:

        std::shared_ptr<IRTree> formIR(std::shared_ptr<ASTree> astRoot);

    private:

        std::shared_ptr<IRTree> function(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> parameter(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> variable(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> statement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> ifStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> compoundStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> assignmentStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> returnStatement(std::shared_ptr<ASTree> node);
        
        std::shared_ptr<IRTree> whileStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> forStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> doWhileStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> switchStatement(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> _case(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> _default(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> _break();

        std::shared_ptr<IRTree> numericalExpression(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> mergeLiterals(std::shared_ptr<IRTree> lchild, std::shared_ptr<IRTree> rchild, std::string& op);

        template<typename T>
        T mergeValues(T l, T r, std::string& op);

        std::shared_ptr<IRTree> relationalExpression(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> id(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> literal(std::shared_ptr<ASTree> node);

        std::shared_ptr<IRTree> functionCall(std::shared_ptr<ASTree> node);
    
        std::shared_ptr<IRTree> argument(std::shared_ptr<ASTree> node);

};

#endif
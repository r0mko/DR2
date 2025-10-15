#ifndef DRCODE_H
#define DRCODE_H
#include <QtCore>

typedef int(*ByteCodeNode)(void**);

namespace DRVM
{

class CodeElement
{
    ByteCodeNode functionPtr;
    void** argsPtr;

public:
    enum ElementType { call, args };
    CodeElement() : functionPtr(0), argsPtr(0) {}
    CodeElement(ElementType type, void* ptr);

};

class DrCode
{
    QVector<CodeElement> code;

public:
    DrCode();
};
} // namespace

namespace AST {



class Node
{
public:
    enum NodeType
    {
        node_Block,
        node_VarAssignment,
        node_VarDeclaration,
        node_ArgList,
        node_Arg,
        node_Constant,
        node_FunCall,
        node_Expression,
        node_CondStatement,
        node_Empty
    };
    virtual DRVM::CodeElement emitCode() = 0;
    NodeType type() const { return nodeType; }

protected:
    NodeType nodeType;
    Node(NodeType t) : nodeType(t) {}
};

class ArgNode : public Node
{
    QVariant value;
public:
    ArgNode(const QVariant& v) : Node(node_Arg) { value=v; }
    void* ptr() { return value.data(); }
};

class ArgListNode : public Node
{
    //    QList<
public:

};

class CallNode : public Node
{
    QString functionName;
public:
    CallNode(const QString& n) : Node(node_FunCall) { functionName = n;}
    DRVM::CodeElement emitCode();

};


class Block : public Node
{
    QList<Node> d;
public:
    Block() : Node(node_Block) {}
//    Block& operator <<(Block& out, Node& n);
    int size() const { return d.size(); }
};

}
#endif // DRCODE_H

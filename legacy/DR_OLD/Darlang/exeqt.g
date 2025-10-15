%decl darlangparser.h
%parser DarlangParser
%impl darlang_parser.cpp

%token TypeINT "int"
%token TypeUINT "uint"
%token TypeINT64 "int64"
%token TypeUINT64 "uint64"
%token TypeDOUBLE "double"
%token TypeFLOAT "float"
%token TypeSTRING "string"
%token TypeBYTE "byte"
%token TypeBOOL "bool"
%token TypeOBJECT "object"
%token TypeVOID "void"
%token T_FALSE "false"
%token T_TRUE "true"
%token T_IF "if"
%token T_WHILE "while"
%token T_ELSE "else"
%token T_PLUS "+"
%token T_PLUSPLUS "++"
%token T_MINUS "-"
%token T_MINUSMINUS "--"
%token T_STAR "*"
%token T_DIV "/"
%token T_LPAREN "("
%token T_RPAREN ")"
%token T_LCURLYBRACE "{"
%token T_RCURLYBRACE "}"
%token T_LBRACKET "["
%token T_RBRACKET "]"
%token T_QUOTE "\""
%token T_COMMA ","
%token T_DOT "."
%token T_SEMICOLON ";"
%token T_EQ "="
%token T_MORE ">"
%token T_LESS "<"
%token T_MORE_EQ ">="
%token T_LESS_EQ "<="
%token T_AND "&"
%token T_ANDAND "&&"
%token T_OR "|"
%token T_OROR "||"
%token T_IDENTIFIER
%token T_STRING_LITERAL
%token T_NUMBER_INTEGER
%token T_NUMBER_REAL
%start Darcode

Darcode : SourceElements ;
SourceElements : SourceElements SourceElement ;
SourceElements : SourceElement;
SourceElement : FunctionCall T_SEMICOLON;
SourceElement : VariableDeclaration T_SEMICOLON ;
SourceElement : VariableAssignment T_SEMICOLON ;
SourceElement : IfStatement;

IfStatement : T_IF T_LPAREN Expression T_RPAREN Block ;
IfStatement : T_IF T_LPAREN Expression T_RPAREN SourceElement ;
IfStatement : T_IF T_LPAREN Expression T_RPAREN SourceElement T_ELSE SourceElement ;
IfStatement : T_IF T_LPAREN Expression T_RPAREN Block T_ELSE Block ;

IterationStatement: T_WHILE T_LPAREN Expression T_RPAREN Statement ;


Block : T_LCURLYBRACE SourceElements T_RCURLYBRACE ;


Number : T_NUMBER_INTEGER ;

Number : T_NUMBER_REAL ;
Expression : Number ;
Expression : T_IDENTIFIER ;

Expression : T_STRING_LITERAL ;


FunctionCall : T_IDENTIFIER T_LPAREN T_RPAREN ;
FunctionCall : T_IDENTIFIER T_LPAREN Args T_RPAREN ;

Args : Expression ;

Args : Args T_COMMA Expression;

VariableDeclaration : Type T_IDENTIFIER ;

VariableAssignment : VariableDeclaration T_EQ Expression ;

AssignmentExpression : T_IDENTIFIER T_EQ Expression ;

Type : TypeBOOL | TypeBYTE | TypeSTRING | TypeINT | TypeUINT | TypeINT64 | TypeUINT64 | TypeFLOAT | TypeDOUBLE;
Expression : SumExpression ;
Expression : SubExpression ;
Expression : FunctionCall ;
FieldExpression : T_IDENTIFIER T_DOT T_IDENTIFIER ;

FieldExpression : FieldExpression T_DOT T_IDENTIFIER ;

Expression : FieldExpression ;

SumExpression : Expression T_PLUS Expression ;

SubExpression : Expression T_MINUS Expression ;


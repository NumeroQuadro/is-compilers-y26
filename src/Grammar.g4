grammar Grammar;

// Стартовое правило
prog
    : statement* EOF
    ;

// Общие операторы
statement
    : ifStatement
    | whileStatement
    | forStatement
    | assignment ';'
    | variableDeclaration ';'
    ;

// Правила для условий
ifStatement
    : 'if' expression block (elifStatement)* (elseStatement)?
    ;

elifStatement
    : 'elif' expression block
    ;

elseStatement
    : 'else' block
    ;

// Правило для блока
block
    : '{' statement* '}'
    ;

// Правила для циклов
whileStatement
    : 'while' expression block
    ;

forStatement
    : 'for' variableDeclaration ';' expression ';' assignment block
    ;

// Правила для выражений
expression
    : expression 'or' expression
    | expression 'and' expression
    | comparison
    ;

comparison
    : term (comparisonOperator term)*
    ;

comparisonOperator
    : '==' | '<>' | '<' | '>' | '<=' | '>='
    ;

// Добавлены арифметические операторы
term
    : term ('+' | '-' | '*' | '/') term
    | IDENTIFIER
    | NUMBER
    | '(' expression ')'
    ;

// Правило для объявления переменных с указанием типа
variableDeclaration
    : type IDENTIFIER '=' expression
    ;

// Правило для присваивания
assignment
    : IDENTIFIER '=' expression
    ;

// Правило для типов данных
type
    : 'num'
    ;

// Лексические правила
IDENTIFIER
    : [a-zA-Z_][a-zA-Z0-9_]*
    ;

NUMBER
    : [0-9]+
    ;

WS
    : [ \t\r\n]+ -> skip
    ;

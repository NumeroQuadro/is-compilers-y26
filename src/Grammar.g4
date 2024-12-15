grammar Grammar;

INT : [0-9]+;
WS  : [ \t\r]+ -> skip;
NL  : '\n';

PLUS   : '+';
MINUS  : '-';
MULT   : '*';
DIV    : '/';
POPEN  : '(';
PCLOSE : ')';
GT     : '>';
IF     : 'if';
ELSE   : 'else';
LBRACE : '{';
RBRACE : '}';

input
    : NL* statement NL* EOF
    ;

statement
    : ifStatement                  # IfElse
    | plusOrMinus                  # Expression
    ;

ifStatement
    : IF condition block ELSE block # IfElseStatement
    ;

condition
    : plusOrMinus GT plusOrMinus     # GreaterThanCondition
    ;

block
    : LBRACE NL* statement (NL+ statement)* NL* RBRACE # CodeBlock
    ;

plusOrMinus
    : plusOrMinus PLUS  multOrDiv    # PlusOp
    | plusOrMinus MINUS multOrDiv    # BinaryMinusOp
    | multOrDiv                      # ToMultOrDiv
    ;

multOrDiv
    : multOrDiv MULT atom            # MultOp
    | multOrDiv DIV  atom            # DivOp
    | atom                           # ToAtom
    ;

atom
    : INT                            # Int
    | MINUS plusOrMinus              # UnaryMinusOp
    | POPEN plusOrMinus PCLOSE       # ParenthesisOp
    ;

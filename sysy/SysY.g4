grammar SysY;

root : compUnit*;

compUnit:
    varDecl
    | funcDef;

type: INT | FLOAT | VOID;

dimensions: (LB additiveExp RB)+;

varDecl: CONST? type varDef (COMMA varDef)* SEMI;

varDef:
    scalarVarDef
    | arrayVarDef;

scalarVarDef :Ident (ASSIGN additiveExp)?;

arrayVarDef :Ident dimensions (ASSIGN initVal)?;

initVal:
	additiveExp
	| LC (initVal (COMMA initVal)*)? RC;

funcDef: type Ident LP (funcArg (COMMA funcArg)*)? RP blockStmt;

funcArg: type Ident (LB RB (LB additiveExp RB)*)?;

blockStmt: LC stmt* RC;

stmt:
    assignStmt
    | varDecl
    | expStmt
    | ifStmt
    | ifElseStmt
    | whileStmt
	| blockStmt
	| blankStmt
	| breakStmt
	| continueStmt
	| retStmt;

assignStmt: lVal ASSIGN additiveExp SEMI;

blankStmt: SEMI;

expStmt: additiveExp SEMI;

ifElseStmt: IF LP lorExp RP stmt ELSE stmt;

ifStmt: IF LP lorExp RP stmt;

whileStmt: WHILE LP lorExp RP stmt;

breakStmt: BREAK SEMI;

continueStmt: CONTINUE SEMI;

retStmt: RETURN additiveExp? SEMI;

lVal: Ident (LB additiveExp RB)*;

unaryExp:
    (ADD | SUB | LNOT) unaryExp
    | LP additiveExp RP
    | varExp
    | funcCallExp
    | numberExp;

varExp:
    scalarVarExp
    | arrayVarExp;

scalarVarExp: Ident;

arrayVarExp: Ident (LB additiveExp RB)+;

funcCallExp: Ident LP (additiveExp (COMMA additiveExp)*)? RP;

numberExp:
    IntConst
    | FloatConst;

lorExp: landExp (LOR landExp)*;

landExp: equalityExp (LAND equalityExp)*;

equalityExp: relationalExp ((EQ | NE) relationalExp)*;

relationalExp: additiveExp ((LT | GT | LE | GE) additiveExp)*;

additiveExp: multiplicativeExp ((ADD | SUB) multiplicativeExp)*;

multiplicativeExp: unaryExp ((MUL | DIV | MOD) unaryExp)*;

BREAK: 'break';
CONST: 'const';
CONTINUE: 'continue';
ELSE: 'else';
FLOAT: 'float';
IF: 'if';
INT: 'int';
RETURN: 'return';
VOID: 'void';
WHILE: 'while';

ASSIGN: '=';

ADD: '+';
SUB: '-';
MUL: '*';
DIV: '/';
MOD: '%';

EQ: '==';
NE: '!=';
LT: '<';
LE: '<=';
GT: '>';
GE: '>=';

LNOT: '!';
LAND: '&&';
LOR: '||';

LP: '(';
RP: ')';
LB: '[';
RB: ']';
LC: '{';
RC: '}';

COMMA: ',';
SEMI: ';';

Ident: [A-Za-z_][0-9A-Za-z_]*;

IntConst:
	[1-9][0-9]*
	| '0'[0-7]*
	| '0'[Xx][0-9A-Fa-f]+;

FloatConst:
    ([0-9]+'.'[0-9]*|'.'[0-9]+)([Ee][+-]?[0-9]+)?
    | [0-9]+[Ee][+-]?[0-9]+
    | '0'[Xx]([0-9A-Fa-f]+'.'[0-9A-Fa-f]*|'.'[0-9A-Fa-f]+)[Pp][+-]?[0-9]+;

Whitespace: [ \r\t]+ -> skip;

Newline: '\n' -> skip;

BlockComment: '/*' .*? '*/' -> skip;

LineComment: '//' (~'\n'* '\\\n')* ~'\n'* -> skip;
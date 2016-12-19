% A MiniZinc model
<model> ::= [ <item> ";" ... ]

% Items
<item>  ::= <include-item>
          | <var-decl-item>
          | <assign-item>
          | <constraint-item> 
          | <solve-item>
          | <output-item>
          | <predicate-item>
          | <test-item>
          | <function-item>
          | <annotation-item>

%
<type-inst-syn-item> ::= "type" <ident> <annotations> "=" <ti-expr>

<ti-expr-and-id> ::= <ti-expr> ":" <ident>

<include-item> ::= "include" <string-literal>

<var-decl-item> ::= <ti-expr-and-id> <annotations> [ "=" <expr> ]

<assign-item> ::= <ident> "=" <expr>

<constraint-item> ::= "constraint" <expr>

<solve-item> ::= "solve" <annotations> "satisfy"
               | "solve" <annotations> "minimize" <expr>
               | "solve" <annotations> "maximize" <expr>

<output-item> ::= "output" <expr>

<annotation-item> ::= "annotation" <ident> <params>

<predicate-item> ::= "predicate" <operation-item-tail>

<test-item> ::= "test" <operation-item-tail>

<function-item> ::= "function" <ti-expr> ":" <operation-item-tail>

<operation-item-tail> ::= <ident> <params> <annotations> [ "=" <expr> ]

<params> ::= [ ( <ti-expr-and-id> "," ... ) ]

% Type-inst expressions
<ti-expr> ::= <base-ti-expr>

<base-ti-expr> ::= <var-par> <base-ti-expr-tail>

<var-par> ::= "var" | "par" | ε

<base-ti-expr-tail> ::= <ident>
                      | "bool"
                      | "int"
                      | "float"
                      | "string"
                      | <set-ti-expr-tail>
                      | <array-ti-expr-tail>
                      | "ann"
                      | "opt" <base-ti-expr-tail>
                      | { <expr> "," ... }
                      | <num-expr> ".." <num-expr>

% Set type-inst expressions
<set-ti-expr-tail> ::= "set" "of" <base-type>

% Array type-inst expressions
<array-ti-expr-tail> ::= "array" [ <ti-expr> "," ... ] "of" <ti-expr>
                       | "list" "of" <ti-expr>

%
<ti-variable-expr-tail> ::= $[A-Za-z][A-Za-z0-9_]*

<op-ti-expr-tail> ::= "op" ( <ti-expr> ":" ( <ti-expr> "," ... ) )

% Expressions
<expr> ::= <expr-atom> <expr-binop-tail>

<expr-atom> ::= <expr-atom-head> <expr-atom-tail> <annotations>

<expr-binop-tail> ::= "[" <bin-op> <expr> "]"

<expr-atom-head> ::= <builtin-un-op> <expr-atom>
                   | "(" <expr> ")"
                   | <ident-or-quoted-op>
                   | "_"
                   | <bool-literal>
                   | <int-literal>
                   | <float-literal>
                   | <string-literal>
                   | <set-literal>
                   | <set-comp>
                   | <simple-array-literal>
                   | <simple-array-literal-2d>
                   | <indexed-array-literal>
                   | <simple-array-comp>
                   | <indexed-array-comp>
                   | <ann-literal>
                   | <if-then-else-expr>
                   | <let-expr>
                   | <call-expr>
                   | <gen-call-expr>

<expr-atom-tail> ::= ε
                   | <array-access-tail> <expr-atom-tail>

% Numeric expressions
<num-expr> ::= <num-expr-atom> <num-expr-binop-tail>

<num-expr-atom> ::= <num-expr-atom-head> <expr-atom-tail> <annotations>

<num-expr-binop-tail> ::= "[" <num-bin-op> <num-expr> "]"

<num-expr-atom-head> ::= <builtin-num-un-op> <num-expr-atom>
                       | "(" <num-expr> ")"
                       | <ident-or-quoted-op>
                       | <int-literal>
                       | <float-literal>
                       | <if-then-else-expr>
                       | <case-expr>
                       | <let-expr>
                       | <call-expr>
                       | <gen-call-expr>

% Built-in operators
<builtin-op> ::= <builtin-bin-op> | <builtin-un-op>

<bin-op> ::= <builtin-bin-op> | ‘<ident>‘

<builtin-bin-op> ::= "<->" | "->" | "<-" | "\/" | "xor" | "/\" | "<" | ">" | "<=" | ">=" | "==" | "=" | "!="
                   | "in" | "subset" | "superset" | "union" | "diff" | "symdiff"
                   | ".." | "intersect" | "++" | <builtin-num-bin-op>

<builtin-un-op> ::= "not" | <builtin-num-un-op>

% Built-in numeric operators
<num-bin-op> ::= <builtin-num-bin-op> | ‘<ident>‘

<builtin-num-bin-op> ::= "+" | "-" | "*" | "/" | "div" | "mod"

<builtin-num-un-op> ::= "+" | "-"

% Literals
<bool-literal> ::= "false" | "true"

<int-literal> ::= [0-9]+
                | 0x[0-9A-Fa-f]+
                | 0o[0-7]+

<float-literal> ::= [0-9]+.[0-9]+
                  | [0-9]+.[0-9]+[Ee][-+]?[0-9]+
                  | [0-9]+[Ee][-+]?[0-9]+

<string-contents> ::= ([^"\n\] | \[^\n(])*

<string-literal> ::= "<string-contents>"
                   | "<string-contents>"\(" <string-interpolate-tail>

<string-interpolate-tail> ::= <expr> ")"<string-contents>"
                            | <expr> ")"<string-contents>"\(" <string-interpolate-tail>

<set-literal> ::= "{" [ <expr> "," ... ] "}"

<set-comp> ::= "{" <expr> "|" <comp-tail> "}"

<comp-tail> ::= <generator> "," ... [ "where" <expr> ]

<generator> ::= <ident> "," ... "in" <expr>

<array-literal> ::= "[" [ <expr> "," ... ] "]"

<array-literal-2d> ::= "[|" [ (<expr> "," ...) | ... ] "|]"

<array-comp> ::= "[" <expr> "|" <comp-tail> "]"

<array-access-tail> ::= "[ <expr> "," ... "]"

<ann-literal> ::= <ident> [ "(" <expr> "," ... ")" ]

<if-then-else-expr> ::= "if" <expr> "then" <expr> ( "elseif" <expr> "then" <expr> )* "else" <expr> "endif"

<call-expr> ::= <ident-or-quoted-op> [ "(" <expr> "," ... ")" ]

<let-expr> ::= "let" "{" <let-item> ";" ... "}" "in" <expr>

<let-item> ::= <var-decl-item>
             | <constraint-item>

<gen-call-expr> ::= <ident-or-quoted-op> "(" <comp-tail> ")" "(" <expr> ")"

<ident> ::= [A-Za-z][A-Za-z0-9_]* | ’[^’\xa\xd\x0]*’

<ident-or-quoted-op> ::= <ident>
                       | ’<builtin-op>’

<annotations> ::= ( "::" <annotation> )*

<annotation> ::= <expr-atom-head> <expr-atom-tail>

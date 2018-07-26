% A FlatZinc model
<model> ::= 
  [ <predicate-item> ]*
  [ <par-decl-item> ]*
  [ <var-decl-item> ]*
  [ <constraint-item> ]*
  <solve-item>

% Predicate items
<predicate-item> ::= "predicate" <identifier> "(" [ <pred-param-type> : <identifier> "," ... ] ")" ";"

% Identifiers
<identifier> ::= [A-Za-z][A-Za-z0-9_]*

<basic-par-type> ::= "bool"
                   | "int"
                   | "float"
                   | "set of int"

<par-type> ::= <basic-par-type>
             | "array" "[" <index-set> "]" "of" <basic-par-type>

<basic-var-type> ::= "var" "bool"
                   | "var" "int"
                   | "var" <int-literal> ".." <int-literal>
                   | "var" "{" <int-literal> "," ... "}"
                   | "var" "float"
                   | "var" <float-literal> ".." <float-literal>
                   | "var" "set" "of" <int-literal> ".." <int-literal>
                   | "var" "set" "of" "{" [ <int-literal> "," ... ] "}"

<array-var-type> ::= "array" "[" <index-set> "]" "of" <basic-var-type>

<index-set> ::= "1" ".." <int-literal>

<basic-pred-param-type> ::= <basic-par-type>
                          | <basic-var-type>
                          | <int-literal> ".." <int-literal>
                          | <float-literal> ".." <float-literal>
                          | "{" <int-literal> "," ... "}"
                          | "set" "of" <int-literal> .. <int-literal>
                          | "set" "of" "{" [  <int-literal> "," ... ] "}"
                          | "var" "set" "of" "int"

<pred-param-type> ::= <basic-pred-param-type>
                    | "array" "[" <pred-index-set> "]" "of" <basic-pred-param-type>

<pred-index-set> ::= <index-set>
                   | "int"

<basic-literal-expr> ::= <bool-literal>
                       | <int-literal>
                       | <float-literal>
                       | <set-literal>

<basic-expr> ::= <basic-literal-expr>
              | <var-par-identifier>

<expr>       ::= <basic-expr>
               | <array-literal>

<par-expr>   ::= <basic-literal-expr>
               | <par-array-literal>

<var-par-identifier> ::= [A-Za-z_][A-Za-z0-9_]*

% Boolean literals
<bool-literal> ::= "false"
                 | "true"

% Integer literals
<int-literal> ::= [0-9]+
                | 0x[0-9A-Fa-f]+
                | 0o[0-7]+

% Float literals
<float-literal> ::= [0-9]+.[0-9]+
                  | [0-9]+.[0-9]+[Ee][-+]?[0-9]+
                  | [0-9]+[Ee][-+]?[0-9]+

% Set literals
<set-literal> ::= "{" [ <int-literal> "," ... ] "}"
                | <int-literal> ".." <int-literal>
                | "{" [ <float-literal> "," ... ] "}"
                | <float-literal> ".." <float-literal>

<array-literal> ::= "[" [ <basic-expr> "," ... ] "]"

<par-array-literal> ::= "[" [ <basic-literal-expr> "," ... ] "]"

% Parameter declarations

<par-decl-item> ::= <par-type> ":" <var-par-identifier> "=" <par-expr> ";"

% Variable declarations

<var-decl-item> ::= <basic-var-type> ":" <var-par-identifier> <annotations> [ "=" <basic-expr> ] ";"
                  | <array-var-type> ":" <var-par-identifier> <annotations> "=" <array-literal> ";"
                  

% Constraint items

<constraint-item> ::= "constraint" <identifier> "(" [ <expr> "," ... ] ")" <annotations> ";"

% Solve item

<solve-item> ::= "solve" <annotations> "satisfy" ";"
               | "solve" <annotations> "minimize" <basic-expr> ";"
               | "solve" <annotations> "maximize" <basic-expr> ";"

% Annotations

<annotations> ::= [ "::" <annotation> ]*

<annotation> ::= <identifier>
               | <identifier> "(" <ann-expr> "," ... ")"

<ann-expr>   := <expr>
              | <annotation>

% End of FlatZinc grammar

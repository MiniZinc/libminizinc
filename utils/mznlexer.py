from pygments.lexer import RegexLexer, words, include, inherit
from pygments.token import *

class MznLexer(RegexLexer):
    name = 'MiniZinc'
    aliases = ['minizinc']
    filenames = ['*.mzn','*.fzn','*.dzn']

    tokens = {
        'root': [
            (words((
                'ann','annotation','any','array','assert','bool','constraint','enum','float','function',
                'in','include','int','list','of','op','output','minimize','maximize','par','predicate',
                'record','set','solve','string','test','tuple','type','var','where',
                'subset','superset','intersect','union','diff','symdiff',
                'satisfy', 'if', 'then', 'elseif', 'else', 'endif', 'false', 'true', 'not', 'div', 'mod',
                'xor', 'opt'
                ), prefix=r'\b', suffix=r'\b'), Name.Builtin),
            (words((
                '<->', '->', '<-', '/\\', '\\/', '..'
                )), Name.Builtin),
            (r'/\*', Comment.Multiline, 'comment'),
            (r'%.*?$', Comment.Singleline),
            (r'[A-Za-z][A-Za-z0-9_]*', Name),
            # Octal Literal
            (r'0o[0-7_]+', Number.Oct),
            # Hexadecimal Literal
            (r'0x[0-9a-fA-F_]+', Number.Hex),
            # Decimal Literal
            (r'[0-9]+\.[0-9]+|'
             r'[0-9]+\.[0-9]+[Ee][+-]?[0-9]+|[0-9]+[Ee][+-]?[0-9]+', Number.Float),
            (r'[0-9][0-9_]*', Number.Integer),            
            (r'"', String, 'string'),
            (r'.', Text)
        ],
        'string': [
                   (r'\\\(', String.Interpol, 'string-interpol'),
                   (r'"', String, '#pop'),
                   (r"""\\['"\\nrt]|\\x[0-9a-fA-F]{2}|\\[0-7]{1,3}"""
                    r"""|\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8}""", String.Escape),
                   (r'[^\\"]+', String),
                   (r'\\', String)
        ],
        'string-interpol': [
           (r'\(', String.Interpol, '#push'),
           (r'\)', String.Interpol, '#pop'),
           include('root')
        ],
        'comment': [
            (r'[^*/]', Comment.Multiline),
            (r'/\*', Comment.Multiline, '#push'),
            (r'\*/', Comment.Multiline, '#pop'),
            (r'[*/]', Comment.Multiline)
        ]        
    }

class MznDefLexer(MznLexer):
    name = 'MiniZincDef'
    aliases = ['minizincdef']

    tokens = {
        'root': [
            (r'<[0-9a-zA-Z- ]+>', Comment.Special),
            (r'::=', Comment.Special),
            (words(('|',), prefix=r'^\s*'), Comment.Special),
            inherit
        ]
    }

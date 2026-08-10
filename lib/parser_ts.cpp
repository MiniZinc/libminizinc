/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Lowering of the tree-sitter concrete syntax tree into the MiniZinc AST.
 *
 * The grammar (lib/thirdparty/tree_sitter_minizinc.c, generated from
 * lib/thirdparty/tree_sitter_minizinc/grammar.js) covers more of the language
 * than this compiler implements; anything it accepts but we cannot compile is
 * rejected here with a "not supported" syntax error rather than silently
 * dropped.
 *
 * Data files are parsed with the DataZinc grammar instead, which has only
 * assignment items and only literal-shaped expressions, so that a model
 * construct in a data file is reported where it appears rather than left to the
 * type checker. Its node types are a subset of MiniZinc's, so one lowering
 * serves both; `argument` and `signedLiteralText` are where the shapes differ.
 * A data file the DataZinc grammar rejects but MiniZinc accepts is still
 * parsed, with a deprecation warning: see `fall_back_to_model_grammar`.
 */

#include <minizinc/parser.hh>
#include <minizinc/warning.hh>

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <memory>
#include <set>
#include <sstream>

#include <minizinc/_thirdparty/tree_sitter/api.h>

extern "C" const TSLanguage* tree_sitter_minizinc(void);
extern "C" const TSLanguage* tree_sitter_datazinc(void);

namespace MiniZinc {

namespace {

/// Dense node kinds, so that dispatch is a jump table rather than a chain of
/// TSSymbol comparisons.
enum class K : unsigned char {
  Unknown = 0,
  // items
  SourceFile,
  Annotation,
  Assignment,
  Constraint,
  Declaration,
  Enumeration,
  FunctionItem,
  Goal,
  Include,
  Output,
  Predicate,
  TypeAlias,
  ClassDecl,
  // expressions
  AnnotatedExpression,
  ArrayComprehension,
  Call,
  CaseExpression,
  CaseExpressionCase,
  GeneratorCall,
  IfThenElse,
  IndexedAccess,
  InfixOperator,
  InversedIdentifier,
  Lambda,
  LetExpression,
  ParenthesisedExpression,
  PostfixOperator,
  PrefixOperator,
  RecordAccess,
  SetComprehension,
  StringInterpolation,
  TupleAccess,
  // literals
  Absent,
  Anonymous,
  ArrayLiteral,
  ArrayLiteral2d,
  ArrayLiteral2dRow,
  ArrayLiteral3d,
  ArrayLiteral3dRow,
  ArrayLiteral3dSlice,
  ArrayLiteralMember,
  BooleanLiteral,
  FloatLiteral,
  Infinity,
  IntegerLiteral,
  RecordLiteral,
  RecordMember,
  SetLiteral,
  StringLiteral,
  TupleLiteral,
  // types
  AnyType,
  ArrayDimension,
  ArrayType,
  ListType,
  NewType,
  OperationType,
  PrimitiveType,
  RecordType,
  RecordTypeField,
  SetType,
  TupleType,
  TypeBase,
  TypeConcatenation,
  TypeInstEnumId,
  TypeInstId,
  // names and patterns
  BacktickIdentifier,
  Identifier,
  QuotedIdentifier,
  PatternCall,
  PatternNumericLiteral,
  PatternRecord,
  PatternRecordField,
  PatternTuple,
  // misc
  AnnotationParameters,
  AnonymousEnumeration,
  ArgOrParam,
  AssignmentGenerator,
  EnumerationConstructor,
  EnumerationMembers,
  EscapeSequence,
  Generator,
  Parameter,
  StringCharacters,
  // trivia
  BlockComment,
  DocComment,
  FileDocComment,
  LineComment,
  END
};

struct KindName {
  K kind;
  const char* name;
};

// Every named, visible node type in node-types.json must appear here; the
// coverage assertion in `Syms` fails the first parse otherwise.
const KindName KIND_NAMES[] = {
    {K::SourceFile, "source_file"},
    {K::Annotation, "annotation"},
    {K::Assignment, "assignment"},
    {K::Constraint, "constraint"},
    {K::Declaration, "declaration"},
    {K::Enumeration, "enumeration"},
    {K::FunctionItem, "function_item"},
    {K::Goal, "goal"},
    {K::Include, "include"},
    {K::Output, "output"},
    {K::Predicate, "predicate"},
    {K::TypeAlias, "type_alias"},
    {K::ClassDecl, "class_decl"},
    {K::AnnotatedExpression, "annotated_expression"},
    {K::ArrayComprehension, "array_comprehension"},
    {K::Call, "call"},
    {K::CaseExpression, "case_expression"},
    {K::CaseExpressionCase, "case_expression_case"},
    {K::GeneratorCall, "generator_call"},
    {K::IfThenElse, "if_then_else"},
    {K::IndexedAccess, "indexed_access"},
    {K::InfixOperator, "infix_operator"},
    {K::InversedIdentifier, "inversed_identifier"},
    {K::Lambda, "lambda"},
    {K::LetExpression, "let_expression"},
    {K::ParenthesisedExpression, "parenthesised_expression"},
    {K::PostfixOperator, "postfix_operator"},
    {K::PrefixOperator, "prefix_operator"},
    {K::RecordAccess, "record_access"},
    {K::SetComprehension, "set_comprehension"},
    {K::StringInterpolation, "string_interpolation"},
    {K::TupleAccess, "tuple_access"},
    {K::Absent, "absent"},
    {K::Anonymous, "anonymous"},
    {K::ArrayLiteral, "array_literal"},
    {K::ArrayLiteral2d, "array_literal_2d"},
    {K::ArrayLiteral2dRow, "array_literal_2d_row"},
    {K::ArrayLiteral3d, "array_literal_3d"},
    {K::ArrayLiteral3dRow, "array_literal_3d_row"},
    {K::ArrayLiteral3dSlice, "array_literal_3d_slice"},
    {K::ArrayLiteralMember, "array_literal_member"},
    {K::BooleanLiteral, "boolean_literal"},
    {K::FloatLiteral, "float_literal"},
    {K::Infinity, "infinity"},
    {K::IntegerLiteral, "integer_literal"},
    {K::RecordLiteral, "record_literal"},
    {K::RecordMember, "record_member"},
    {K::SetLiteral, "set_literal"},
    {K::StringLiteral, "string_literal"},
    {K::TupleLiteral, "tuple_literal"},
    {K::AnyType, "any_type"},
    {K::ArrayDimension, "array_dimension"},
    {K::ArrayType, "array_type"},
    {K::ListType, "list_type"},
    {K::NewType, "new_type"},
    {K::OperationType, "operation_type"},
    {K::PrimitiveType, "primitive_type"},
    {K::RecordType, "record_type"},
    {K::RecordTypeField, "record_type_field"},
    {K::SetType, "set_type"},
    {K::TupleType, "tuple_type"},
    {K::TypeBase, "type_base"},
    {K::TypeConcatenation, "type_concatenation"},
    {K::TypeInstEnumId, "type_inst_enum_id"},
    {K::TypeInstId, "type_inst_id"},
    {K::BacktickIdentifier, "backtick_identifier"},
    {K::Identifier, "identifier"},
    {K::QuotedIdentifier, "quoted_identifier"},
    {K::PatternCall, "pattern_call"},
    {K::PatternNumericLiteral, "pattern_numeric_literal"},
    {K::PatternRecord, "pattern_record"},
    {K::PatternRecordField, "pattern_record_field"},
    {K::PatternTuple, "pattern_tuple"},
    {K::AnnotationParameters, "annotation_parameters"},
    {K::AnonymousEnumeration, "anonymous_enumeration"},
    {K::ArgOrParam, "arg_or_param"},
    {K::AssignmentGenerator, "assignment_generator"},
    {K::EnumerationConstructor, "enumeration_constructor"},
    {K::EnumerationMembers, "enumeration_members"},
    {K::EscapeSequence, "escape_sequence"},
    {K::Generator, "generator"},
    {K::Parameter, "parameter"},
    {K::StringCharacters, "string_characters"},
    {K::BlockComment, "block_comment"},
    {K::DocComment, "doc_comment"},
    {K::FileDocComment, "file_doc_comment"},
    {K::LineComment, "line_comment"},
};

const char* const FIELD_NAMES[] = {
    "annotation",   "annotation_parameter",
    "any",          "argument",
    "body",         "cardinality",
    "case",         "collection",
    "column_index", "condition",
    "default",      "definition",
    "dimension",    "domain",
    "else",         "expression",
    "field",        "file",
    "function",     "generator",
    "identifier",   "in",
    "index",        "item",
    "left",         "member",
    "name",         "objective",
    "operand",      "operator",
    "opt",          "parameter",
    "parameters",   "record",
    "result",       "right",
    "row",          "slice",
    "strategy",     "template",
    "tuple",        "type",
    "value",        "var_par",
    "where",
};

/// Field ids, in the same order as FIELD_NAMES.
enum class F : unsigned char {
  Annotation = 0,
  AnnotationParameter,
  Any,
  Argument,
  Body,
  Cardinality,
  Case,
  Collection,
  ColumnIndex,
  Condition,
  Default,
  Definition,
  Dimension,
  Domain,
  Else,
  Expression,
  Field,
  File,
  Function,
  Generator,
  Identifier,
  In,
  Index,
  Item,
  Left,
  Member,
  Name,
  Objective,
  Operand,
  Operator,
  Opt,
  Parameter,
  Parameters,
  Record,
  Result,
  Right,
  Row,
  Slice,
  Strategy,
  Template,
  Tuple,
  Type,
  Value,
  VarPar,
  Where,
  END
};

/// Symbol and field lookup tables, built once from the language.
class Syms {
public:
  /// `complete` is false for the DataZinc grammar: it is a strict subset, so it
  /// genuinely lacks most fields. A missing field id is 0, which matches no
  /// child, and those fields are never asked for on a DataZinc tree.
  Syms(const TSLanguage* lang, bool complete) {
    std::map<std::string, K> byName;
    for (const auto& kn : KIND_NAMES) {
      byName.emplace(kn.name, kn.kind);
    }
    // Looked up by name rather than with ts_language_symbol_for_name, because a
    // node type that is also used as an alias has more than one symbol id.
    _kinds.assign(ts_language_symbol_count(lang), K::Unknown);
    for (size_t s = 0; s < _kinds.size(); s++) {
      const char* name = ts_language_symbol_name(lang, static_cast<TSSymbol>(s));
      if (name != nullptr) {
        auto it = byName.find(name);
        if (it != byName.end()) {
          _kinds[s] = it->second;
        }
      }
    }
    for (unsigned int i = 0; i < static_cast<unsigned int>(F::END); i++) {
      _fields[i] = ts_language_field_id_for_name(lang, FIELD_NAMES[i], strlen(FIELD_NAMES[i]));
      if (_fields[i] == 0 && complete) {
        // Silent otherwise: `child()` would return null and the lowering would
        // read the child as absent
        throw InternalError(std::string("tree-sitter grammar has no field '") + FIELD_NAMES[i] +
                            "'");
      }
    }
    checkAllSymbolsCovered(lang);
  }

  K kind(TSNode n) const {
    TSSymbol s = ts_node_symbol(n);
    return s < _kinds.size() ? _kinds[s] : K::Unknown;
  }
  TSFieldId field(F f) const { return _fields[static_cast<unsigned int>(f)]; }

private:
  /// Guards against a grammar bump introducing a node this file does not know
  /// about, which would otherwise be silently mishandled.
  void checkAllSymbolsCovered(const TSLanguage* lang) const {
    for (size_t s = 1; s < _kinds.size(); s++) {
      auto sym = static_cast<TSSymbol>(s);
      if (ts_language_symbol_type(lang, sym) != TSSymbolTypeRegular) {
        continue;
      }
      const char* name = ts_language_symbol_name(lang, sym);
      if (name == nullptr || name[0] == '_' || _kinds[s] != K::Unknown) {
        continue;
      }
      throw InternalError(std::string("tree-sitter node type '") + name +
                          "' is not handled by this parser");
    }
  }

  std::vector<K> _kinds;
  TSFieldId _fields[static_cast<unsigned int>(F::END)];
};

const Syms& syms(bool dataFile) {
  static const Syms model(tree_sitter_minizinc(), true);
  static const Syms data(tree_sitter_datazinc(), false);
  return dataFile ? data : model;
}

/// A parser is reused per thread: `Solns2Out` parses one of these per solution,
/// so creating one each time would show up in long searches.
TSParser* ts_parser(const TSLanguage* lang) {
  struct Holder {
    TSParser* p;
    Holder() : p(ts_parser_new()) {}
    ~Holder() { ts_parser_delete(p); }
  };
  static thread_local Holder holder;
  ts_parser_reset(holder.p);
  ts_parser_set_language(holder.p, lang);
  return holder.p;
}

/// How an infix/prefix/postfix operator spelling maps onto the AST. Unicode
/// spellings are aliases of the ASCII ones, as they are in the lexer.
struct OpEntry {
  const char* text;
  BinOpType bot;     ///< used when `call` is null
  const char* call;  ///< non-null: build a Call with this name instead
};

const OpEntry INFIX_OPS[] = {
    {"<->", BOT_EQUIV, nullptr},
    {"⟷", BOT_EQUIV, nullptr},
    {"⇔", BOT_EQUIV, nullptr},
    {"->", BOT_IMPL, nullptr},
    {"→", BOT_IMPL, nullptr},
    {"⇒", BOT_IMPL, nullptr},
    {"<-", BOT_RIMPL, nullptr},
    {"←", BOT_RIMPL, nullptr},
    {"⇐", BOT_RIMPL, nullptr},
    {"\\/", BOT_OR, nullptr},
    {"∨", BOT_OR, nullptr},
    {"xor", BOT_XOR, nullptr},
    {"⊻", BOT_XOR, nullptr},
    {"/\\", BOT_AND, nullptr},
    {"∧", BOT_AND, nullptr},
    {"<", BOT_LE, nullptr},
    {">", BOT_GR, nullptr},
    {"<=", BOT_LQ, nullptr},
    {"≤", BOT_LQ, nullptr},
    {">=", BOT_GQ, nullptr},
    {"≥", BOT_GQ, nullptr},
    {"=", BOT_EQ, nullptr},
    {"==", BOT_EQ, nullptr},
    {"!=", BOT_NQ, nullptr},
    {"≠", BOT_NQ, nullptr},
    {"in", BOT_IN, nullptr},
    {"∈", BOT_IN, nullptr},
    {"subset", BOT_SUBSET, nullptr},
    {"⊆", BOT_SUBSET, nullptr},
    {"superset", BOT_SUPERSET, nullptr},
    {"⊇", BOT_SUPERSET, nullptr},
    {"union", BOT_UNION, nullptr},
    {"∪", BOT_UNION, nullptr},
    {"diff", BOT_DIFF, nullptr},
    {"∖", BOT_DIFF, nullptr},
    {"symdiff", BOT_SYMDIFF, nullptr},
    {"intersect", BOT_INTERSECT, nullptr},
    {"∩", BOT_INTERSECT, nullptr},
    {"++", BOT_PLUSPLUS, nullptr},
    {"+", BOT_PLUS, nullptr},
    {"-", BOT_MINUS, nullptr},
    {"*", BOT_MULT, nullptr},
    {"/", BOT_DIV, nullptr},
    {"div", BOT_IDIV, nullptr},
    {"mod", BOT_MOD, nullptr},
    {"^", BOT_POW, nullptr},
    {"..", BOT_DOTDOT, nullptr},
    {"~+", BOT_PLUS, "~+"},
    {"~-", BOT_MINUS, "~-"},
    {"~*", BOT_MULT, "~*"},
    {"~/", BOT_DIV, "~/"},
    {"~div", BOT_IDIV, "~div"},
    {"~=", BOT_EQ, "~="},
    {"~!=", BOT_NQ, "~!="},
    {"default", BOT_EQ, "default"},
    {"..<", BOT_DOTDOT, "'..<'"},
    {"<..", BOT_DOTDOT, "'<..'"},
    {"<..<", BOT_DOTDOT, "'<..<'"},
};

/// `Annotation::add(vector)` keeps source order, where repeated single adds
/// would reverse it; the guard matches `Expression::addAnnotations`.
void add_annotations(Expression* e, const std::vector<Expression*>& anns) {
  if (anns.empty()) {
    return;
  }
  if (!Expression::isUnboxedVal(e) && e != Constants::constants().literalTrue &&
      e != Constants::constants().literalFalse) {
    Expression::ann(e).add(anns);
  }
}

const OpEntry* infix_op(const std::string& text) {
  for (const auto& e : INFIX_OPS) {
    if (text == e.text) {
      return &e;
    }
  }
  return nullptr;
}

/// Range operators with a missing operand become calls; `o` marks the gap.
const char* open_range_call(const std::string& op, bool operandOnLeft) {
  if (op == "..") {
    return operandOnLeft ? "..o" : "o..";
  }
  if (op == "..<") {
    return operandOnLeft ? "..<o" : "o..<";
  }
  if (op == "<..") {
    return operandOnLeft ? "<..o" : "o<..";
  }
  if (op == "<..<") {
    return operandOnLeft ? "<..<o" : "o<..<";
  }
  return nullptr;
}

/// Quoted identifiers naming an operator (`'+'(a,b)`) build a BinOp/UnOp
/// rather than a Call. Matched exactly, so that identifiers that merely look
/// like operators (`'C⁻¹'`) stay identifiers.
struct QuotedOp {
  const char* text;
  int bot;  ///< -1 means `not`
};

const QuotedOp QUOTED_OPS[] = {
    {"<->", BOT_EQUIV},
    {"->", BOT_IMPL},
    {"<-", BOT_RIMPL},
    {"\\/", BOT_OR},
    {"/\\", BOT_AND},
    {"xor", BOT_XOR},
    {"<", BOT_LE},
    {"<=", BOT_LQ},
    {">", BOT_GR},
    {">=", BOT_GQ},
    {"=", BOT_EQ},
    {"==", BOT_EQ},
    {"!=", BOT_NQ},
    {"in", BOT_IN},
    {"subset", BOT_SUBSET},
    {"superset", BOT_SUPERSET},
    {"union", BOT_UNION},
    {"diff", BOT_DIFF},
    {"symdiff", BOT_SYMDIFF},
    {"+", BOT_PLUS},
    {"-", BOT_MINUS},
    {"*", BOT_MULT},
    {"/", BOT_DIV},
    {"div", BOT_IDIV},
    {"mod", BOT_MOD},
    {"^", BOT_POW},
    {"intersect", BOT_INTERSECT},
    {"++", BOT_PLUSPLUS},
    {"..", BOT_DOTDOT},
    {"not", -1},
};

/// Quoted identifiers naming an operator keep their quotes in the AST, and a
/// few spellings are normalised (`'=='` is `'='`). Anything else in quotes is
/// an ordinary identifier and loses them.
const char* quoted_op_name(const std::string& content) {
  static const std::map<std::string, const char*> NAMES = {
      {"<->", "'<->'"},
      {"->", "'->'"},
      {"<-", "'<-'"},
      {"\\/", "'\\/'"},
      {"xor", "'xor'"},
      {"/\\", "'/\\'"},
      {"<", "'<'"},
      {">", "'>'"},
      {"<=", "'<='"},
      {">=", "'>='"},
      {"=", "'='"},
      {"==", "'='"},
      {"!=", "'!='"},
      {"in", "'in'"},
      {"subset", "'subset'"},
      {"superset", "'superset'"},
      {"union", "'union'"},
      {"diff", "'diff'"},
      {"symdiff", "'symdiff'"},
      {"..", "'..'"},
      {"<..", "'<..'"},
      {"..<", "'..<'"},
      {"<..<", "'<..<'"},
      {"+", "'+'"},
      {"-", "'-'"},
      {"*", "'*'"},
      {"^", "'^'"},
      {"/", "'/'"},
      {"div", "'div'"},
      {"mod", "'mod'"},
      {"intersect", "'intersect'"},
      {"not", "'not'"},
      {"++", "'++'"},
  };
  auto it = NAMES.find(content);
  return it == NAMES.end() ? nullptr : it->second;
}

/// Type keywords cannot name a function. tree-sitter keywords are contextual,
/// so `int(x)` would otherwise parse as a call to a function named `int`,
/// where the reference lexer reserves the word and rejects it.
bool is_reserved_call_name(const std::string& s) {
  static const std::set<std::string> RESERVED = {
      "ann", "any", "array",  "bool", "enum",   "float", "int",  "list",
      "opt", "par", "record", "set",  "string", "tuple", "type", "var",
  };
  return RESERVED.count(s) != 0;
}

const QuotedOp* quoted_op(const std::string& text) {
  for (const auto& e : QUOTED_OPS) {
    if (text == e.text) {
      return &e;
    }
  }
  return nullptr;
}

/// Variant of the definition in lexer.lxx: overflow throws ArithmeticError.
bool decimal_to_intval(const char* b, const char* e, IntVal& out) {
  IntVal x = 0;
  try {
    for (const char* p = b; p != e; p++) {
      x = (x * 10) + (*p - '0');
    }
  } catch (ArithmeticError&) {
    return false;
  }
  out = x;
  return true;
}

bool based_to_intval(const char* b, const char* e, int base, IntVal& out) {
  IntVal x = 0;
  try {
    for (const char* p = b; p != e; p++) {
      char c = *p;
      int d;
      if (c >= '0' && c <= '9') {
        d = c - '0';
      } else if (c >= 'a' && c <= 'f') {
        d = c - 'a' + 10;
      } else {
        d = c - 'A' + 10;
      }
      if (d >= base) {
        return false;
      }
      x = (x * base) + d;
    }
  } catch (ArithmeticError&) {
    return false;
  }
  out = x;
  return true;
}

/// Location of a byte offset, used before a tree exists.
ParserLocation nul_location(const ParserState& pp, unsigned int offset) {
  unsigned int line = 1;
  unsigned int lineStart = 0;
  for (unsigned int i = 0; i < offset; i++) {
    if (pp.buf[i] == '\n') {
      line++;
      lineStart = i + 1;
    }
  }
  unsigned int col = 0;
  for (unsigned int i = lineStart; i < offset; i++) {
    if ((static_cast<unsigned char>(pp.buf[i]) & 0xc0) != 0x80) {
      col++;
    }
  }
  return {ASTString(pp.filename), line, col + 1, line, col + 1};
}

/// Walks the children of a node under one field, without collecting them: a
/// `TSNode` is 32 bytes and a data file can hold millions of array members.
class ChildIterator {
public:
  /// The past-the-end iterator.
  ChildIterator() = default;
  /// Yields the children of \a parent under field \a id. If \a namedOnly, takes
  /// every named child instead of asking each one for its field, which is worth
  /// doing because that question costs a walk of every hidden ancestor. Only
  /// valid where the rule gives all named children the same field, so the first
  /// one is still checked.
  ChildIterator(TSNode parent, TSFieldId id, bool namedOnly)
      : _cursor(ts_tree_cursor_new(parent)), _id(id), _namedOnly(namedOnly), _live(true) {
    if (!ts_tree_cursor_goto_first_child(&_cursor)) {
      stop();
    } else if (!wanted()) {
      advance();
    }
  }
  ChildIterator(ChildIterator&& o) noexcept
      : _cursor(o._cursor), _id(o._id), _namedOnly(o._namedOnly), _live(o._live) {
    o._live = false;
  }
  ChildIterator(const ChildIterator&) = delete;
  ChildIterator& operator=(const ChildIterator&) = delete;
  ChildIterator& operator=(ChildIterator&&) = delete;
  ~ChildIterator() { stop(); }

  TSNode operator*() const { return ts_tree_cursor_current_node(&_cursor); }
  ChildIterator& operator++() {
    advance();
    return *this;
  }
  bool operator!=(const ChildIterator& o) const { return _live != o._live; }

private:
  void stop() {
    if (_live) {
      ts_tree_cursor_delete(&_cursor);
      _live = false;
    }
  }
  void advance() {
    while (ts_tree_cursor_goto_next_sibling(&_cursor)) {
      if (wanted()) {
        return;
      }
    }
    stop();
  }
  bool wanted() {
    if (!_namedOnly) {
      return ts_tree_cursor_current_field_id(&_cursor) == _id;
    }
    TSNode c = ts_tree_cursor_current_node(&_cursor);
    // Comments are named but fill no field, and may sit between any two
    // children.
    if (!ts_node_is_named(c) || ts_node_is_extra(c)) {
      return false;
    }
    if (!_checked) {
      _checked = true;
      if (ts_tree_cursor_current_field_id(&_cursor) != _id) {
        throw InternalError("tree-sitter parser: named children of '" +
                            std::string(ts_node_type(ts_tree_cursor_current_node(&_cursor))) +
                            "' do not all carry the expected field");
      }
    }
    return true;
  }

  TSTreeCursor _cursor{};
  TSFieldId _id = 0;
  bool _namedOnly = false;
  bool _checked = false;
  bool _live = false;
};

class ChildRange {
public:
  ChildRange(TSNode parent, TSFieldId id, bool namedOnly)
      : _parent(parent), _id(id), _namedOnly(namedOnly) {}
  ChildIterator begin() const { return {_parent, _id, _namedOnly}; }
  static ChildIterator end() { return {}; }

private:
  TSNode _parent;
  TSFieldId _id;
  bool _namedOnly;
};

/// Builds AST items for one file. Holds no garbage-collected state: the item
/// loop releases the GC lock between items, so anything allocated must be
/// reachable from the model by then.
class Lowerer {
public:
  Lowerer(ParserState& pp, TSNode root, const Syms& s, const TSLanguage* lang)
      : _pp(pp), _buf(pp.buf), _len(pp.length), _s(s), _lang(lang), _root(root) {
    // Byte columns equal code point columns unless the file has multi-byte
    // characters, which is the overwhelmingly common case.
    _ascii = true;
    for (unsigned int i = 0; i < _len; i++) {
      if ((static_cast<unsigned char>(_buf[i]) & 0x80) != 0) {
        _ascii = false;
        break;
      }
    }
  }

  void run();
  /// Reports every ERROR/MISSING node in the tree; true if there was any.
  bool reportSyntaxErrors();
  /// Called when the DataZinc grammar rejected a data file, on a tree re-read
  /// with the model grammar: names the offending item ("constraint item not
  /// allowed in data file") instead of reporting a syntax error at whatever
  /// token DataZinc happened to trip on. True if it reported anything.
  bool reportModelItemsInDataFile();
  /// Warns at the first place the DataZinc grammar could not continue. Called on
  /// the DataZinc tree when the file turned out to be valid MiniZinc, so the
  /// expression parses but does not belong in a data file.
  void warnNotDataZinc();

private:
  // -- tree navigation -------------------------------------------------------
  TSNode child(TSNode n, F f) const { return ts_node_child_by_field_id(n, _s.field(f)); }
  bool has(TSNode n, F f) const { return !ts_node_is_null(child(n, f)); }
  /// Every child of `n` under field `f`, in source order.
  ChildRange children(TSNode n, F f) const { return {n, _s.field(f), false}; }
  /// As `children`, for rules whose named children all carry field `f`. Saves
  /// asking each child for its field; see `ChildIterator`.
  ChildRange namedChildren(TSNode n, F f) const { return {n, _s.field(f), true}; }
  /// `children`, collected. Only for callers that need random access.
  std::vector<TSNode> childVector(TSNode n, F f) const;
  std::string text(TSNode n) const {
    return {_buf + ts_node_start_byte(n), _buf + ts_node_end_byte(n)};
  }

  // -- locations -------------------------------------------------------------
  /// Number of UTF-8 code points in [lineStart, byte).
  unsigned int codePointColumn(unsigned int lineStart, unsigned int byte) const;
  ParserLocation loc(TSNode n) const;

  // -- diagnostics -----------------------------------------------------------
  void error(TSNode n, const std::string& msg);
  void warn(TSNode n, const std::string& msg);
  /// Records a call made by a data file for the type checker to judge, unless it
  /// is one of the reshaping builtins. Whether the rest are enum constructors or
  /// model calls is not knowable here: a data file may name a constructor before
  /// the assignment that introduces it, or in an entirely different file.
  void noteDataCall(Call* c);
  /// Grammar-supported syntax that this compiler does not implement.
  std::nullptr_t futureFeature(TSNode n, const char* what);
  void collectSyntaxErrors(TSNode n);
  /// The field \a n fills in its parent, or `F::END` if it fills none
  F fieldOf(TSNode n) const;
  /// The first token after \a n, or a null node at end of file
  static TSNode nextToken(TSNode n);

  // -- items -----------------------------------------------------------------
  Item* item(TSNode n, const std::string* docComment);
  Item* includeItem(TSNode n);
  Item* constraintItem(TSNode n);
  Item* goalItem(TSNode n);
  Item* assignItem(TSNode n);
  /// `wholeSpan` includes the `= definition` in the declaration's location, as
  /// happens inside a `let`; at the top level the location stops after the
  /// annotations.
  Item* declarationItem(TSNode n, bool wholeSpan = false);
  Item* outputItem(TSNode n);
  Item* operationItem(TSNode n);
  Item* annotationItem(TSNode n);
  Item* enumerationItem(TSNode n);
  Item* typeAliasItem(TSNode n);

  // -- expressions -----------------------------------------------------------
  Expression* expr(TSNode n);
  Expression* infixExpr(TSNode n);
  Expression* prefixExpr(TSNode n);
  Expression* postfixExpr(TSNode n);
  Expression* callExpr(TSNode n);
  Expression* generatorCallExpr(TSNode n);
  Expression* arrayLiteral(TSNode n);
  Expression* arrayLiteral2d(TSNode n);
  Expression* arrayLiteral3d(TSNode n);
  Expression* comprehension(TSNode n, bool isSet);
  Expression* iteExpr(TSNode n);
  Expression* letExpr(TSNode n);
  Expression* indexedAccess(TSNode n);
  Expression* stringLiteral(TSNode n);
  Expression* stringInterpolation(TSNode n);
  Expression* intLiteral(TSNode n, bool negated);
  Expression* floatLiteral(TSNode n, bool negated);
  /// DataZinc folds a leading `-` into the numeric token rather than making it a
  /// prefix operator, so strip it here and fold it into `negated`.
  /// Points \a b / \a e at the literal's text in the source buffer, with a
  /// leading `-` folded into \a negated. A data file holds millions of these,
  /// so each one is read in place rather than copied into a std::string.
  void signedLiteralText(TSNode n, bool& negated, const char*& b, const char*& e) const;
  Expression* recordLiteral(TSNode n);
  Expression* tupleLiteral(TSNode n);

  // -- types -----------------------------------------------------------------
  TypeInst* typeInst(TSNode n);
  TypeInst* typeBase(TSNode n);
  /// Applies the `var`/`par` and `opt` prefixes of `n` to `t`.
  void applyVarParOpt(TSNode n, Type& t) const;
  TypeInst* arrayTypeInst(TSNode n);
  VarDecl* parameter(TSNode n);

  // -- shared helpers --------------------------------------------------------
  std::vector<Expression*> annotations(TSNode n);
  std::vector<Expression*> callArguments(TSNode n, bool& ok);
  Expression* argument(TSNode arg);
  /// Re-reads a `_type` node as an expression (call arguments and array index
  /// sets share the type rule with declarations).
  Expression* typeAsExpr(TSNode n);
  ASTString identifier(TSNode n);
  /// Accepts only identifier patterns; destructuring is not supported.
  bool patternName(TSNode n, ASTString& out);
  /// `idLocations` places each variable at its own identifier rather than at
  /// the collection; generator calls do this, plain comprehensions do not.
  bool generators(TSNode n, Generators& gens, bool idLocations = false);
  bool stringContents(TSNode n, std::string& out);
  bool appendStringPiece(TSNode n, std::string& out);
  std::vector<VarDecl*> parameters(TSNode n, bool& ok);
  /// `ann: name` on an operation item becomes a trailing parameter.
  VarDecl* annParameter(TSNode n);
  void addDocComment(Item* it, const std::string& doc, TSNode n);

  ParserState& _pp;
  const char* _buf;
  unsigned int _len;
  const Syms& _s;
  const TSLanguage* _lang;
  TSNode _root;
  bool _ascii;
  mutable unsigned int _colLineStart = 0;
  mutable unsigned int _colByte = 0;
  mutable unsigned int _colCount = 0;
};

// ---------------------------------------------------------------------------
// Navigation and locations
// ---------------------------------------------------------------------------

std::vector<TSNode> Lowerer::childVector(TSNode n, F f) const {
  std::vector<TSNode> out;
  for (TSNode c : children(n, f)) {
    out.push_back(c);
  }
  return out;
}

unsigned int Lowerer::codePointColumn(unsigned int lineStart, unsigned int byte) const {
  if (_ascii) {
    return byte - lineStart;
  }
  // Columns are asked for in source order, so continue from the previous
  // answer rather than rescanning the line; FlatZinc lines can be megabytes.
  unsigned int from = lineStart;
  unsigned int n = 0;
  if (_colLineStart == lineStart && _colByte <= byte) {
    from = _colByte;
    n = _colCount;
  }
  for (unsigned int i = from; i < byte && i < _len; i++) {
    if ((static_cast<unsigned char>(_buf[i]) & 0xc0) != 0x80) {
      n++;
    }
  }
  _colLineStart = lineStart;
  _colByte = byte;
  _colCount = n;
  return n;
}

ParserLocation Lowerer::loc(TSNode n) const {
  TSPoint s = ts_node_start_point(n);
  TSPoint e = ts_node_end_point(n);
  uint32_t sb = ts_node_start_byte(n);
  uint32_t eb = ts_node_end_byte(n);
  // `startByte - startPoint.column` is the byte offset of the start of the
  // line, since TSPoint columns are counted in bytes.
  unsigned int firstColumn = codePointColumn(sb - s.column, sb) + 1;
  unsigned int lastColumn = codePointColumn(eb - e.column, eb);
  if (e.row == s.row && lastColumn < firstColumn) {
    lastColumn = firstColumn;  // zero-width node (a MISSING token)
  }
  return {ASTString(_pp.filename), s.row + 1, firstColumn, e.row + 1, lastColumn};
}

void Lowerer::warn(TSNode n, const std::string& msg) {
  Warning(Location(loc(n)), msg).print(_pp.err, /*werror=*/false);
}

void Lowerer::error(TSNode n, const std::string& msg) {
  _pp.hadError = true;
  ParserLocation l = loc(n);
  std::vector<ASTString> includeStack;
  for (Model* m = _pp.model; m != nullptr; m = m->parent()) {
    if (m->parent() != nullptr) {
      includeStack.push_back(m->filename());
    }
  }
  _pp.syntaxErrors.emplace_back(
      Location(l),
      _pp.getCurrentLine(ts_node_start_byte(n), static_cast<int>(l.firstColumn()),
                         static_cast<int>(l.lastColumn())),
      includeStack, msg);
}

std::nullptr_t Lowerer::futureFeature(TSNode n, const char* what) {
  error(n, std::string(what) + " are not supported by this version of MiniZinc");
  return nullptr;
}

/// Functions a data file may call, being those that reshape data rather than
/// compute it. The rule for the list is that anything `--output-mode dzn` can
/// write has to be readable again, which is these plus enum constructors.
const char* const DATA_FUNCTIONS[] = {"anon_enum", "anon_enum_set", "array1d", "array2d", "array3d",
                                      "array4d",   "array5d",       "array6d", "to_enum"};

/// How an item that may not appear in a data file is named, or null if it may.
/// Kept word for word the same as the bison parser's `notInDatafile`.
const char* model_item_name(K k) {
  switch (k) {
    case K::Include:
      return "include";
    case K::Constraint:
      return "constraint";
    case K::Goal:
      return "solve";
    case K::Declaration:
    case K::Enumeration:
      return "variable declaration";
    case K::Output:
      return "output";
    case K::Predicate:
    case K::FunctionItem:
      return "predicate";
    case K::Annotation:
      return "annotation";
    case K::TypeAlias:
      return "type alias";
    default:
      return nullptr;
  }
}

void Lowerer::warnNotDataZinc() {
  // Descend to the smallest node still covering the error, which is the
  // offending construct rather than the item it sits in. `has_error` alone does
  // not get there: it is an error *cost*, and a small enough error leaf is
  // stored inline and costs nothing.
  TSNode n = _root;
  for (bool descended = true; descended;) {
    descended = false;
    for (uint32_t i = 0, count = ts_node_child_count(n); i < count; i++) {
      TSNode c = ts_node_child(n, i);
      if (ts_node_has_error(c) || ts_node_is_error(c) || ts_node_is_missing(c)) {
        n = c;
        descended = true;
        break;
      }
    }
  }
  warn(n, "only data belongs in a data file; using a MiniZinc expression here is deprecated");
}

bool Lowerer::reportModelItemsInDataFile() {
  bool reported = false;
  for (TSNode n : children(_root, F::Item)) {
    if (const char* what = model_item_name(_s.kind(n))) {
      error(n, std::string(what) + " item not allowed in data file");
      reported = true;
    }
  }
  return reported;
}

/// What a node filling this field stands for. A MISSING node is a placeholder
/// tree-sitter inserted, and its type is only the cheapest repair -- `identifier`
/// where any expression would have done -- so the field is the honest answer.
const char* field_role(F f) {
  switch (f) {
    case F::Argument:
    case F::Cardinality:
    case F::Collection:
    case F::Condition:
    case F::Default:
    case F::Definition:
    case F::Expression:
    case F::Left:
    case F::Member:
    case F::Objective:
    case F::Operand:
    case F::Result:
    case F::Right:
    case F::Template:
    case F::Value:
    case F::Where:
      return "expression";
    case F::Name:
      return "name";
    case F::Domain:
    case F::Type:
      return "type";
    default:
      return nullptr;
  }
}

/// The operator \a text ends with, or empty if it ends with anything else. A
/// word operator has to end a word, so that `myin` does not look like `in`.
std::string trailing_operator(const std::string& text) {
  size_t end = text.find_last_not_of(" \t\r\n");
  if (end == std::string::npos) {
    return {};
  }
  for (size_t len = std::min<size_t>(end + 1, 8); len > 0; len--) {
    std::string suffix = text.substr(end + 1 - len, len);
    bool isWord = (isalpha(static_cast<unsigned char>(suffix[0])) != 0);
    if (isWord && end + 1 > len &&
        (isalnum(static_cast<unsigned char>(text[end - len])) != 0 || text[end - len] == '_')) {
      continue;
    }
    // Keywords that a construct continues past, so what is missing after them
    // is the expression rather than the keyword itself
    if (suffix == "," || suffix == "=" || suffix == "then" || suffix == "else" ||
        suffix == "elseif" || suffix == "of" || infix_op(suffix) != nullptr) {
      return suffix;
    }
  }
  return {};
}

/// The closer for the last bracket left open in \a text, or 0 if none is. Quotes
/// and comments are skipped so that a bracket inside them does not count.
char unclosed_bracket(const std::string& text) {
  std::vector<char> open;
  for (size_t i = 0; i < text.size(); i++) {
    char c = text[i];
    if (c == '%') {
      while (i < text.size() && text[i] != '\n') {
        i++;
      }
    } else if (c == '/' && i + 1 < text.size() && text[i + 1] == '*') {
      size_t close = text.find("*/", i + 2);
      i = close == std::string::npos ? text.size() : close + 1;
    } else if (c == '"') {
      for (i++; i < text.size() && text[i] != '"'; i++) {
        i += static_cast<size_t>(text[i] == '\\');
      }
    } else if (c == '(' || c == '[' || c == '{') {
      open.push_back(c);
    } else if (c == ')' || c == ']' || c == '}') {
      if (!open.empty()) {
        open.pop_back();
      }
    }
  }
  if (open.empty()) {
    return 0;
  }
  switch (open.back()) {
    case '(':
      return ')';
    case '[':
      return ']';
    default:
      return '}';
  }
}

/// A short name for the construct an error occurred in, to give the message
/// some of the context bison's "expecting ..." list used to provide.
const char* context_name(K k) {
  switch (k) {
    case K::IfThenElse:
      return "if-then-else expression";
    case K::LetExpression:
      return "let expression";
    case K::ArrayComprehension:
      return "array comprehension";
    case K::SetComprehension:
      return "set comprehension";
    case K::ArrayLiteral:
      return "array literal";
    case K::ArrayLiteral2d:
      return "2d array literal";
    case K::ArrayLiteral3d:
      return "3d array literal";
    case K::SetLiteral:
      return "set literal";
    case K::TupleLiteral:
      return "tuple literal";
    case K::RecordLiteral:
      return "record literal";
    case K::StringInterpolation:
      return "string interpolation";
    case K::Call:
      return "call";
    case K::GeneratorCall:
      return "generator call";
    case K::Generator:
      return "generator";
    case K::IndexedAccess:
      return "array access";
    case K::FunctionItem:
      return "function declaration";
    case K::Predicate:
      return "predicate declaration";
    case K::Annotation:
      return "annotation declaration";
    case K::Declaration:
      return "variable declaration";
    case K::Assignment:
      return "assignment";
    case K::Constraint:
      return "constraint item";
    case K::Goal:
      return "solve item";
    case K::Output:
      return "output item";
    case K::Include:
      return "include item";
    case K::Enumeration:
      return "enum declaration";
    case K::TypeAlias:
      return "type alias";
    case K::Parameter:
      return "parameter declaration";
    case K::ArrayType:
    case K::SetType:
    case K::TupleType:
    case K::RecordType:
    case K::ListType:
    case K::TypeBase:
      return "type-inst expression";
    default:
      return nullptr;
  }
}

bool Lowerer::reportSyntaxErrors() {
  if (!ts_node_has_error(_root)) {
    return false;
  }
  collectSyntaxErrors(_root);
  if (!_pp.hadError) {
    // has_error is set but no node claims it; report the whole file rather
    // than lowering a tree we know to be broken
    error(_root, "syntax error");
  }
  return true;
}

TSNode Lowerer::nextToken(TSNode n) {
  for (TSNode cur = n; !ts_node_is_null(cur); cur = ts_node_parent(cur)) {
    TSNode sib = ts_node_next_sibling(cur);
    if (!ts_node_is_null(sib)) {
      while (ts_node_child_count(sib) > 0) {
        sib = ts_node_child(sib, 0);
      }
      return sib;
    }
  }
  return {};
}

F Lowerer::fieldOf(TSNode n) const {
  TSNode parent = ts_node_parent(n);
  if (ts_node_is_null(parent)) {
    return F::END;
  }
  TSTreeCursor cursor = ts_tree_cursor_new(parent);
  F found = F::END;
  if (ts_tree_cursor_goto_first_child(&cursor)) {
    do {
      if (!ts_node_eq(ts_tree_cursor_current_node(&cursor), n)) {
        continue;
      }
      TSFieldId id = ts_tree_cursor_current_field_id(&cursor);
      for (unsigned int i = 0; id != 0 && i < static_cast<unsigned int>(F::END); i++) {
        if (_s.field(static_cast<F>(i)) == id) {
          found = static_cast<F>(i);
          break;
        }
      }
      break;
    } while (ts_tree_cursor_goto_next_sibling(&cursor));
  }
  ts_tree_cursor_delete(&cursor);
  return found;
}

void Lowerer::collectSyntaxErrors(TSNode n) {
  if (ts_node_is_missing(n)) {
    if (const char* role = field_role(fieldOf(n))) {
      error(n, std::string("syntax error, missing ") + role);
    } else {
      error(n, std::string("syntax error, missing `") + ts_node_type(n) + "'");
    }
    return;
  }
  if (ts_node_is_error(n)) {
    // An unterminated string literal leaves the opening quote and the string
    // body inside the error node; say so rather than blaming an earlier token.
    uint32_t children = ts_node_child_count(n);
    for (uint32_t i = 0; i < children; i++) {
      TSNode c = ts_node_child(n, i);
      if (_s.kind(c) == K::StringCharacters || (!ts_node_is_named(c) && text(c) == "\"")) {
        for (uint32_t j = 0; j < children; j++) {
          TSNode q = ts_node_child(n, j);
          if (!ts_node_is_named(q) && text(q) == "\"") {
            error(q, "syntax error, unterminated string literal");
            return;
          }
        }
        break;
      }
    }
    // A nested error node holds text that is not a token at all, which is more
    // precise than the run of tokens the outer node covers.
    TSNode anchor = n;
    for (uint32_t i = 0, count = ts_node_child_count(n); i < count; i++) {
      TSNode c = ts_node_child(n, i);
      if (ts_node_is_error(c)) {
        anchor = c;
        break;
      }
    }
    // The parser stopped after the last token it could use, so when that token
    // is an operator what is missing is the operand it was waiting for.
    std::string t = text(anchor);
    std::string op = trailing_operator(t);
    char closer = unclosed_bracket(t);
    std::string msg;
    if (!op.empty()) {
      msg = "syntax error, missing expression after `" + op + "'";
    } else if (closer != 0) {
      msg = std::string("syntax error, missing `") + closer + "'";
    } else {
      // Name where the trouble starts rather than quoting the whole run
      TSNode first = anchor;
      while (ts_node_child_count(first) > 0) {
        first = ts_node_child(first, 0);
      }
      std::string what = text(first);
      msg = "syntax error, unexpected `" + (what.empty() ? std::string("end of file") : what) + "'";
      TSNode parent = ts_node_parent(n);
      if (!ts_node_is_null(parent)) {
        if (const char* ctx = context_name(_s.kind(parent))) {
          msg += " in ";
          msg += ctx;
        }
      }
    }
    error(anchor, msg);
    return;
  }
  if (!ts_node_has_error(n)) {
    return;
  }
  uint32_t count = ts_node_child_count(n);
  for (uint32_t i = 0; i < count; i++) {
    collectSyntaxErrors(ts_node_child(n, i));
  }
}

// ---------------------------------------------------------------------------
// Items
// ---------------------------------------------------------------------------

void Lowerer::run() {
  std::string pendingDoc;
  bool havePendingDoc = false;
  uint32_t count = ts_node_child_count(_root);
  for (uint32_t i = 0; i < count; i++) {
    TSNode c = ts_node_child(_root, i);
    if (!ts_node_is_named(c)) {
      continue;  // the `;` separators
    }
    switch (_s.kind(c)) {
      case K::FileDocComment: {
        if (_pp.parseDocComments) {
          std::string t = text(c);
          // strip the `/***` and `*/` delimiters
          _pp.model->addDocComment(t.size() > 6 ? t.substr(4, t.size() - 6) : std::string());
        }
        continue;
      }
      case K::DocComment: {
        std::string t = text(c);
        pendingDoc = t.size() > 5 ? t.substr(3, t.size() - 5) : std::string();
        havePendingDoc = true;
        continue;
      }
      case K::LineComment:
      case K::BlockComment:
        continue;
      default:
        break;
    }
    Item* it = item(c, havePendingDoc ? &pendingDoc : nullptr);
    havePendingDoc = false;
    if (it != nullptr) {
      _pp.model->addItem(it);
      GC::unlock();
      GC::lock();
    }
  }
}

void Lowerer::addDocComment(Item* it, const std::string& doc, TSNode n) {
  if (!_pp.parseDocComments) {
    return;
  }
  std::vector<Expression*> args(1);
  args[0] = new StringLit(loc(n), doc);
  Call* c = Call::a(Location(loc(n)), Constants::constants().ann.doc_comment, args);
  Expression::type(c, Type::ann());
  if (auto* fi = Item::dynamicCast<FunctionI>(it)) {
    fi->ann().add(c);
  } else if (auto* vdi = Item::dynamicCast<VarDeclI>(it)) {
    Expression::addAnnotation(vdi->e(), c);
  } else {
    error(n,
          "documentation comments are only supported for function, predicate and variable "
          "declarations");
  }
}

Item* Lowerer::item(TSNode n, const std::string* docComment) {
  Item* it = nullptr;
  switch (_s.kind(n)) {
    case K::Include:
      it = includeItem(n);
      break;
    case K::Constraint:
      it = constraintItem(n);
      break;
    case K::Goal:
      it = goalItem(n);
      break;
    case K::Assignment:
      it = assignItem(n);
      break;
    case K::Declaration:
      it = declarationItem(n);
      break;
    case K::Output:
      it = outputItem(n);
      break;
    case K::Predicate:
    case K::FunctionItem:
      it = operationItem(n);
      break;
    case K::Annotation:
      it = annotationItem(n);
      break;
    case K::Enumeration:
      it = enumerationItem(n);
      break;
    case K::TypeAlias:
      it = typeAliasItem(n);
      break;
    case K::ClassDecl:
      return static_cast<Item*>(futureFeature(n, "class declarations"));
    default:
      error(n, "internal: unhandled item kind '" + std::string(ts_node_type(n)) + "'");
      return nullptr;
  }
  if (it != nullptr && docComment != nullptr) {
    addDocComment(it, *docComment, n);
  }
  return it;
}

Item* Lowerer::includeItem(TSNode n) {
  TSNode fileNode = child(n, F::File);
  if (ts_node_is_null(fileNode)) {
    return nullptr;
  }
  std::string file;
  if (!stringContents(fileNode, file)) {
    return nullptr;
  }
  std::string canonicalName = _pp.canonicalFilename(file);
  auto* ii = new IncludeI(loc(n), ASTString(file));
  auto seen = _pp.seenModels.find(canonicalName);
  if (seen == _pp.seenModels.end()) {
    auto* im = new Model;
    im->setParent(_pp.model);
    im->setFilename(canonicalName);
    std::string fpath = FileUtils::dir_name(_pp.filename);
    if (fpath.empty()) {
      fpath = "./";
    }
    _pp.files.emplace_back(im, ii, fpath, canonicalName, _pp.isSTDLib);
    ii->m(im);
    _pp.seenModels.insert(std::make_pair(canonicalName, im));
  } else {
    ii->m(seen->second, false);
  }
  return ii;
}

Item* Lowerer::constraintItem(TSNode n) {
  Expression* e = expr(child(n, F::Expression));
  if (e == nullptr) {
    return nullptr;
  }
  // `constraint :: <string> <expr>` names the constraint; it is not a general
  // annotation. Annotations on the constraint itself go on the expression
  // (`constraint <expr> :: <ann>`), which is handled by `annotated_expression`.
  std::vector<TSNode> annNodes = childVector(n, F::Annotation);
  if (!annNodes.empty()) {
    if (annNodes.size() > 1) {
      error(n, "a constraint takes at most one name");
      return nullptr;
    }
    K k = _s.kind(annNodes[0]);
    if (k != K::StringLiteral && k != K::StringInterpolation) {
      error(annNodes[0], "the name of a constraint must be a string");
      return nullptr;
    }
    Expression* name = expr(annNodes[0]);
    if (name == nullptr) {
      return nullptr;
    }
    Expression::ann(e).add(Call::a(loc(annNodes[0]), ASTString("mzn_constraint_name"), {name}));
  }
  return new ConstraintI(loc(n), e);
}

Item* Lowerer::goalItem(TSNode n) {
  TSNode strategy = child(n, F::Strategy);
  std::string what = ts_node_is_null(strategy) ? "satisfy" : text(strategy);
  SolveI* si;
  if (what == "satisfy") {
    si = SolveI::sat(loc(n));
  } else {
    Expression* obj = expr(child(n, F::Objective));
    if (obj == nullptr) {
      return nullptr;
    }
    si = what == "minimize" ? SolveI::min(loc(n), obj) : SolveI::max(loc(n), obj);
  }
  si->ann().add(annotations(n));
  return si;
}

Item* Lowerer::assignItem(TSNode n) {
  ASTString name = identifier(child(n, F::Name));
  Expression* e = expr(child(n, F::Definition));
  if (name.empty() || e == nullptr) {
    return nullptr;
  }
  return new AssignI(loc(n), name, e);
}

Item* Lowerer::declarationItem(TSNode n, bool wholeSpan) {
  TypeInst* ti = typeInst(child(n, F::Type));
  TSNode nameNode = child(n, F::Name);
  ASTString name;
  if (ti == nullptr || !patternName(nameNode, name)) {
    return nullptr;
  }
  std::vector<Expression*> anns = annotations(n);
  ParserLocation declLoc = loc(n);
  if (!wholeSpan && has(n, F::Definition)) {
    TSNode last = anns.empty() ? nameNode : childVector(n, F::Annotation).back();
    ParserLocation end = loc(last);
    declLoc = ParserLocation(ASTString(_pp.filename), declLoc.firstLine(), declLoc.firstColumn(),
                             end.lastLine(), end.lastColumn());
  }
  auto* vd = new VarDecl(declLoc, ti, new Id(loc(nameNode), name, nullptr));
  if (!anns.empty()) {
    add_annotations(vd, anns);
  }
  TSNode def = child(n, F::Definition);
  if (!ts_node_is_null(def)) {
    Expression* e = expr(def);
    if (e == nullptr) {
      return nullptr;
    }
    vd->e(e);
  } else if (Expression::type(ti).any() && ti->domain() == nullptr) {
    error(n, "declarations with `any' type-inst require definition");
  }
  return VarDeclI::a(loc(n), vd);
}

Item* Lowerer::outputItem(TSNode n) {
  Expression* e = expr(child(n, F::Expression));
  if (e == nullptr) {
    return nullptr;
  }
  auto* oi = new OutputI(loc(n), e);
  // `output :: <section> <expr>`; the grammar admits only the forms that
  // cannot swallow the expression that follows.
  TSNode section = child(n, F::Annotation);
  if (!ts_node_is_null(section)) {
    Expression* sec = expr(section);
    if (sec == nullptr) {
      return nullptr;
    }
    oi->ann().add(Call::a(loc(n), ASTString("mzn_output_section"), {sec}));
  }
  return oi;
}

std::vector<VarDecl*> Lowerer::parameters(TSNode n, bool& ok) {
  std::vector<VarDecl*> out;
  for (TSNode p : children(n, F::Parameter)) {
    VarDecl* vd = parameter(p);
    if (vd == nullptr) {
      ok = false;
      continue;
    }
    out.push_back(vd);
  }
  return out;
}

VarDecl* Lowerer::annParameter(TSNode n) {
  TSNode ap = child(n, F::AnnotationParameter);
  if (ts_node_is_null(ap)) {
    return nullptr;
  }
  ASTString name = identifier(ap);
  if (name.empty()) {
    return nullptr;
  }
  auto* ti = new TypeInst(loc(n), Type::ann(1));
  auto* vd = new VarDecl(loc(n), ti, new Id(loc(ap), name, nullptr));
  vd->toplevel(false);
  return vd;
}

Item* Lowerer::operationItem(TSNode n) {
  bool isPredicate = _s.kind(n) == K::Predicate;
  TSNode nameNode = child(n, F::Name);
  // `predicate p⁻¹(...)` declares the inverse of `p`, named with the suffix
  bool inverse = _s.kind(nameNode) == K::InversedIdentifier;
  ASTString name = identifier(inverse ? child(nameNode, F::Identifier) : nameNode);
  if (name.empty()) {
    return nullptr;
  }
  if (inverse) {
    name = ASTString(std::string(name.c_str(), name.size()) + "⁻¹");
  }
  TypeInst* ti;
  if (isPredicate) {
    std::string what = text(child(n, F::Type));
    ti = new TypeInst(loc(n), what == "test" ? Type::parbool() : Type::varbool());
  } else {
    ti = typeInst(child(n, F::Type));
    if (ti == nullptr) {
      return nullptr;
    }
    if (Expression::type(ti).any() && ti->domain() == nullptr) {
      error(n, "return type cannot have `any' type-inst without type-inst variable");
    }
  }
  bool ok = true;
  std::vector<VarDecl*> params = parameters(n, ok);
  if (!ok) {
    return nullptr;
  }
  VarDecl* annParam = annParameter(n);
  if (annParam != nullptr) {
    params.push_back(annParam);
  }
  Expression* body = nullptr;
  TSNode bodyNode = child(n, F::Body);
  if (!ts_node_is_null(bodyNode)) {
    body = expr(bodyNode);
    if (body == nullptr) {
      return nullptr;
    }
  }
  auto* fi = new FunctionI(loc(n), name, ti, params, body, _pp.isSTDLib, annParam != nullptr);
  fi->ann().add(annotations(n));
  return fi;
}

Item* Lowerer::annotationItem(TSNode n) {
  ASTString name = identifier(child(n, F::Name));
  if (name.empty()) {
    return nullptr;
  }
  auto* ti = new TypeInst(loc(n), Type::ann());
  bool ok = true;
  std::vector<VarDecl*> params;
  TSNode paramsNode = child(n, F::Parameters);
  if (!ts_node_is_null(paramsNode)) {
    params = parameters(paramsNode, ok);
    if (!ok) {
      return nullptr;
    }
  }
  TSNode bodyNode = child(n, F::Body);
  if (ts_node_is_null(bodyNode) && params.empty()) {
    return VarDeclI::a(loc(n), new VarDecl(loc(n), ti, name));
  }
  Expression* body = nullptr;
  if (!ts_node_is_null(bodyNode)) {
    body = expr(bodyNode);
    if (body == nullptr) {
      return nullptr;
    }
  }
  return new FunctionI(loc(n), name, ti, params, body, _pp.isSTDLib);
}

Item* Lowerer::typeAliasItem(TSNode n) {
  ASTString name = identifier(child(n, F::Name));
  TypeInst* ti = typeInst(child(n, F::Type));
  if (name.empty() || ti == nullptr) {
    return nullptr;
  }
  auto* vd = new VarDecl(loc(n), nullptr, name, ti);
  std::vector<Expression*> anns = annotations(n);
  if (!anns.empty()) {
    add_annotations(vd, anns);
  }
  return VarDeclI::a(loc(n), vd);
}

Item* Lowerer::enumerationItem(TSNode n) {
  ASTString name = identifier(child(n, F::Name));
  if (name.empty()) {
    return nullptr;
  }
  auto* ti = new TypeInst(loc(n), Type::parsetint());
  ti->setIsEnum(true);
  std::vector<Expression*> cases;
  bool stringMembers = false;
  for (TSNode c : children(n, F::Case)) {
    switch (_s.kind(c)) {
      case K::ArrayLiteral: {
        // `enum E = ["a", "b"]` names the members with strings (parser.yxx:567),
        // and is the whole body rather than one case of a `++` chain
        std::vector<Expression*> strs;
        for (TSNode m : namedChildren(c, F::Member)) {
          // A member written with an index is a node of its own, so anything
          // that is not plainly a string is wrong here either way.
          if (_s.kind(m) != K::StringLiteral) {
            error(m, "syntax error, enum members given as an array must be strings");
            return nullptr;
          }
          Expression* e = expr(m);
          if (e == nullptr) {
            return nullptr;
          }
          strs.push_back(e);
        }
        cases.push_back(Call::a(loc(c), Constants::constants().ids.anonEnumFromStrings,
                                {new ArrayLit(loc(c), strs)}));
        stringMembers = true;
        break;
      }
      case K::EnumerationMembers: {
        std::vector<Expression*> ids;
        for (TSNode m : namedChildren(c, F::Member)) {
          ASTString id = identifier(m);
          if (id.empty()) {
            return nullptr;
          }
          ids.push_back(new Id(loc(m), id, nullptr));
        }
        cases.push_back(new SetLit(loc(c), ids));
        break;
      }
      case K::AnonymousEnumeration: {
        std::vector<Expression*> args;
        for (TSNode p : children(c, F::Parameter)) {
          Expression* e = typeAsExpr(p);
          if (e == nullptr) {
            return nullptr;
          }
          args.push_back(e);
        }
        cases.push_back(Call::a(loc(c), Constants::constants().ids.anon_enum_set, args));
        break;
      }
      case K::EnumerationConstructor: {
        ASTString cname = identifier(child(c, F::Name));
        if (cname.empty()) {
          return nullptr;
        }
        std::vector<Expression*> args;
        for (TSNode p : children(c, F::Parameter)) {
          Expression* e = typeAsExpr(child(p, F::Type));
          if (e == nullptr) {
            return nullptr;
          }
          args.push_back(e);
        }
        cases.push_back(Call::a(loc(c), cname, args));
        break;
      }
      default:
        error(c, "internal: unhandled enum case '" + std::string(ts_node_type(c)) + "'");
        return nullptr;
    }
  }
  if (stringMembers && cases.size() > 1) {
    error(n, "syntax error, enum members given as an array cannot be combined with `++'");
    return nullptr;
  }
  VarDecl* vd;
  if (cases.empty()) {
    vd = new VarDecl(loc(n), ti, name);
  } else {
    Expression* e;
    if (cases.size() == 1) {
      e = cases[0];
    } else {
      e = Call::a(loc(n), ASTString("enumFromConstructors"), {new ArrayLit(loc(n), cases)});
    }
    vd = new VarDecl(loc(n), ti, name, e);
  }
  std::vector<Expression*> anns = annotations(n);
  if (!anns.empty()) {
    add_annotations(vd, anns);
  }
  return VarDeclI::a(loc(n), vd);
}

std::vector<Expression*> Lowerer::annotations(TSNode n) {
  std::vector<Expression*> out;
  for (TSNode a : children(n, F::Annotation)) {
    if (Expression* e = expr(a)) {
      out.push_back(e);
    }
  }
  return out;
}

// ---------------------------------------------------------------------------
// Expressions
// ---------------------------------------------------------------------------

Expression* Lowerer::expr(TSNode n) {
  if (ts_node_is_null(n)) {
    return nullptr;
  }
  switch (_s.kind(n)) {
    case K::Identifier:
    case K::QuotedIdentifier: {
      ASTString id = identifier(n);
      return id.empty() ? nullptr : new Id(loc(n), id, nullptr);
    }
    case K::IntegerLiteral:
      return intLiteral(n, false);
    case K::FloatLiteral:
      return floatLiteral(n, false);
    case K::BooleanLiteral:
      return Constants::constants().boollit(_buf[ts_node_start_byte(n)] == 't');
    case K::Infinity:
      // DataZinc folds the sign into the token, so `-infinity` is one node
      return IntLit::a(_buf[ts_node_start_byte(n)] == '-' ? -IntVal::infinity()
                                                          : IntVal::infinity());
    case K::Absent:
      return Constants::constants().absent;
    case K::Anonymous:
      return new AnonVar(loc(n));
    case K::StringLiteral:
      return stringLiteral(n);
    case K::StringInterpolation:
      return stringInterpolation(n);
    case K::SetLiteral: {
      if (text(n) == "∅") {
        return futureFeature(n, "empty set literals written as `∅'");
      }
      std::vector<Expression*> members;
      for (TSNode m : namedChildren(n, F::Member)) {
        Expression* e = expr(m);
        if (e == nullptr) {
          return nullptr;
        }
        members.push_back(e);
      }
      return new SetLit(loc(n), members);
    }
    case K::ArrayLiteral:
      return arrayLiteral(n);
    case K::ArrayLiteral2d:
      return arrayLiteral2d(n);
    case K::ArrayLiteral3d:
      return arrayLiteral3d(n);
    case K::TupleLiteral:
      return tupleLiteral(n);
    case K::RecordLiteral:
      return recordLiteral(n);
    case K::ParenthesisedExpression:
      return expr(child(n, F::Expression));
    case K::AnnotatedExpression: {
      Expression* e = expr(child(n, F::Expression));
      if (e == nullptr) {
        return nullptr;
      }
      std::vector<Expression*> anns = annotations(n);
      if (!anns.empty()) {
        add_annotations(e, anns);
      }
      return e;
    }
    case K::InfixOperator:
      return infixExpr(n);
    case K::PrefixOperator:
      return prefixExpr(n);
    case K::PostfixOperator:
      return postfixExpr(n);
    case K::Call: {
      Expression* c = callExpr(n);
      // Noted here rather than in `callExpr` so that no path out of it -- plain,
      // quoted or inversed name -- can skip the check
      if (_pp.isDatafile && c != nullptr && Expression::isa<Call>(c)) {
        noteDataCall(Expression::cast<Call>(c));
      }
      return c;
    }
    case K::GeneratorCall:
      return generatorCallExpr(n);
    case K::IndexedAccess:
      return indexedAccess(n);
    case K::TupleAccess: {
      Expression* v = expr(child(n, F::Tuple));
      TSNode f = child(n, F::Field);
      if (v == nullptr || ts_node_is_null(f)) {
        return nullptr;
      }
      IntVal idx;
      if (!decimal_to_intval(_buf + ts_node_start_byte(f), _buf + ts_node_end_byte(f), idx)) {
        error(f, "invalid tuple field index");
        return nullptr;
      }
      return new FieldAccess(loc(n), v, IntLit::a(idx));
    }
    case K::RecordAccess: {
      Expression* v = expr(child(n, F::Record));
      TSNode f = child(n, F::Field);
      ASTString field = identifier(f);
      if (v == nullptr || field.empty()) {
        return nullptr;
      }
      return new FieldAccess(loc(n), v, new Id(loc(f), field, nullptr));
    }
    case K::ArrayComprehension:
      return comprehension(n, false);
    case K::SetComprehension:
      return comprehension(n, true);
    case K::IfThenElse:
      return iteExpr(n);
    case K::LetExpression:
      return letExpr(n);
    case K::InversedIdentifier: {
      // `f^-1` on its own is `f` to the power -1; as a call target it names the
      // inverse function (handled in callExpr).
      ASTString id = identifier(child(n, F::Identifier));
      if (id.empty()) {
        return nullptr;
      }
      return new BinOp(loc(n), new Id(loc(n), id, nullptr), BOT_POW, IntLit::a(-1));
    }
    case K::Lambda:
      return futureFeature(n, "lambda expressions");
    case K::CaseExpression:
      return futureFeature(n, "case expressions");
    case K::PatternCall:
    case K::PatternTuple:
    case K::PatternRecord:
    case K::PatternNumericLiteral:
      return futureFeature(n, "pattern matching");
    case K::TypeBase:
    case K::ArrayType:
    case K::SetType:
    case K::TupleType:
    case K::RecordType:
    case K::ListType:
    case K::AnyType:
    case K::OperationType:
    case K::TypeConcatenation:
      return typeAsExpr(n);
    default:
      error(n, "internal: unhandled expression kind '" + std::string(ts_node_type(n)) + "'");
      return nullptr;
  }
}

Expression* Lowerer::infixExpr(TSNode n) {
  TSNode opNode = child(n, F::Operator);
  Expression* lhs = expr(child(n, F::Left));
  Expression* rhs = expr(child(n, F::Right));
  if (lhs == nullptr || rhs == nullptr || ts_node_is_null(opNode)) {
    return nullptr;
  }
  if (_s.kind(opNode) == K::BacktickIdentifier) {
    std::string t = text(opNode);
    std::string name = t.substr(1, t.size() - 2);
    if (name.empty()) {
      error(opNode, "syntax error, empty operator name");
      return nullptr;
    }
    return Call::a(loc(n), ASTString(name), {lhs, rhs});
  }
  std::string op = text(opNode);
  const OpEntry* e = infix_op(op);
  if (e == nullptr) {
    error(opNode, "internal: unhandled operator '" + op + "'");
    return nullptr;
  }
  if (e->call != nullptr) {
    return Call::a(loc(n), ASTString(e->call), {lhs, rhs});
  }
  if (e->bot == BOT_DOTDOT && Expression::isa<IntLit>(lhs) && Expression::isa<IntLit>(rhs)) {
    return new SetLit(loc(n), IntSetVal::a(IntLit::v(Expression::cast<IntLit>(lhs)),
                                           IntLit::v(Expression::cast<IntLit>(rhs))));
  }
  return new BinOp(loc(n), lhs, e->bot, rhs);
}

Expression* Lowerer::prefixExpr(TSNode n) {
  TSNode opNode = child(n, F::Operator);
  TSNode operand = child(n, F::Operand);
  std::string op = text(opNode);
  if (const char* call = open_range_call(op, false)) {
    Expression* e = expr(operand);
    return e == nullptr ? nullptr : Call::a(loc(n), ASTString(call), {e});
  }
  if (op == "not" || op == "¬") {
    Expression* e = expr(operand);
    return e == nullptr ? nullptr : new UnOp(loc(n), UOT_NOT, e);
  }
  // Fold the sign into the literal, so that -9223372036854775808 is
  // representable and so that the AST matches the bison parser's.
  if (op == "-" && _s.kind(operand) == K::IntegerLiteral) {
    return intLiteral(operand, true);
  }
  if (op == "-" && _s.kind(operand) == K::FloatLiteral) {
    return floatLiteral(operand, true);
  }
  Expression* e = expr(operand);
  if (e == nullptr) {
    return nullptr;
  }
  if (op == "+") {
    if (Expression::isa<IntLit>(e) || Expression::isa<FloatLit>(e)) {
      return e;
    }
    return new UnOp(loc(n), UOT_PLUS, e);
  }
  if (Expression::isa<IntLit>(e)) {
    return IntLit::a(-IntLit::v(Expression::cast<IntLit>(e)));
  }
  if (Expression::isa<FloatLit>(e)) {
    return FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>(e)));
  }
  return new UnOp(loc(n), UOT_MINUS, e);
}

Expression* Lowerer::postfixExpr(TSNode n) {
  TSNode opNode = child(n, F::Operator);
  std::string op = text(opNode);
  Expression* e = expr(child(n, F::Operand));
  if (e == nullptr) {
    return nullptr;
  }
  if (op == "^-1" || op == "⁻¹") {
    if (e == Constants::constants().absent) {
      return e;  // `<>^-1` is `<>`, as in the bison grammar
    }
    return new BinOp(loc(n), e, BOT_POW, IntLit::a(-1));
  }
  if (const char* call = open_range_call(op, true)) {
    return Call::a(loc(n), ASTString(call), {e});
  }
  error(opNode, "internal: unhandled postfix operator '" + op + "'");
  return nullptr;
}

Expression* Lowerer::argument(TSNode arg) {
  // DataZinc has no named or defaulted arguments, so it points the `argument`
  // field straight at the expression rather than wrapping it in an arg_or_param.
  if (_s.kind(arg) != K::ArgOrParam) {
    return expr(arg);
  }
  TSNode def = child(arg, F::Default);
  if (!ts_node_is_null(def)) {
    return futureFeature(def, "default values for call arguments");
  }
  TSNode e = child(arg, F::Expression);
  if (ts_node_is_null(e)) {
    return typeAsExpr(child(arg, F::Type));
  }
  // Named argument: the name shares the grammar rule with a type
  TSNode nameNode = child(arg, F::Type);
  Expression* nameExpr = typeAsExpr(nameNode);
  auto* id = Expression::dynamicCast<Id>(nameExpr);
  if (id == nullptr) {
    error(nameNode, "invalid name for named argument");
    return nullptr;
  }
  if (!id->v().empty() && id->v().c_str()[0] == '_') {
    std::ostringstream oss;
    oss << "parameter `" << id->v()
        << "' starts with '_' and cannot be used as a named argument (pass it positionally)";
    error(nameNode, oss.str());
    return nullptr;
  }
  Expression* value = expr(e);
  if (value == nullptr) {
    return nullptr;
  }
  return new VarDecl(loc(arg), new TypeInst(loc(arg), Type()), id->v(), value);
}

std::vector<Expression*> Lowerer::callArguments(TSNode n, bool& ok) {
  std::vector<Expression*> args;
  for (TSNode a : children(n, F::Argument)) {
    Expression* e = argument(a);
    if (e == nullptr) {
      ok = false;
      continue;
    }
    args.push_back(e);
  }
  return args;
}

void Lowerer::noteDataCall(Call* c) {
  for (const char* f : DATA_FUNCTIONS) {
    if (c->id() == f) {
      return;
    }
  }
  _pp.dataFileCalls.push_back(c);
}

Expression* Lowerer::callExpr(TSNode n) {
  TSNode fn = child(n, F::Function);
  // There are no first-class functions, so a call of a call is a generator call
  // whose head is not made of generators, such as `forall(i: 1..3)(p(i))`.
  if (_s.kind(fn) == K::Call) {
    for (TSNode a : children(fn, F::Argument)) {
      if (has(a, F::Expression)) {
        error(a, "illegal expression in generator call");
        return nullptr;
      }
    }
    error(fn, "illegal expression in generator call");
    return nullptr;
  }
  bool ok = true;
  std::vector<Expression*> args = callArguments(n, ok);
  if (!ok) {
    return nullptr;
  }
  if (_s.kind(fn) == K::Anonymous) {
    return Call::a(loc(n), Constants::constants().ids.anon_enum_set, args);
  }
  if (_s.kind(fn) == K::InversedIdentifier) {
    ASTString id = identifier(child(fn, F::Identifier));
    if (id.empty()) {
      return nullptr;
    }
    return Call::a(loc(n), ASTString(std::string(id.c_str(), id.size()) + "⁻¹"), args);
  }
  if (_s.kind(fn) == K::QuotedIdentifier) {
    std::string t = text(fn);
    std::string name = t.substr(1, t.size() - 2);
    if (const QuotedOp* q = quoted_op(name)) {
      if (q->bot == -1) {
        if (args.size() != 1) {
          error(n, "syntax error, unary operator with two arguments");
          return nullptr;
        }
        return new UnOp(loc(n), UOT_NOT, args[0]);
      }
      if (args.size() != 2) {
        error(n, "syntax error, binary operator with unary argument list");
        return nullptr;
      }
      auto bot = static_cast<BinOpType>(q->bot);
      if (bot == BOT_DOTDOT && Expression::isa<IntLit>(args[0]) &&
          Expression::isa<IntLit>(args[1])) {
        return new SetLit(loc(n), IntSetVal::a(IntLit::v(Expression::cast<IntLit>(args[0])),
                                               IntLit::v(Expression::cast<IntLit>(args[1]))));
      }
      return new BinOp(loc(n), args[0], bot, args[1]);
    }
    // `'..<'`, `'<..'` and `'<..<'` stay calls, with either one or two operands
    return Call::a(loc(n), identifier(fn), args);
  }
  if (_s.kind(fn) != K::Identifier) {
    error(fn, "invalid function name in call");
    return nullptr;
  }
  std::string name = text(fn);
  if (is_reserved_call_name(name)) {
    error(fn, "syntax error, `" + name + "' is a reserved keyword");
    return nullptr;
  }
  return Call::a(loc(n), identifier(fn), args);
}

Expression* Lowerer::generatorCallExpr(TSNode n) {
  TSNode fn = child(n, F::Function);
  Generators gens;
  if (!generators(n, gens, /*idLocations=*/true)) {
    return nullptr;
  }
  Expression* tmpl = expr(child(n, F::Template));
  if (tmpl == nullptr) {
    return nullptr;
  }
  auto* comp = new Comprehension(loc(n), tmpl, gens, false);
  ASTString name;
  if (_s.kind(fn) == K::InversedIdentifier) {
    ASTString id = identifier(child(fn, F::Identifier));
    if (id.empty()) {
      return nullptr;
    }
    name = ASTString(std::string(id.c_str(), id.size()) + "⁻¹");
  } else if (_s.kind(fn) == K::Identifier || _s.kind(fn) == K::QuotedIdentifier) {
    name = identifier(fn);
  } else {
    error(fn, "illegal expression in generator call");
    return nullptr;
  }
  return Call::a(loc(n), name, {comp});
}

Expression* Lowerer::indexedAccess(TSNode n) {
  Expression* v = expr(child(n, F::Collection));
  if (v == nullptr) {
    return nullptr;
  }
  std::vector<Expression*> idx;
  for (TSNode i : children(n, F::Index)) {
    // A bare range operator token is a slice covering the whole dimension
    if (!ts_node_is_named(i)) {
      std::string op = text(i);
      if (op == "..") {
        idx.push_back(new SetLit(loc(i), IntSetVal::a(-IntVal::infinity(), IntVal::infinity())));
      } else {
        idx.push_back(Call::a(loc(i), ASTString("'" + op + "'"), std::vector<Expression*>()));
      }
      continue;
    }
    Expression* e = expr(i);
    if (e == nullptr) {
      return nullptr;
    }
    idx.push_back(e);
  }
  return new ArrayAccess(loc(n), v, idx);
}

Expression* Lowerer::arrayLiteral(TSNode n) {
  // Only a leading run of members may carry indices; `[0: x, y]` means the
  // array starts at index 0, so the index and value lists may differ in length.
  std::vector<Expression*> indices;
  std::vector<Expression*> values;
  for (TSNode m : namedChildren(n, F::Member)) {
    // Only a member written with an index gets a node of its own; a plain one
    // is the value itself.
    bool isIndexed = _s.kind(m) == K::ArrayLiteralMember;
    Expression* v = expr(isIndexed ? child(m, F::Value) : m);
    if (v == nullptr) {
      return nullptr;
    }
    if (!isIndexed) {
      if (indices.size() > 1) {
        error(n, "invalid array literal, mixing indexed and non-indexed values");
        return nullptr;
      }
      values.push_back(v);
      continue;
    }
    if (indices.size() != values.size()) {
      error(n, "invalid array literal, mixing indexed and non-indexed values");
      return nullptr;
    }
    Expression* i = expr(child(m, F::Index));
    if (i == nullptr) {
      return nullptr;
    }
    auto* tup = Expression::dynamicCast<ArrayLit>(i);
    if (tup != nullptr && tup->isTuple() && tup->size() == 1) {
      i = (*tup)[0];
    }
    indices.push_back(i);
    values.push_back(v);
  }
  if (indices.empty()) {
    return new ArrayLit(loc(n), values);
  }
  // A tuple index means a multi-dimensional index set
  const auto* tuple = Expression::dynamicCast<ArrayLit>(indices[0]);
  if (tuple == nullptr) {
    for (const auto* t : indices) {
      if (Expression::isa<ArrayLit>(t)) {
        error(n, "syntax error, non-uniform indexed array literal");
        return nullptr;
      }
    }
    return Call::a(loc(n), "arrayNd",
                   {new ArrayLit(loc(n), indices), new ArrayLit(loc(n), values)});
  }
  if (indices.size() != values.size()) {
    error(n, "syntax error, non-uniform indexed array literal");
    return nullptr;
  }
  std::vector<std::vector<Expression*>> dims(tuple->size());
  for (const auto* t : indices) {
    const auto* tup = Expression::dynamicCast<ArrayLit>(t);
    if (tup == nullptr || tup->size() != dims.size()) {
      error(n, "syntax error, non-uniform indexed array literal");
      return nullptr;
    }
    for (unsigned int i = 0; i < dims.size(); i++) {
      dims[i].push_back((*tup)[i]);
    }
  }
  std::vector<Expression*> arrayNdArgs(dims.size());
  for (unsigned int i = 0; i < dims.size(); i++) {
    arrayNdArgs[i] = new ArrayLit(loc(n), dims[i]);
  }
  arrayNdArgs.push_back(new ArrayLit(loc(n), values));
  return Call::a(loc(n), "arrayNd", arrayNdArgs);
}

Expression* Lowerer::arrayLiteral2d(TSNode n) {
  std::vector<Expression*> columnHeader;
  for (TSNode c : children(n, F::ColumnIndex)) {
    Expression* e = expr(c);
    if (e == nullptr) {
      return nullptr;
    }
    columnHeader.push_back(e);
  }
  std::vector<Expression*> rowHeader;
  std::vector<std::vector<Expression*>> rows;
  for (TSNode r : children(n, F::Row)) {
    TSNode idxNode = child(r, F::Index);
    bool hasIndex = !ts_node_is_null(idxNode);
    // Every row carries an index or none does. A row header shorter than the
    // rows would leave `nCols` below to invent a shape.
    if (!rows.empty() && hasIndex == rowHeader.empty()) {
      error(r, "syntax error, mixing indexed and non-indexed sub-arrays in 2d array literal");
      return nullptr;
    }
    if (hasIndex) {
      Expression* e = expr(idxNode);
      if (e == nullptr) {
        return nullptr;
      }
      rowHeader.push_back(e);
    }
    std::vector<Expression*> row;
    for (TSNode m : children(r, F::Member)) {
      Expression* e = expr(m);
      if (e == nullptr) {
        return nullptr;
      }
      row.push_back(e);
    }
    if (!rows.empty() && row.size() != rows.back().size()) {
      error(r, "syntax error, all sub-arrays of 2d array literal must have the same length");
      return nullptr;
    }
    rows.push_back(row);
  }
  if (!columnHeader.empty() && !rows.empty() && rows[0].size() != columnHeader.size()) {
    error(n, "syntax error, sub-array of 2d array literal has different length from index row");
    return nullptr;
  }
  if (columnHeader.empty() && rowHeader.empty()) {
    return new ArrayLit(loc(n), rows);
  }
  std::vector<Expression*> flat;
  for (auto& row : rows) {
    for (auto* e : row) {
      flat.push_back(e);
    }
  }
  if (rowHeader.empty()) {
    auto nRows = columnHeader.empty() ? 0 : flat.size() / columnHeader.size();
    rowHeader.resize(nRows);
    for (unsigned int i = 0; i < nRows; i++) {
      rowHeader[i] = IntLit::a(i + 1);
    }
  } else if (columnHeader.empty()) {
    auto nCols = rowHeader.empty() ? 0 : flat.size() / rowHeader.size();
    columnHeader.resize(nCols);
    for (unsigned int i = 0; i < nCols; i++) {
      columnHeader[i] = IntLit::a(i + 1);
    }
  }
  return Call::a(loc(n), "array2d",
                 {new ArrayLit(loc(n), rowHeader), new ArrayLit(loc(n), columnHeader),
                  new ArrayLit(loc(n), flat)});
}

Expression* Lowerer::arrayLiteral3d(TSNode n) {
  std::vector<std::vector<std::vector<Expression*>>> slices;
  for (TSNode s : children(n, F::Slice)) {
    std::vector<std::vector<Expression*>> rows;
    for (TSNode r : children(s, F::Row)) {
      std::vector<Expression*> row;
      for (TSNode m : namedChildren(r, F::Member)) {
        Expression* e = expr(m);
        if (e == nullptr) {
          return nullptr;
        }
        row.push_back(e);
      }
      rows.push_back(row);
    }
    slices.push_back(rows);
  }
  std::vector<std::pair<int, int>> dims(3);
  dims[0] = {1, static_cast<int>(slices.size())};
  dims[1] = {1, slices.empty() ? 0 : static_cast<int>(slices[0].size())};
  dims[2] = {1, (slices.empty() || slices[0].empty()) ? 0 : static_cast<int>(slices[0][0].size())};
  std::vector<Expression*> flat;
  for (const auto& rows : slices) {
    if (static_cast<int>(rows.size()) != dims[1].second) {
      error(n, "syntax error, all sub-arrays of 3d array literal must have the same length");
      return nullptr;
    }
    for (const auto& row : rows) {
      if (static_cast<int>(row.size()) != dims[2].second) {
        error(n, "syntax error, all sub-arrays of 3d array literal must have the same length");
        return nullptr;
      }
      for (auto* e : row) {
        flat.push_back(e);
      }
    }
  }
  return new ArrayLit(loc(n), flat, dims);
}

Expression* Lowerer::tupleLiteral(TSNode n) {
  std::vector<Expression*> members;
  for (TSNode m : namedChildren(n, F::Member)) {
    Expression* e = expr(m);
    if (e == nullptr) {
      return nullptr;
    }
    members.push_back(e);
  }
  return ArrayLit::constructTuple(loc(n), members);
}

Expression* Lowerer::recordLiteral(TSNode n) {
  std::vector<Expression*> fields;
  for (TSNode m : namedChildren(n, F::Member)) {
    ASTString name = identifier(child(m, F::Name));
    Expression* value = expr(child(m, F::Value));
    if (name.empty() || value == nullptr) {
      return nullptr;
    }
    fields.push_back(new VarDecl(loc(m), new TypeInst(loc(m), Type()), name, value));
  }
  if (fields.empty()) {
    error(n, "syntax error, empty record literal");
    return nullptr;
  }
  ArrayLit* al = ArrayLit::constructTuple(loc(n), fields);
  Expression::type(al, Type::record());
  return al;
}

bool Lowerer::generators(TSNode n, Generators& gens, bool idLocations) {
  for (TSNode g : children(n, F::Generator)) {
    if (_s.kind(g) == K::AssignmentGenerator) {
      TSNode nameNode = child(g, F::Name);
      ASTString name;
      if (!patternName(nameNode, name)) {
        return false;
      }
      Expression* value = expr(child(g, F::Value));
      if (value == nullptr) {
        return false;
      }
      if (idLocations) {
        std::vector<Id*> ids{new Id(loc(nameNode), name, nullptr)};
        gens.g.emplace_back(ids, nullptr, value);
      } else {
        std::vector<std::string> ids{std::string(name.c_str(), name.size())};
        gens.g.emplace_back(ids, nullptr, value);
      }
      TSNode where = child(g, F::Where);
      if (!ts_node_is_null(where)) {
        Expression* w = expr(where);
        if (w == nullptr) {
          return false;
        }
        gens.g.emplace_back(static_cast<int>(gens.g.size()), w);
      }
      continue;
    }
    // `_` reaches here as an empty name, and only Generator's std::string
    // overload turns that into an anonymous variable
    std::vector<std::string> ids;
    std::vector<Id*> idExprs;
    for (TSNode nameNode : children(g, F::Name)) {
      ASTString name;
      if (!patternName(nameNode, name)) {
        return false;
      }
      ids.emplace_back(name.c_str(), name.size());
      if (idLocations) {
        idExprs.push_back(new Id(loc(nameNode), name, nullptr));
      }
    }
    Expression* collection = expr(child(g, F::Collection));
    if (collection == nullptr) {
      return false;
    }
    Expression* where = nullptr;
    TSNode whereNode = child(g, F::Where);
    if (!ts_node_is_null(whereNode)) {
      where = expr(whereNode);
      if (where == nullptr) {
        return false;
      }
    }
    if (idLocations) {
      gens.g.emplace_back(idExprs, collection, where);
    } else {
      gens.g.emplace_back(ids, collection, where);
    }
  }
  return true;
}

Expression* Lowerer::comprehension(TSNode n, bool isSet) {
  Generators gens;
  if (!generators(n, gens)) {
    return nullptr;
  }
  Expression* tmpl = expr(child(n, F::Template));
  if (tmpl == nullptr) {
    return nullptr;
  }
  TSNode idxNode = child(n, F::Index);
  if (!ts_node_is_null(idxNode)) {
    Expression* idx = expr(idxNode);
    if (idx == nullptr) {
      return nullptr;
    }
    std::vector<Expression*> tv;
    if (auto* al = Expression::dynamicCast<ArrayLit>(idx)) {
      for (unsigned int i = 0; i < al->size(); i++) {
        tv.push_back((*al)[i]);
      }
    } else {
      tv.push_back(idx);
    }
    tv.push_back(tmpl);
    auto* t = ArrayLit::constructTuple(loc(n), tv);
    Type ty = Type::tuple();
    ty.typeId(Type::COMP_INDEX);
    t->type(ty);
    tmpl = t;
  }
  return new Comprehension(loc(n), tmpl, gens, isSet);
}

Expression* Lowerer::iteExpr(TSNode n) {
  std::vector<TSNode> conds = childVector(n, F::Condition);
  std::vector<TSNode> results = childVector(n, F::Result);
  if (conds.size() != results.size()) {
    error(n, "internal: malformed if-then-else");
    return nullptr;
  }
  std::vector<Expression*> ifThen;
  for (size_t i = 0; i < conds.size(); i++) {
    Expression* c = expr(conds[i]);
    Expression* r = expr(results[i]);
    if (c == nullptr || r == nullptr) {
      return nullptr;
    }
    ifThen.push_back(c);
    ifThen.push_back(r);
  }
  Expression* elseE = nullptr;
  TSNode elseNode = child(n, F::Else);
  if (!ts_node_is_null(elseNode)) {
    elseE = expr(elseNode);
    if (elseE == nullptr) {
      return nullptr;
    }
  }
  return new ITE(loc(n), ifThen, elseE);
}

Expression* Lowerer::letExpr(TSNode n) {
  std::vector<Expression*> lets;
  for (TSNode l : children(n, F::Item)) {
    if (_s.kind(l) == K::Constraint) {
      Expression* e = expr(child(l, F::Expression));
      if (e == nullptr) {
        return nullptr;
      }
      std::vector<Expression*> anns = annotations(l);
      if (!anns.empty()) {
        add_annotations(e, anns);
      }
      lets.push_back(e);
      continue;
    }
    Item* it = declarationItem(l, /*wholeSpan=*/true);
    if (it == nullptr) {
      return nullptr;
    }
    VarDecl* vd = it->cast<VarDeclI>()->e();
    vd->toplevel(false);
    lets.push_back(vd);
  }
  Expression* in = expr(child(n, F::In));
  if (in == nullptr) {
    return nullptr;
  }
  if (lets.empty()) {
    return in;
  }
  return new Let(loc(n), lets, in);
}

// ---------------------------------------------------------------------------
// Literals
// ---------------------------------------------------------------------------

void Lowerer::signedLiteralText(TSNode n, bool& negated, const char*& b, const char*& e) const {
  b = _buf + ts_node_start_byte(n);
  e = _buf + ts_node_end_byte(n);
  if (b != e && *b == '-') {
    negated = !negated;
    b++;
  }
}

Expression* Lowerer::intLiteral(TSNode n, bool negated) {
  const char* b;
  const char* e;
  signedLiteralText(n, negated, b, e);
  auto len = static_cast<size_t>(e - b);
  IntVal v;
  bool ok;
  if (len > 2 && b[0] == '0' && (b[1] == 'x' || b[1] == 'X')) {
    ok = based_to_intval(b + 2, e, 16, v);
  } else if (len > 2 && b[0] == '0' && b[1] == 'o') {
    ok = based_to_intval(b + 2, e, 8, v);
  } else if (len > 2 && b[0] == '0' && b[1] == 'b') {
    ok = based_to_intval(b + 2, e, 2, v);
  } else if (negated && len == 19 && memcmp(b, "9223372036854775808", 19) == 0) {
    return IntLit::a(IntVal(-9223372036854775807LL - 1));
  } else {
    ok = decimal_to_intval(b, e, v);
  }
  if (!ok) {
    error(n, "invalid integer literal");
    return nullptr;
  }
  return IntLit::a(negated ? -v : v);
}

Expression* Lowerer::floatLiteral(TSNode n, bool negated) {
  const char* b;
  const char* e;
  signedLiteralText(n, negated, b, e);
  // strtod needs a terminated string. Copying into a buffer keeps the common
  // literal off the heap; truncating an over-long one would silently change
  // its value, so that case falls back to a string.
  char stack[64];
  std::string heap;
  const char* buf;
  auto len = static_cast<size_t>(e - b);
  if (len < sizeof(stack)) {
    memcpy(stack, b, len);
    stack[len] = '\0';
    buf = stack;
  } else {
    heap.assign(b, e);
    buf = heap.c_str();
  }
#ifdef _WIN32
  double v = _strtod_l(buf, nullptr, _pp.cLocale);
#else
  double v = strtod_l(buf, nullptr, _pp.cLocale);
#endif
  // Deliberately no ERANGE check: denormals such as 4.9e-324 are accepted.
  if (!std::isfinite(v)) {
    error(n, "invalid float literal");
    return nullptr;
  }
  return FloatLit::a(negated ? -v : v);
}

bool Lowerer::stringContents(TSNode n, std::string& out) {
  uint32_t count = ts_node_child_count(n);
  for (uint32_t i = 0; i < count; i++) {
    if (!appendStringPiece(ts_node_child(n, i), out)) {
      return false;
    }
  }
  return true;
}

bool Lowerer::appendStringPiece(TSNode n, std::string& out) {
  {
    switch (_s.kind(n)) {
      case K::StringCharacters:
        out.append(_buf + ts_node_start_byte(n), _buf + ts_node_end_byte(n));
        break;
      case K::EscapeSequence: {
        std::string e = text(n);
        if (e.size() < 2) {
          error(n, "invalid escape sequence");
          return false;
        }
        switch (e[1]) {
          case 'n':
            out += '\n';
            break;
          case 't':
            out += '\t';
            break;
          case 'r':
            out += '\r';
            break;
          case '\\':
          case '\'':
          case '"':
            out += e[1];
            break;
          case 'x': {
            IntVal v;
            if (!based_to_intval(e.data() + 2, e.data() + e.size(), 16, v)) {
              error(n, "invalid escape sequence");
              return false;
            }
            out += static_cast<char>(v.toInt());
            break;
          }
          case 'u':
          case 'U': {
            IntVal v;
            if (!based_to_intval(e.data() + 2, e.data() + e.size(), 16, v)) {
              error(n, "invalid escape sequence");
              return false;
            }
            // Unicode stops at U+10FFFF, and the surrogate halves are not
            // characters. Without this the 4-byte branch below truncates and
            // writes bytes that are not valid UTF-8.
            if (v < 0 || v > 0x10FFFF || (v >= 0xD800 && v <= 0xDFFF)) {
              error(n, "invalid code point in escape sequence");
              return false;
            }
            // encode the code point as UTF-8
            auto cp = static_cast<unsigned long>(v.toInt());
            if (cp < 0x80) {
              out += static_cast<char>(cp);
            } else if (cp < 0x800) {
              out += static_cast<char>(0xc0 | (cp >> 6));
              out += static_cast<char>(0x80 | (cp & 0x3f));
            } else if (cp < 0x10000) {
              out += static_cast<char>(0xe0 | (cp >> 12));
              out += static_cast<char>(0x80 | ((cp >> 6) & 0x3f));
              out += static_cast<char>(0x80 | (cp & 0x3f));
            } else {
              out += static_cast<char>(0xf0 | (cp >> 18));
              out += static_cast<char>(0x80 | ((cp >> 12) & 0x3f));
              out += static_cast<char>(0x80 | ((cp >> 6) & 0x3f));
              out += static_cast<char>(0x80 | (cp & 0x3f));
            }
            break;
          }
          default: {
            // octal
            IntVal v;
            if (!based_to_intval(e.data() + 1, e.data() + e.size(), 8, v)) {
              error(n, "invalid escape sequence");
              return false;
            }
            out += static_cast<char>(v.toInt());
            break;
          }
        }
        break;
      }
      default:
        break;  // the surrounding quotes
    }
  }
  return true;
}

Expression* Lowerer::stringLiteral(TSNode n) {
  std::string s;
  if (!stringContents(n, s)) {
    return nullptr;
  }
  return new StringLit(loc(n), s);
}

Expression* Lowerer::stringInterpolation(TSNode n) {
  // The string parts arrive as `string_characters`/`escape_sequence` children
  // and the interpolated expressions as the remaining named children.
  struct Part {
    bool isString;
    std::string s;
    Expression* e;
  };
  std::vector<Part> parts;
  // Each part is wrapped in an unnamed alias node (`string` or `expression`)
  // holding the real node; the remaining children are the delimiters.
  uint32_t count = ts_node_child_count(n);
  for (uint32_t i = 0; i < count; i++) {
    TSNode wrapper = ts_node_child(n, i);
    uint32_t inner = ts_node_child_count(wrapper);
    if (inner == 0) {
      continue;  // the quotes and the `\(` ... `)` delimiters
    }
    K first = _s.kind(ts_node_child(wrapper, 0));
    if (first == K::StringCharacters || first == K::EscapeSequence) {
      if (parts.empty() || !parts.back().isString) {
        parts.push_back({true, {}, nullptr});
      }
      for (uint32_t j = 0; j < inner; j++) {
        if (!appendStringPiece(ts_node_child(wrapper, j), parts.back().s)) {
          return nullptr;
        }
      }
      continue;
    }
    Expression* e = expr(ts_node_child(wrapper, 0));
    if (e == nullptr) {
      return nullptr;
    }
    parts.push_back({false, {}, e});
  }
  // Normalised to alternate string/expression, starting and ending with a
  // string, so that the result matches what the bison parser builds.
  Location l(loc(n));
  std::vector<Part> norm;
  bool wantString = true;
  for (auto& p : parts) {
    if (p.isString != wantString) {
      norm.push_back({true, {}, nullptr});
      wantString = !wantString;
    }
    norm.push_back(p);
    wantString = !wantString;
  }
  if (norm.empty() || !norm.back().isString) {
    norm.push_back({true, {}, nullptr});
  }
  Expression* result = new StringLit(l, norm.back().s);
  for (size_t i = norm.size() - 1; i > 0; i--) {
    Part& p = norm[i - 1];
    Expression* lhs = p.isString ? static_cast<Expression*>(new StringLit(l, p.s))
                                 : Call::a(l, ASTString("format"), {p.e});
    result = new BinOp(l, lhs, BOT_PLUSPLUS, result);
  }
  return result;
}

// ---------------------------------------------------------------------------
// Names and patterns
// ---------------------------------------------------------------------------

ASTString Lowerer::identifier(TSNode n) {
  if (ts_node_is_null(n)) {
    return {};
  }
  switch (_s.kind(n)) {
    case K::Identifier:
      return ASTString(text(n));
    case K::QuotedIdentifier: {
      if (_pp.isFlatZinc) {
        error(n, "quoted identifiers are not allowed in FlatZinc");
        return {};
      }
      std::string t = text(n);
      std::string content = t.substr(1, t.size() - 2);
      if (const char* op = quoted_op_name(content)) {
        return ASTString(op);
      }
      if (content.empty()) {
        // Callers spell failure as an empty name, so this would delete the item
        error(n, "syntax error, empty quoted identifier");
        return {};
      }
      return ASTString(content);
    }
    default:
      error(n, "expected an identifier");
      return {};
  }
}

bool Lowerer::patternName(TSNode n, ASTString& out) {
  if (ts_node_is_null(n)) {
    return false;
  }
  switch (_s.kind(n)) {
    case K::Identifier:
    case K::QuotedIdentifier:
      out = identifier(n);
      return !out.empty();
    case K::Anonymous:
      out = ASTString("");
      return true;
    default:
      futureFeature(n, "destructuring patterns");
      return false;
  }
}

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

Expression* Lowerer::typeAsExpr(TSNode n) {
  if (ts_node_is_null(n)) {
    return nullptr;
  }
  if (_s.kind(n) == K::TypeConcatenation) {
    // `a ++ b` in a position that also admits a type
    Expression* lhs = typeAsExpr(child(n, F::Left));
    Expression* rhs = typeAsExpr(child(n, F::Right));
    if (lhs == nullptr || rhs == nullptr) {
      return nullptr;
    }
    return new BinOp(loc(n), lhs, BOT_PLUSPLUS, rhs);
  }
  if (_s.kind(n) != K::TypeBase) {
    error(n, "expected an expression, found a type");
    return nullptr;
  }
  if (has(n, F::VarPar) || has(n, F::Opt) || has(n, F::Any)) {
    error(n, "expected an expression, found a type");
    return nullptr;
  }
  TSNode domain = child(n, F::Domain);
  switch (_s.kind(domain)) {
    case K::PrimitiveType:
    case K::TypeInstId:
    case K::TypeInstEnumId:
      error(domain, "expected an expression, found a type");
      return nullptr;
    case K::NewType:
      return futureFeature(domain, "object types");
    default:
      return expr(domain);
  }
}

TypeInst* Lowerer::typeInst(TSNode n) {
  if (ts_node_is_null(n)) {
    return nullptr;
  }
  switch (_s.kind(n)) {
    case K::TypeBase:
      return typeBase(n);
    case K::ArrayType:
      return arrayTypeInst(n);
    case K::ListType: {
      // `list of T` is `array[1..infinity] of T`
      TypeInst* inner = typeInst(child(n, F::Type));
      if (inner == nullptr) {
        return nullptr;
      }
      TypeInst* ti = inner->isarray() ? new TypeInst(loc(n), Type::tuple(), inner) : inner;
      std::vector<TypeInst*> ranges(1);
      ranges[0] =
          new TypeInst(loc(n), Type(),
                       new BinOp(loc(n), IntLit::a(1), BOT_DOTDOT, IntLit::a(IntVal::infinity())));
      ti->setRanges(ranges);
      return ti;
    }
    case K::SetType: {
      TypeInst* inner = typeInst(child(n, F::Type));
      if (inner == nullptr) {
        return nullptr;
      }
      Type tt = Expression::type(inner);
      tt.st(Type::ST_SET);
      applyVarParOpt(n, tt);
      TSNode card = child(n, F::Cardinality);
      if (!ts_node_is_null(card)) {
        Expression* c = expr(card);
        if (c == nullptr) {
          return nullptr;
        }
        ArrayLit* marker = ArrayLit::constructTuple(loc(n), {c, inner});
        auto* ti = new TypeInst(loc(n), tt, marker);
        ti->setIsEnum(inner->isEnum());
        return ti;
      }
      inner->type(tt);
      return inner;
    }
    case K::TupleType: {
      std::vector<Expression*> fields;
      for (TSNode f : children(n, F::Field)) {
        TypeInst* ti = typeInst(f);
        if (ti == nullptr) {
          return nullptr;
        }
        fields.push_back(ti);
      }
      Type tt = Type::tuple();
      applyVarParOpt(n, tt);
      return new TypeInst(loc(n), tt, ArrayLit::constructTuple(loc(n), fields));
    }
    case K::RecordType: {
      std::vector<Expression*> fields;
      for (TSNode f : children(n, F::Field)) {
        TypeInst* fti = typeInst(child(f, F::Type));
        ASTString fname = identifier(child(f, F::Name));
        if (fti == nullptr || fname.empty()) {
          return nullptr;
        }
        auto* field = new VarDecl(loc(f), fti, fname);
        field->toplevel(false);
        fields.push_back(field);
      }
      Type tt = Type::record();
      applyVarParOpt(n, tt);
      return new TypeInst(loc(n), tt, ArrayLit::constructTuple(loc(n), fields));
    }
    case K::TypeConcatenation: {
      TypeInst* lhs = typeInst(child(n, F::Left));
      TypeInst* rhs = typeInst(child(n, F::Right));
      if (lhs == nullptr || rhs == nullptr) {
        return nullptr;
      }
      // The result keeps the left operand's type-inst; the concatenation itself
      // becomes its domain (as in the bison grammar).
      Type tt = Expression::type(lhs);
      tt.dim(0);
      auto* inner = new TypeInst(loc(n), tt, lhs->domain());
      auto* bop = new BinOp(loc(n), inner, BOT_PLUSPLUS, rhs);
      bop->type(tt);
      lhs->domain(bop);
      return lhs;
    }
    case K::AnyType:
      return new TypeInst(loc(n), Type::mkAny());
    case K::OperationType:
      return static_cast<TypeInst*>(futureFeature(n, "function types"));
    default:
      error(n, "internal: unhandled type kind '" + std::string(ts_node_type(n)) + "'");
      return nullptr;
  }
}

void Lowerer::applyVarParOpt(TSNode n, Type& t) const {
  if (has(n, F::VarPar)) {
    if (text(child(n, F::VarPar)) == "var") {
      t.ti(Type::TI_VAR);
    }
    t.tiExplicit(true);
  }
  if (has(n, F::Opt)) {
    t.ot(Type::OT_OPTIONAL);
    t.otExplicit(true);
  }
}

TypeInst* Lowerer::typeBase(TSNode n) {
  TSNode domain = child(n, F::Domain);
  if (has(n, F::Any)) {
    // `any $X`; the lexer strips a single leading `$`
    return new TypeInst(loc(n), Type::mkAny(), new TIId(loc(domain), text(domain).substr(1)));
  }
  TypeInst* ti;
  switch (_s.kind(domain)) {
    case K::PrimitiveType: {
      std::string t = text(domain);
      Type ty;
      if (t == "int") {
        ty = Type::parint();
      } else if (t == "bool") {
        ty = Type::parbool();
      } else if (t == "float") {
        ty = Type::parfloat();
      } else if (t == "string") {
        ty = Type::parstring();
      } else {
        ty = Type::ann();
      }
      ti = new TypeInst(loc(n), ty);
      break;
    }
    case K::TypeInstId:
      // `$X`; the lexer strips a single leading `$`
      ti = new TypeInst(loc(n), Type::top(), new TIId(loc(domain), text(domain).substr(1)));
      break;
    case K::TypeInstEnumId:
      // `$$E`; likewise, so the name keeps one `$`
      ti = new TypeInst(loc(n), Type::parint(), new TIId(loc(domain), text(domain).substr(1)));
      break;
    case K::NewType:
      return static_cast<TypeInst*>(futureFeature(domain, "object types"));
    default: {
      Expression* d = expr(domain);
      if (d == nullptr) {
        return nullptr;
      }
      ti = new TypeInst(loc(n), Type(), d);
      break;
    }
  }
  Type tt = Expression::type(ti);
  applyVarParOpt(n, tt);
  ti->type(tt);
  return ti;
}

TypeInst* Lowerer::arrayTypeInst(TSNode n) {
  std::vector<TypeInst*> ranges;
  for (TSNode d : children(n, F::Dimension)) {
    TSNode nameNode = child(d, F::Name);
    if (!ts_node_is_null(nameNode)) {
      // `array[i in S] of T`: the binder is smuggled through as a marker tuple
      ASTString name = identifier(nameNode);
      Expression* set = typeAsExpr(child(d, F::Type));
      if (name.empty() || set == nullptr) {
        return nullptr;
      }
      auto* binder = new Id(loc(nameNode), name, nullptr);
      auto* rangeTi = new TypeInst(loc(child(d, F::Type)), Type(), set);
      ArrayLit* marker = ArrayLit::constructTuple(loc(d), {rangeTi, binder});
      ranges.push_back(new TypeInst(loc(d), Type(), marker));
      continue;
    }
    TypeInst* ti = typeInst(child(d, F::Type));
    if (ti == nullptr) {
      return nullptr;
    }
    ranges.push_back(ti);
  }
  TypeInst* inner = typeInst(child(n, F::Type));
  if (inner == nullptr) {
    return nullptr;
  }
  TypeInst* ti = inner->isarray() ? new TypeInst(loc(n), Type::tuple(), inner) : inner;
  ti->setRanges(ranges);
  return ti;
}

VarDecl* Lowerer::parameter(TSNode n) {
  TypeInst* ti = typeInst(child(n, F::Type));
  if (ti == nullptr) {
    return nullptr;
  }
  if (Expression::type(ti).any() && ti->domain() == nullptr) {
    error(n, "parameter declaration cannot have `any' type-inst without type-inst variable");
  }
  TSNode nameNode = child(n, F::Name);
  if (ts_node_is_null(nameNode)) {
    // An unnamed parameter, as in `predicate p(int)`
    auto* anon = new VarDecl(loc(n), ti, ASTString());
    anon->toplevel(false);
    return anon;
  }
  ASTString name;
  if (!patternName(nameNode, name)) {
    return nullptr;
  }
  auto* vd = new VarDecl(loc(n), ti, new Id(loc(nameNode), name, nullptr));
  vd->toplevel(false);
  std::vector<Expression*> anns = annotations(n);
  if (!anns.empty()) {
    add_annotations(vd, anns);
  }
  TSNode def = child(n, F::Default);
  if (!ts_node_is_null(def)) {
    Expression* e = expr(def);
    if (e == nullptr) {
      return nullptr;
    }
    vd->e(e);
  }
  return vd;
}

using TreePtr = std::unique_ptr<TSTree, void (*)(TSTree*)>;

TreePtr parse_with(const TSLanguage* lang, const ParserState& pp) {
  return {ts_parser_parse_string(ts_parser(lang), nullptr, pp.buf, pp.length), ts_tree_delete};
}

/// Handles a data file the DataZinc grammar could not parse. If it is valid
/// MiniZinc then a model item is still an error, but an expression only earns a
/// warning and is parsed as MiniZinc, so that data files which compute their
/// data keep working. True if this took care of the file.
bool fall_back_to_model_grammar(ParserState& pp, TSNode dataRoot) {
  TreePtr tree = parse_with(tree_sitter_minizinc(), pp);
  if (tree == nullptr) {
    return false;
  }
  TSNode root = ts_tree_root_node(tree.get());
  Lowerer asModel(pp, root, syms(false), tree_sitter_minizinc());
  if (ts_node_has_error(root)) {
    // Not valid MiniZinc either, so this really is a syntax error and the
    // DataZinc grammar's report is the more precise one.
    return false;
  }
  if (asModel.reportModelItemsInDataFile()) {
    return true;
  }
  Lowerer asData(pp, dataRoot, syms(true), tree_sitter_datazinc());
  asData.warnNotDataZinc();
  asModel.run();
  return true;
}

}  // namespace

void parse_tree_sitter(ParserState& pp) {
  if (const void* nul = memchr(pp.buf, 0, pp.length)) {
    auto offset = static_cast<unsigned int>(static_cast<const char*>(nul) - pp.buf);
    ParserLocation l = nul_location(pp, offset);
    pp.hadError = true;
    pp.syntaxErrors.emplace_back(Location(l),
                                 pp.getCurrentLine(offset, static_cast<int>(l.firstColumn()),
                                                   static_cast<int>(l.lastColumn())),
                                 std::vector<ASTString>(), "syntax error, null character");
    return;
  }
  const TSLanguage* lang = pp.isDatafile ? tree_sitter_datazinc() : tree_sitter_minizinc();
  TreePtr tree = parse_with(lang, pp);
  if (tree == nullptr) {
    throw InternalError("tree-sitter failed to parse '" + std::string(pp.filename) + "'");
  }
  TSNode root = ts_tree_root_node(tree.get());
  if (pp.isDatafile && ts_node_has_error(root) && fall_back_to_model_grammar(pp, root)) {
    return;
  }
  Lowerer lowerer(pp, root, syms(pp.isDatafile), lang);
  // A broken tree is not lowered: the recovered subtrees would otherwise
  // produce a cascade of misleading follow-on errors.
  if (lowerer.reportSyntaxErrors()) {
    return;
  }
  lowerer.run();
}

}  // namespace MiniZinc

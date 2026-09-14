/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Lower tree-feller reductions into the MiniZinc AST. On rejection, tree-sitter
 * runs once to recover and report syntax errors. */

#include <minizinc/model.hh>
#include <minizinc/parser.hh>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// clang-format off
// api.h first: the grammar ABI header in tree_feller.h skips what it declares.
#include <tree_sitter/api.h>
#include <tree_feller.h>
// clang-format on

#include "parser_ts.hh"

extern "C" const TSLanguage* tree_sitter_minizinc(void);

namespace MiniZinc {

namespace {

/// Dense node kinds, so that dispatch is a jump table rather than a chain of
/// TSSymbol comparisons.
enum class K : unsigned char {
  Unknown = 0,
  // items
  SourceFile,
  Item,  ///< the hidden `_item` choice, which is only reduced at the top level
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
  ConcatenatedDomain,  ///< `_concatenated_domain`, a hidden rule aliased to `type_base`
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

const OpEntry* infix_op(const std::string& text) {
  for (const auto& e : INFIX_OPS) {
    if (text == e.text) {
      return &e;
    }
  }
  return nullptr;
}

/// Symbol and field lookup tables, built once from the language.
class Syms {
public:
  explicit Syms(const TSLanguage* lang) : _tokenCount(lang->token_count) {
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
      if (_fields[i] == 0) {
        // Silent otherwise: `child()` would return null and the lowering would
        // read the child as absent
        throw InternalError(std::string("tree-sitter grammar has no field '") + FIELD_NAMES[i] +
                            "'");
      }
    }
    checkAllSymbolsCovered(lang);

    // The reduction stream reports the grammar's own symbols: hidden rules and
    // repetitions are reduced too, and an aliased rule keeps its own symbol.
    _raw.assign(lang->symbol_count, K::Unknown);
    _splice.assign(lang->symbol_count, false);
    for (uint32_t i = 0; i < lang->symbol_count; i++) {
      auto s = static_cast<TSSymbol>(i);
      K k = _kinds[i];
      if (s >= lang->token_count) {
        if (lang->public_symbol_map[s] != s) {
          // A hidden rule only ever used under an alias. Only `type_base` has a
          // different shape from the rule it is named after.
          if (strcmp(lang->symbol_names[s], "type_base") == 0) {
            k = K::ConcatenatedDomain;
          }
        } else if (!lang->symbol_metadata[s].visible) {
          if (strcmp(lang->symbol_names[s], "_item") == 0) {
            k = K::Item;
          } else {
            _splice[s] = true;
          }
        }
      }
      _raw[i] = k;
    }
    // An anonymous token is named by its text, so operators resolve per symbol
    _infixOps.assign(lang->token_count, nullptr);
    for (uint32_t i = 0; i < lang->token_count; i++) {
      auto s = static_cast<TSSymbol>(i);
      if (!lang->symbol_metadata[s].named) {
        _infixOps[i] = infix_op(lang->symbol_names[s]);
      }
    }

    // Fields by production and structural child position. Inherited entries
    // describe a hidden child's fields, which that child reports itself.
    _stride = lang->max_alias_sequence_length;
    if (lang->field_count != 0) {
      for (uint32_t p = 0; p < lang->production_id_count; p++) {
        const TSMapSlice& slice = lang->field_map_slices[p];
        for (uint32_t j = slice.index; j < static_cast<uint32_t>(slice.index + slice.length); j++) {
          _stride = std::max<unsigned int>(_stride, lang->field_map_entries[j].child_index + 1U);
        }
      }
    }
    _fieldAt.assign(static_cast<size_t>(lang->production_id_count) * _stride, 0);
    if (lang->field_count != 0) {
      for (uint32_t p = 0; p < lang->production_id_count; p++) {
        const TSMapSlice& slice = lang->field_map_slices[p];
        for (uint32_t j = slice.index; j < static_cast<uint32_t>(slice.index + slice.length); j++) {
          const TSFieldMapEntry& e = lang->field_map_entries[j];
          if (!e.inherited) {
            _fieldAt[p * _stride + e.child_index] = e.field_id;
          }
        }
      }
    }
  }

  /// The kind of a tree-sitter node, which carries its public symbol.
  K kind(TSNode n) const {
    TSSymbol s = ts_node_symbol(n);
    return s < _kinds.size() ? _kinds[s] : K::Unknown;
  }
  /// The kind of a symbol in the reduction stream.
  K rawKind(TSSymbol s) const { return s < _raw.size() ? _raw[s] : K::Unknown; }
  /// Hidden rules and repetitions, whose children stand in for them.
  bool splice(TSSymbol s) const { return s < _splice.size() && _splice[s]; }
  bool isToken(TSSymbol s) const { return s < _tokenCount; }
  /// The infix operator an operator token spells, or null.
  const OpEntry* infixOp(TSSymbol s) const { return s < _infixOps.size() ? _infixOps[s] : nullptr; }
  TSFieldId field(F f) const { return _fields[static_cast<unsigned int>(f)]; }
  TSFieldId fieldAt(uint16_t production, uint32_t index) const {
    return index < _stride ? _fieldAt[production * _stride + index] : 0;
  }

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
  std::vector<K> _raw;
  std::vector<bool> _splice;
  std::vector<TSFieldId> _fieldAt;
  std::vector<const OpEntry*> _infixOps;
  unsigned int _stride = 0;
  uint32_t _tokenCount;
  TSFieldId _fields[static_cast<unsigned int>(F::END)];
};

const Syms& syms() {
  static const Syms s(tree_sitter_minizinc());
  return s;
}

/// Load the shared, read-only tree-feller tables once.
const TFLanguage* tf_tables() {
  struct Holder {
    TFLanguage* lang;
    explicit Holder(const TSLanguage* ts) {
      const char* message = nullptr;
      lang = tf_language_load(ts, &message);
      if (lang == nullptr) {
        throw InternalError(message != nullptr ? message : "cannot load the parser tables");
      }
    }
    ~Holder() { tf_language_free(lang); }
  };
  static const Holder holder(tree_sitter_minizinc());
  return holder.lang;
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

/// Name a model-only item, or return null for a valid data item.
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

/// Source positions, converted to the code point columns the AST uses.
class SourceText {
public:
  explicit SourceText(const ParserState& pp)
      : buf(pp.buf), len(pp.length), _filename(ASTString(pp.filename)), _lineOffset(pp.lineOffset) {
    // Byte columns equal code point columns unless the file has multi-byte
    // characters, which is the overwhelmingly common case.
    _ascii = true;
    for (unsigned int i = 0; i < len; i++) {
      if ((static_cast<unsigned char>(buf[i]) & 0x80) != 0) {
        _ascii = false;
        break;
      }
    }
  }

  /// Points count bytes, so `startByte - column` is where the line starts.
  ParserLocation loc(uint32_t startByte, uint32_t startRow, uint32_t startColumn, uint32_t endByte,
                     uint32_t endRow, uint32_t endColumn) const {
    unsigned int firstColumn = codePointColumn(startByte - startColumn, startByte) + 1;
    unsigned int lastColumn = codePointColumn(endByte - endColumn, endByte);
    if (endRow == startRow && lastColumn < firstColumn) {
      lastColumn = firstColumn;  // zero-width node (a MISSING token)
    }
    return {_filename, startRow + 1 + _lineOffset, firstColumn, endRow + 1 + _lineOffset,
            lastColumn};
  }

  /// The byte offset where a location starts.
  unsigned int byteOf(const Location& l) const {
    unsigned int line = 1 + _lineOffset;
    unsigned int i = 0;
    for (; i < len && line < l.firstLine(); i++) {
      if (buf[i] == '\n') {
        line++;
      }
    }
    for (unsigned int col = 1; i < len && col < l.firstColumn(); col++) {
      do {
        i++;
      } while (i < len && (static_cast<unsigned char>(buf[i]) & 0xc0) == 0x80);
    }
    return i;
  }

  const char* buf;
  unsigned int len;

private:
  /// Number of UTF-8 code points in [lineStart, byte). Columns are asked for in
  /// source order, so continue from the previous answer rather than rescanning
  /// the line; FlatZinc lines can be megabytes.
  unsigned int codePointColumn(unsigned int lineStart, unsigned int byte) const {
    if (_ascii) {
      return byte - lineStart;
    }
    unsigned int from = lineStart;
    unsigned int n = 0;
    if (_colLineStart == lineStart && _colByte <= byte) {
      from = _colByte;
      n = _colCount;
    }
    for (unsigned int i = from; i < byte && i < len; i++) {
      if ((static_cast<unsigned char>(buf[i]) & 0xc0) != 0x80) {
        n++;
      }
    }
    _colLineStart = lineStart;
    _colByte = byte;
    _colCount = n;
    return n;
  }

  ASTString _filename;
  /// Non-zero for a file read out of a library bundle
  unsigned int _lineOffset;
  bool _ascii;
  mutable unsigned int _colLineStart = 0;
  mutable unsigned int _colByte = 0;
  mutable unsigned int _colCount = 0;
};

void add_syntax_error(ParserState& pp, const ParserLocation& l, unsigned int startByte,
                      const std::string& msg) {
  pp.hadError = true;
  std::vector<ASTString> includeStack;
  for (Model* m = pp.model; m != nullptr; m = m->parent()) {
    if (m->parent() != nullptr) {
      includeStack.push_back(m->filename());
    }
  }
  pp.syntaxErrors.emplace_back(Location(l),
                               pp.getCurrentLine(startByte, static_cast<int>(l.firstColumn()),
                                                 static_cast<int>(l.lastColumn())),
                               includeStack, msg);
}

// ---------------------------------------------------------------------------
// Syntax errors, from tree-sitter's recovered tree

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

/// Reports the ERROR and MISSING nodes of a tree-sitter tree.
class Diagnostics {
public:
  Diagnostics(ParserState& pp, TSNode root, const Syms& s)
      : _pp(pp), _src(pp), _s(s), _root(root) {}

  /// Reports every ERROR/MISSING node in the tree; true if there was any.
  bool reportSyntaxErrors() {
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

private:
  std::string text(TSNode n) const {
    return {_src.buf + ts_node_start_byte(n), _src.buf + ts_node_end_byte(n)};
  }
  ParserLocation loc(TSNode n) const {
    TSPoint s = ts_node_start_point(n);
    TSPoint e = ts_node_end_point(n);
    return _src.loc(ts_node_start_byte(n), s.row, s.column, ts_node_end_byte(n), e.row, e.column);
  }
  void error(TSNode n, const std::string& msg) {
    add_syntax_error(_pp, loc(n), ts_node_start_byte(n), msg);
  }

  /// The field \a n fills in its parent, or `F::END` if it fills none
  F fieldOf(TSNode n) const {
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

  void collectSyntaxErrors(TSNode n) {
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
        msg =
            "syntax error, unexpected `" + (what.empty() ? std::string("end of file") : what) + "'";
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

  ParserState& _pp;
  SourceText _src;
  const Syms& _s;
  TSNode _root;
};

// ---------------------------------------------------------------------------
// Lowering, from tree-feller's reduction stream

/// A child as its parent rule sees it. Hidden rules and repetitions are spliced
/// into their parent, so these are the children a tree-sitter node would have,
/// holding what the lowering returned for them.
struct Child {
  void* value;
  uint32_t startByte;
  uint32_t endByte;
  TFPoint startPoint;
  TFPoint endPoint;
  TSSymbol symbol;
  TSFieldId field;
};

/// A rule's children and span: a reduction in progress, or a deferred subtree.
struct Node {
  const Child* kids;
  size_t count;
  uint32_t startByte;
  uint32_t endByte;
  TFPoint startPoint;
  TFPoint endPoint;
  K kind;

  const Child* begin() const { return kids; }
  const Child* end() const { return kids + count; }
};

/// A rule whose meaning depends on where it is used, kept until its parent
/// reduces -- as bison passes a `Generator*` up for `call_expr` to reinterpret.
struct Deferred {
  size_t first;
  size_t count;
  uint32_t startByte;
  uint32_t endByte;
  TFPoint startPoint;
  TFPoint endPoint;
  K kind;
};

bool is_deferred(K k) {
  switch (k) {
    case K::AnnotationParameters:
    case K::ArrayDimension:
    case K::ArrayLiteral2dRow:
    case K::ArrayLiteral3dRow:
    case K::ArrayLiteral3dSlice:
    case K::ArrayLiteralMember:
    case K::ArrayType:
    case K::AssignmentGenerator:
    case K::ConcatenatedDomain:
    case K::Generator:
    case K::InversedIdentifier:
    case K::ListType:
    case K::OperationType:
    case K::Parameter:
    case K::RecordMember:
    case K::RecordType:
    case K::RecordTypeField:
    case K::SetType:
    case K::TupleType:
    case K::TypeBase:
    case K::TypeConcatenation:
      return true;
    default:
      return false;
  }
}

void* count_value(size_t n) { return reinterpret_cast<void*>(n); }
size_t value_count(void* v) { return reinterpret_cast<size_t>(v); }

/// Builds AST items for one file from bottom-up reductions.
class Lowerer {
public:
  Lowerer(ParserState& pp, const Syms& s, Model* items)
      : _pp(pp), _s(s), _src(pp), _buf(pp.buf), _items(items) {}

  void* shift(const TFToken* t, bool extra);
  void* reduce(const TFReduction* r);

  /// A model item was rejected because this is a data file.
  bool rejectedModelItem() const { return _rejectedModelItem; }

private:
  K kind(const Child& c) const { return _s.rawKind(c.symbol); }
  const Child* child(const Node& n, F f) const {
    TSFieldId id = _s.field(f);
    if (id != 0) {
      for (const Child& c : n) {
        if (c.field == id) {
          return &c;
        }
      }
    }
    return nullptr;
  }
  bool has(const Node& n, F f) const { return child(n, f) != nullptr; }
  /// Every child of `n` under field `f`, in source order.
  std::vector<const Child*> children(const Node& n, F f) const {
    std::vector<const Child*> out;
    TSFieldId id = _s.field(f);
    if (id != 0) {
      for (const Child& c : n) {
        if (c.field == id) {
          out.push_back(&c);
        }
      }
    }
    return out;
  }
  /// The children of a deferred child.
  Node node(const Child& c) const {
    const auto* d = static_cast<const Deferred*>(c.value);
    return {_deferredKids.data() + d->first,
            d->count,
            d->startByte,
            d->endByte,
            d->startPoint,
            d->endPoint,
            d->kind};
  }
  std::string text(const Child& c) const { return {_buf + c.startByte, _buf + c.endByte}; }
  std::string text(const Node& n) const { return {_buf + n.startByte, _buf + n.endByte}; }

  ParserLocation loc(const Child& c) const {
    return _src.loc(c.startByte, c.startPoint.row, c.startPoint.column, c.endByte, c.endPoint.row,
                    c.endPoint.column);
  }
  ParserLocation loc(const Node& n) const {
    return _src.loc(n.startByte, n.startPoint.row, n.startPoint.column, n.endByte, n.endPoint.row,
                    n.endPoint.column);
  }
  void error(const Child& c, const std::string& msg) {
    add_syntax_error(_pp, loc(c), c.startByte, msg);
  }
  void error(const Node& n, const std::string& msg) {
    add_syntax_error(_pp, loc(n), n.startByte, msg);
  }
  void error(Expression* e, const std::string& msg) {
    const Location& l = Expression::loc(e);
    add_syntax_error(_pp, l.parserLocation(), _src.byteOf(l), msg);
  }
  /// Grammar-supported syntax that this compiler does not implement.
  std::nullptr_t futureFeature(const Child& c, const char* what) {
    error(c, std::string(what) + " are not supported by this version of MiniZinc");
    return nullptr;
  }
  std::nullptr_t futureFeature(const Node& n, const char* what) {
    error(n, std::string(what) + " are not supported by this version of MiniZinc");
    return nullptr;
  }

  void* lower(const Node& n);
  /// Records a call made by a data file for the type checker to judge, unless it
  /// is one of the reshaping builtins. Whether the rest are enum constructors or
  /// model calls is not knowable here: a data file may name a constructor before
  /// the assignment that introduces it, or in an entirely different file.
  void noteDataCall(Call* c);

  void* topLevelItem(const Node& n);
  void addItem(Item* it);
  void addDocComment(Item* it, const std::string& doc, const Child& c);
  void commitFileDocComments(uint32_t before);
  Item* includeItem(const Node& n);
  Item* constraintItem(const Node& n);
  Item* goalItem(const Node& n);
  Item* assignItem(const Node& n);
  Item* declarationItem(const Node& n);
  Item* outputItem(const Node& n);
  Item* operationItem(const Node& n);
  Item* annotationItem(const Node& n);
  Item* enumerationItem(const Node& n);
  Item* typeAliasItem(const Node& n);

  /// The expression a child stands for: its lowered value, or for a token or a
  /// deferred rule, what it means as an expression.
  Expression* expr(const Child* c);
  Expression* infixExpr(const Node& n);
  Expression* prefixExpr(const Node& n);
  Expression* postfixExpr(const Node& n);
  Expression* callExpr(const Node& n);
  Expression* generatorCallExpr(const Node& n);
  Expression* argOrParam(const Node& n);
  Expression* arrayLiteral(const Node& n);
  Expression* arrayLiteral2d(const Node& n);
  Expression* arrayLiteral3d(const Node& n);
  Expression* comprehension(const Node& n, bool isSet);
  Expression* iteExpr(const Node& n);
  Expression* letExpr(const Node& n);
  Expression* indexedAccess(const Node& n);
  Expression* stringLiteral(const Node& n);
  Expression* stringInterpolation(const Node& n);
  Expression* setLiteral(const Node& n);
  Expression* tupleLiteral(const Node& n);
  Expression* recordLiteral(const Node& n);
  Expression* enumerationMembers(const Node& n);
  Expression* anonymousEnumeration(const Node& n);
  Expression* enumerationConstructor(const Node& n);
  Expression* intLiteral(const Child& c, bool negated);
  Expression* floatLiteral(const Child& c, bool negated);

  TypeInst* typeInst(const Child* c);
  TypeInst* typeBase(const Node& n);
  /// Applies the `var`/`par` and `opt` prefixes of `n` to `t`.
  void applyVarParOpt(const Node& n, Type& t) const;
  TypeInst* arrayTypeInst(const Node& n);
  VarDecl* parameter(const Node& n);
  std::vector<VarDecl*> parameters(const Node& n, bool& ok);
  /// `ann: name` on an operation item becomes a trailing parameter.
  VarDecl* annParameter(const Node& n);

  std::vector<Expression*> annotations(const Node& n);
  std::vector<Expression*> callArguments(const Node& n, bool& ok);
  /// Re-reads a type as an expression (call arguments and array index sets
  /// share the type rule with declarations).
  Expression* typeAsExpr(const Child* c);
  ASTString identifier(const Child* c);
  /// The name an inversed identifier or plain identifier declares or calls.
  ASTString calleeName(const Child* c);
  /// Accepts only identifier patterns; destructuring is not supported.
  bool patternName(const Child* c, ASTString& out);
  /// `idLocations` places each variable at its own identifier rather than at
  /// the collection; generator calls do this, plain comprehensions do not.
  bool generators(const Node& n, Generators& gens, bool idLocations);
  bool appendStringPiece(const Child& c, std::string& out);

  ParserState& _pp;
  const Syms& _s;
  SourceText _src;
  const char* _buf;
  /// Where top-level items go.
  Model* _items;
  bool _rejectedModelItem = false;

  /// Spliced children awaiting their parent, in source order.
  std::vector<Child> _stack;
  std::vector<Child> _scratch;
  std::deque<Deferred> _deferred;
  std::vector<Child> _deferredKids;
  /// Array literals with a member that is not a string literal, and that member:
  /// an enum given by an array must name its members with strings.
  std::vector<std::pair<Expression*, Child>> _nonStringArrays;

  /// Byte spans of the doc comments seen since the last top-level item.
  std::vector<std::pair<uint32_t, uint32_t>> _docComments;
  std::vector<std::pair<uint32_t, uint32_t>> _fileDocComments;
  uint32_t _lastItemEnd = 0;
};

void* Lowerer::shift(const TFToken* t, bool extra) {
  if (extra) {
    K k = _s.rawKind(t->symbol);
    if (k == K::DocComment) {
      _docComments.emplace_back(t->start_byte, t->end_byte);
    } else if (k == K::FileDocComment) {
      _fileDocComments.emplace_back(t->start_byte, t->end_byte);
    }
  }
  return nullptr;
}

void* Lowerer::reduce(const TFReduction* r) {
  // A chain of hidden rules, as `_expression` over `_literal`, passes its one
  // child's entries up unchanged
  if (r->node_count == 1 && _s.splice(r->symbol) && _s.splice(r->children[0].symbol) &&
      _s.fieldAt(r->production_id, 0) == 0) {
    return r->children[0].value;
  }
  // A spliced child's entries are already on top of the stack, in order. A kept
  // child that comes before one of them has to be put in its place.
  size_t spliced = 0;
  bool reorder = false;
  bool kept = false;
  uint32_t index = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.extra) {
      continue;
    }
    TSFieldId f = _s.fieldAt(r->production_id, index++);
    if (_s.splice(c.symbol)) {
      spliced += value_count(c.value);
      reorder = reorder || kept;
    } else if (f != 0 || !_s.isToken(c.symbol) || _s.rawKind(c.symbol) != K::Unknown) {
      kept = true;
    }
  }
  size_t base = _stack.size() - spliced;
  size_t cursor = base;
  if (reorder) {
    _scratch.assign(_stack.begin() + static_cast<long>(base), _stack.end());
    _stack.resize(base);
    cursor = 0;
  }
  index = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.extra) {
      continue;
    }
    TSFieldId f = _s.fieldAt(r->production_id, index++);
    if (_s.splice(c.symbol)) {
      // A spliced child fills the field of the position it stands in, unless
      // its own rule gave it one.
      size_t n = value_count(c.value);
      if (reorder) {
        for (size_t k = 0; k < n; k++) {
          Child e = _scratch[cursor++];
          if (e.field == 0) {
            e.field = f;
          }
          _stack.push_back(e);
        }
      } else {
        if (f != 0) {
          for (size_t k = cursor; k < cursor + n; k++) {
            if (_stack[k].field == 0) {
              _stack[k].field = f;
            }
          }
        }
        cursor += n;
      }
    } else if (f != 0 || !_s.isToken(c.symbol) || _s.rawKind(c.symbol) != K::Unknown) {
      // Unnamed punctuation that fills no field is dropped
      _stack.push_back(
          {c.value, c.start_byte, c.end_byte, c.start_point, c.end_point, c.symbol, f});
    }
  }

  if (_s.splice(r->symbol)) {
    return count_value(_stack.size() - base);
  }
  Node n{_stack.data() + base, _stack.size() - base, r->start_byte,        r->end_byte,
         r->start_point,       r->end_point,         _s.rawKind(r->symbol)};
  if (is_deferred(n.kind)) {
    size_t first = _deferredKids.size();
    _deferredKids.insert(_deferredKids.end(), _stack.begin() + static_cast<long>(base),
                         _stack.end());
    _stack.resize(base);
    _deferred.push_back({first, n.count, n.startByte, n.endByte, n.startPoint, n.endPoint, n.kind});
    return &_deferred.back();
  }
  void* value = lower(n);
  _stack.resize(base);
  return value;
}

void* Lowerer::lower(const Node& n) {
  switch (n.kind) {
    case K::SourceFile:
      commitFileDocComments(UINT32_MAX);
      return nullptr;
    case K::Item:
      return topLevelItem(n);
    case K::Include:
      return includeItem(n);
    case K::Constraint:
      return constraintItem(n);
    case K::Goal:
      return goalItem(n);
    case K::Assignment:
      return assignItem(n);
    case K::Declaration:
      return declarationItem(n);
    case K::Output:
      return outputItem(n);
    case K::Predicate:
    case K::FunctionItem:
      return operationItem(n);
    case K::Annotation:
      return annotationItem(n);
    case K::Enumeration:
      return enumerationItem(n);
    case K::TypeAlias:
      return typeAliasItem(n);
    case K::ClassDecl:
      return futureFeature(n, "class declarations");
    case K::StringLiteral:
      return stringLiteral(n);
    case K::StringInterpolation:
      return stringInterpolation(n);
    case K::SetLiteral:
      return setLiteral(n);
    case K::ArrayLiteral: {
      Expression* e = arrayLiteral(n);
      TSFieldId member = _s.field(F::Member);
      for (const Child& m : n) {
        if (m.field == member && kind(m) != K::StringLiteral) {
          if (e != nullptr) {
            _nonStringArrays.emplace_back(e, m);
          }
          break;
        }
      }
      return e;
    }
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
      add_annotations(e, annotations(n));
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
    case K::ArgOrParam:
      return argOrParam(n);
    case K::IndexedAccess:
      return indexedAccess(n);
    case K::TupleAccess: {
      Expression* v = expr(child(n, F::Tuple));
      const Child* f = child(n, F::Field);
      if (v == nullptr || f == nullptr) {
        return nullptr;
      }
      IntVal idx;
      if (!decimal_to_intval(_buf + f->startByte, _buf + f->endByte, idx)) {
        error(*f, "invalid tuple field index");
        return nullptr;
      }
      return new FieldAccess(loc(n), v, IntLit::a(idx));
    }
    case K::RecordAccess: {
      Expression* v = expr(child(n, F::Record));
      const Child* f = child(n, F::Field);
      ASTString field = identifier(f);
      if (v == nullptr || field.empty()) {
        return nullptr;
      }
      return new FieldAccess(loc(n), v, new Id(loc(*f), field, nullptr));
    }
    case K::ArrayComprehension:
      return comprehension(n, false);
    case K::SetComprehension:
      return comprehension(n, true);
    case K::IfThenElse:
      return iteExpr(n);
    case K::LetExpression:
      return letExpr(n);
    case K::EnumerationMembers:
      return enumerationMembers(n);
    case K::AnonymousEnumeration:
      return anonymousEnumeration(n);
    case K::EnumerationConstructor:
      return enumerationConstructor(n);
    case K::Lambda:
      return futureFeature(n, "lambda expressions");
    case K::CaseExpression:
      return futureFeature(n, "case expressions");
    default:
      // Tokens-in-all-but-name (`boolean_literal`, `primitive_type`, patterns,
      // ...), which their parent reads from the source
      return nullptr;
  }
}

void Lowerer::noteDataCall(Call* c) {
  for (const char* f : DATA_FUNCTIONS) {
    if (c->id() == f) {
      return;
    }
  }
  _pp.dataFileCalls.push_back(c);
}

void* Lowerer::topLevelItem(const Node& n) {
  if (n.count != 1) {
    return nullptr;
  }
  const Child& c = n.kids[0];
  auto* it = static_cast<Item*>(c.value);
  if (it != nullptr && _pp.isDatafile) {
    if (const char* what = model_item_name(kind(c))) {
      error(c, std::string(what) + " item not allowed in data file");
      _rejectedModelItem = true;
      it = nullptr;
    }
  }
  // The last doc comment between the previous item and this one documents it
  const std::pair<uint32_t, uint32_t>* doc = nullptr;
  for (const auto& d : _docComments) {
    if (d.first >= _lastItemEnd && d.second <= c.startByte) {
      doc = &d;
    }
  }
  if (it != nullptr && doc != nullptr) {
    std::string t(_buf + doc->first, _buf + doc->second);
    addDocComment(it, t.size() > 5 ? t.substr(3, t.size() - 5) : std::string(), c);
  }
  commitFileDocComments(c.startByte);
  // Comments inside this item document nothing; ones after it may document the
  // next, as extras can be shifted before the reduction that precedes them.
  auto inside = [&](const std::pair<uint32_t, uint32_t>& d) { return d.first < c.endByte; };
  _docComments.erase(std::remove_if(_docComments.begin(), _docComments.end(), inside),
                     _docComments.end());
  _fileDocComments.erase(std::remove_if(_fileDocComments.begin(), _fileDocComments.end(), inside),
                         _fileDocComments.end());
  _lastItemEnd = c.endByte;
  addItem(it);
  return nullptr;
}

void Lowerer::addItem(Item* it) {
  // Nothing on the stack below a top-level item holds an expression
  _deferred.clear();
  _deferredKids.clear();
  _nonStringArrays.clear();
  if (it != nullptr) {
    _items->addItem(it);
    GC::unlock();
    GC::lock();
  }
}

void Lowerer::addDocComment(Item* it, const std::string& doc, const Child& c) {
  if (!_pp.parseDocComments) {
    return;
  }
  std::vector<Expression*> args(1);
  args[0] = new StringLit(loc(c), doc);
  Call* call = Call::a(Location(loc(c)), Constants::constants().ann.doc_comment, args);
  Expression::type(call, Type::ann());
  if (auto* fi = Item::dynamicCast<FunctionI>(it)) {
    fi->ann().add(call);
  } else if (auto* vdi = Item::dynamicCast<VarDeclI>(it)) {
    Expression::addAnnotation(vdi->e(), call);
  } else {
    error(c,
          "documentation comments are only supported for function, predicate and variable "
          "declarations");
  }
}

void Lowerer::commitFileDocComments(uint32_t before) {
  for (const auto& d : _fileDocComments) {
    if (d.first >= _lastItemEnd && d.second <= before && _pp.parseDocComments) {
      std::string t(_buf + d.first, _buf + d.second);
      // strip the `/***` and `*/` delimiters
      _items->addDocComment(t.size() > 6 ? t.substr(4, t.size() - 6) : std::string());
    }
  }
}

Item* Lowerer::includeItem(const Node& n) {
  const Child* fileNode = child(n, F::File);
  if (fileNode == nullptr || fileNode->value == nullptr) {
    return nullptr;
  }
  const ASTString& v = Expression::cast<StringLit>(static_cast<Expression*>(fileNode->value))->v();
  std::string file(v.c_str(), v.size());
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

Item* Lowerer::constraintItem(const Node& n) {
  Expression* e = expr(child(n, F::Expression));
  if (e == nullptr) {
    return nullptr;
  }
  // `constraint :: <string> <expr>` names the constraint; it is not a general
  // annotation. Annotations on the constraint itself go on the expression
  // (`constraint <expr> :: <ann>`), which is handled by `annotated_expression`.
  std::vector<const Child*> annNodes = children(n, F::Annotation);
  if (!annNodes.empty()) {
    if (annNodes.size() > 1) {
      error(n, "a constraint takes at most one name");
      return nullptr;
    }
    K k = kind(*annNodes[0]);
    if (k != K::StringLiteral && k != K::StringInterpolation) {
      error(*annNodes[0], "the name of a constraint must be a string");
      return nullptr;
    }
    Expression* name = expr(annNodes[0]);
    if (name == nullptr) {
      return nullptr;
    }
    Expression::ann(e).add(Call::a(loc(*annNodes[0]), ASTString("mzn_constraint_name"), {name}));
  }
  return new ConstraintI(loc(n), e);
}

Item* Lowerer::goalItem(const Node& n) {
  const Child* strategy = child(n, F::Strategy);
  std::string what = strategy == nullptr ? "satisfy" : text(*strategy);
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

Item* Lowerer::assignItem(const Node& n) {
  ASTString name = identifier(child(n, F::Name));
  Expression* e = expr(child(n, F::Definition));
  if (name.empty() || e == nullptr) {
    return nullptr;
  }
  return new AssignI(loc(n), name, e);
}

Item* Lowerer::declarationItem(const Node& n) {
  TypeInst* ti = typeInst(child(n, F::Type));
  const Child* nameNode = child(n, F::Name);
  ASTString name;
  if (ti == nullptr || !patternName(nameNode, name)) {
    return nullptr;
  }
  std::vector<const Child*> annNodes = children(n, F::Annotation);
  std::vector<Expression*> anns = annotations(n);
  // At the top level the declaration stops after its annotations; a `let`
  // widens it to the whole item, as in the bison grammar.
  ParserLocation declLoc = loc(n);
  const Child* def = child(n, F::Definition);
  if (def != nullptr) {
    ParserLocation end = loc(annNodes.empty() ? *nameNode : *annNodes.back());
    declLoc = ParserLocation(ASTString(_pp.filename), declLoc.firstLine(), declLoc.firstColumn(),
                             end.lastLine(), end.lastColumn());
  }
  auto* vd = new VarDecl(declLoc, ti, new Id(loc(*nameNode), name, nullptr));
  add_annotations(vd, anns);
  if (def != nullptr) {
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

Item* Lowerer::outputItem(const Node& n) {
  Expression* e = expr(child(n, F::Expression));
  if (e == nullptr) {
    return nullptr;
  }
  auto* oi = new OutputI(loc(n), e);
  // `output :: <section> <expr>`; the grammar admits only the forms that
  // cannot swallow the expression that follows.
  const Child* section = child(n, F::Annotation);
  if (section != nullptr) {
    Expression* sec = expr(section);
    if (sec == nullptr) {
      return nullptr;
    }
    oi->ann().add(Call::a(loc(n), ASTString("mzn_output_section"), {sec}));
  }
  return oi;
}

std::vector<VarDecl*> Lowerer::parameters(const Node& n, bool& ok) {
  std::vector<VarDecl*> out;
  for (const Child* p : children(n, F::Parameter)) {
    VarDecl* vd = parameter(node(*p));
    if (vd == nullptr) {
      ok = false;
      continue;
    }
    out.push_back(vd);
  }
  return out;
}

VarDecl* Lowerer::annParameter(const Node& n) {
  const Child* ap = child(n, F::AnnotationParameter);
  if (ap == nullptr) {
    return nullptr;
  }
  ASTString name = identifier(ap);
  if (name.empty()) {
    return nullptr;
  }
  auto* ti = new TypeInst(loc(n), Type::ann(1));
  auto* vd = new VarDecl(loc(n), ti, new Id(loc(*ap), name, nullptr));
  vd->toplevel(false);
  return vd;
}

ASTString Lowerer::calleeName(const Child* c) {
  // `p⁻¹` names the inverse of `p`, with the suffix
  if (c != nullptr && kind(*c) == K::InversedIdentifier) {
    ASTString id = identifier(child(node(*c), F::Identifier));
    if (id.empty()) {
      return id;
    }
    return ASTString(std::string(id.c_str(), id.size()) + "⁻¹");
  }
  return identifier(c);
}

Item* Lowerer::operationItem(const Node& n) {
  bool isPredicate = n.kind == K::Predicate;
  ASTString name = calleeName(child(n, F::Name));
  if (name.empty()) {
    return nullptr;
  }
  TypeInst* ti;
  if (isPredicate) {
    const Child* type = child(n, F::Type);
    std::string what = type == nullptr ? "" : text(*type);
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
  const Child* bodyNode = child(n, F::Body);
  if (bodyNode != nullptr) {
    body = expr(bodyNode);
    if (body == nullptr) {
      return nullptr;
    }
  }
  auto* fi = new FunctionI(loc(n), name, ti, params, body, _pp.isSTDLib, annParam != nullptr);
  fi->ann().add(annotations(n));
  return fi;
}

Item* Lowerer::annotationItem(const Node& n) {
  ASTString name = identifier(child(n, F::Name));
  if (name.empty()) {
    return nullptr;
  }
  auto* ti = new TypeInst(loc(n), Type::ann());
  bool ok = true;
  std::vector<VarDecl*> params;
  const Child* paramsNode = child(n, F::Parameters);
  if (paramsNode != nullptr) {
    params = parameters(node(*paramsNode), ok);
    if (!ok) {
      return nullptr;
    }
  }
  const Child* bodyNode = child(n, F::Body);
  if (bodyNode == nullptr && params.empty()) {
    return VarDeclI::a(loc(n), new VarDecl(loc(n), ti, name));
  }
  Expression* body = nullptr;
  if (bodyNode != nullptr) {
    body = expr(bodyNode);
    if (body == nullptr) {
      return nullptr;
    }
  }
  return new FunctionI(loc(n), name, ti, params, body, _pp.isSTDLib);
}

Item* Lowerer::typeAliasItem(const Node& n) {
  ASTString name = identifier(child(n, F::Name));
  TypeInst* ti = typeInst(child(n, F::Type));
  if (name.empty() || ti == nullptr) {
    return nullptr;
  }
  auto* vd = new VarDecl(loc(n), nullptr, name, ti);
  add_annotations(vd, annotations(n));
  return VarDeclI::a(loc(n), vd);
}

Expression* Lowerer::enumerationMembers(const Node& n) {
  std::vector<Expression*> ids;
  for (const Child* m : children(n, F::Member)) {
    ASTString id = identifier(m);
    if (id.empty()) {
      return nullptr;
    }
    ids.push_back(new Id(loc(*m), id, nullptr));
  }
  return new SetLit(loc(n), ids);
}

Expression* Lowerer::anonymousEnumeration(const Node& n) {
  std::vector<Expression*> args;
  for (const Child* p : children(n, F::Parameter)) {
    Expression* e = typeAsExpr(p);
    if (e == nullptr) {
      return nullptr;
    }
    args.push_back(e);
  }
  return Call::a(loc(n), Constants::constants().ids.anon_enum_set, args);
}

Expression* Lowerer::enumerationConstructor(const Node& n) {
  ASTString cname = identifier(child(n, F::Name));
  if (cname.empty()) {
    return nullptr;
  }
  std::vector<Expression*> args;
  for (const Child* p : children(n, F::Parameter)) {
    Expression* e = typeAsExpr(child(node(*p), F::Type));
    if (e == nullptr) {
      return nullptr;
    }
    args.push_back(e);
  }
  return Call::a(loc(n), cname, args);
}

Item* Lowerer::enumerationItem(const Node& n) {
  ASTString name = identifier(child(n, F::Name));
  if (name.empty()) {
    return nullptr;
  }
  auto* ti = new TypeInst(loc(n), Type::parsetint());
  ti->setIsEnum(true);
  std::vector<Expression*> cases;
  bool stringMembers = false;
  for (const Child* c : children(n, F::Case)) {
    auto* e = static_cast<Expression*>(c->value);
    if (e == nullptr) {
      return nullptr;
    }
    if (kind(*c) == K::ArrayLiteral) {
      // `enum E = ["a", "b"]` names the members with strings (parser.yxx:567),
      // and is the whole body rather than one case of a `++` chain
      for (const auto& a : _nonStringArrays) {
        if (a.first == e) {
          error(a.second, "syntax error, enum members given as an array must be strings");
          return nullptr;
        }
      }
      e = Call::a(loc(*c), Constants::constants().ids.anonEnumFromStrings, {e});
      stringMembers = true;
    }
    cases.push_back(e);
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
  add_annotations(vd, annotations(n));
  return VarDeclI::a(loc(n), vd);
}

std::vector<Expression*> Lowerer::annotations(const Node& n) {
  std::vector<Expression*> out;
  for (const Child* a : children(n, F::Annotation)) {
    if (Expression* e = expr(a)) {
      out.push_back(e);
    }
  }
  return out;
}

Expression* Lowerer::expr(const Child* c) {
  if (c == nullptr) {
    return nullptr;
  }
  switch (kind(*c)) {
    case K::Identifier:
    case K::QuotedIdentifier: {
      ASTString id = identifier(c);
      return id.empty() ? nullptr : new Id(loc(*c), id, nullptr);
    }
    case K::IntegerLiteral:
      return intLiteral(*c, false);
    case K::FloatLiteral:
      return floatLiteral(*c, false);
    case K::BooleanLiteral:
      return Constants::constants().boollit(_buf[c->startByte] == 't');
    case K::Infinity:
      return IntLit::a(IntVal::infinity());
    case K::Absent:
      return Constants::constants().absent;
    case K::Anonymous:
      return new AnonVar(loc(*c));
    case K::InversedIdentifier: {
      // `f^-1` on its own is `f` to the power -1; as a call target it names the
      // inverse function (handled in callExpr).
      ASTString id = identifier(child(node(*c), F::Identifier));
      if (id.empty()) {
        return nullptr;
      }
      return new BinOp(loc(*c), new Id(loc(*c), id, nullptr), BOT_POW, IntLit::a(-1));
    }
    case K::TypeBase:
    case K::ConcatenatedDomain:
    case K::ArrayType:
    case K::SetType:
    case K::TupleType:
    case K::RecordType:
    case K::ListType:
    case K::AnyType:
    case K::OperationType:
    case K::TypeConcatenation:
      return typeAsExpr(c);
    case K::PatternCall:
    case K::PatternTuple:
    case K::PatternRecord:
    case K::PatternNumericLiteral:
      return futureFeature(*c, "pattern matching");
    default:
      return static_cast<Expression*>(c->value);
  }
}

Expression* Lowerer::infixExpr(const Node& n) {
  const Child* opNode = child(n, F::Operator);
  Expression* lhs = expr(child(n, F::Left));
  Expression* rhs = expr(child(n, F::Right));
  if (lhs == nullptr || rhs == nullptr || opNode == nullptr) {
    return nullptr;
  }
  if (kind(*opNode) == K::BacktickIdentifier) {
    std::string t = text(*opNode);
    std::string name = t.substr(1, t.size() - 2);
    if (name.empty()) {
      error(*opNode, "syntax error, empty operator name");
      return nullptr;
    }
    return Call::a(loc(n), ASTString(name), {lhs, rhs});
  }
  const OpEntry* e = _s.infixOp(opNode->symbol);
  if (e == nullptr) {
    error(*opNode, "internal: unhandled operator '" + text(*opNode) + "'");
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

Expression* Lowerer::prefixExpr(const Node& n) {
  const Child* opNode = child(n, F::Operator);
  const Child* operand = child(n, F::Operand);
  if (opNode == nullptr || operand == nullptr) {
    return nullptr;
  }
  std::string op = text(*opNode);
  if (const char* call = open_range_call(op, false)) {
    Expression* e = expr(operand);
    return e == nullptr ? nullptr : Call::a(loc(n), ASTString(call), {e});
  }
  if (op == "not" || op == "¬") {
    Expression* e = expr(operand);
    return e == nullptr ? nullptr : new UnOp(loc(n), UOT_NOT, e);
  }
  // Fold the sign into the literal, so that -9223372036854775808 is
  // representable, as the bison lexer's MZN_MAX_NEGATIVE_INTEGER_LITERAL is.
  if (op == "-" && kind(*operand) == K::IntegerLiteral) {
    return intLiteral(*operand, true);
  }
  if (op == "-" && kind(*operand) == K::FloatLiteral) {
    return floatLiteral(*operand, true);
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

Expression* Lowerer::postfixExpr(const Node& n) {
  const Child* opNode = child(n, F::Operator);
  Expression* e = expr(child(n, F::Operand));
  if (e == nullptr || opNode == nullptr) {
    return nullptr;
  }
  std::string op = text(*opNode);
  if (op == "^-1" || op == "⁻¹") {
    if (e == Constants::constants().absent) {
      return e;  // `<>^-1` is `<>`, as in the bison grammar
    }
    return new BinOp(loc(n), e, BOT_POW, IntLit::a(-1));
  }
  if (const char* call = open_range_call(op, true)) {
    return Call::a(loc(n), ASTString(call), {e});
  }
  error(*opNode, "internal: unhandled postfix operator '" + op + "'");
  return nullptr;
}

Expression* Lowerer::argOrParam(const Node& n) {
  const Child* def = child(n, F::Default);
  if (def != nullptr) {
    return futureFeature(*def, "default values for call arguments");
  }
  const Child* e = child(n, F::Expression);
  if (e == nullptr) {
    return typeAsExpr(child(n, F::Type));
  }
  // Named argument: the name shares the grammar rule with a type
  const Child* nameNode = child(n, F::Type);
  Expression* nameExpr = typeAsExpr(nameNode);
  auto* id = Expression::dynamicCast<Id>(nameExpr);
  if (id == nullptr) {
    error(*nameNode, "invalid name for named argument");
    return nullptr;
  }
  if (!id->v().empty() && id->v().c_str()[0] == '_') {
    std::ostringstream oss;
    oss << "parameter `" << id->v()
        << "' starts with '_' and cannot be used as a named argument (pass it positionally)";
    error(*nameNode, oss.str());
    return nullptr;
  }
  Expression* value = expr(e);
  if (value == nullptr) {
    return nullptr;
  }
  return new VarDecl(loc(n), new TypeInst(loc(n), Type()), id->v(), value);
}

std::vector<Expression*> Lowerer::callArguments(const Node& n, bool& ok) {
  std::vector<Expression*> args;
  for (const Child* a : children(n, F::Argument)) {
    // Each `arg_or_param` was lowered when it reduced
    Expression* e = expr(a);
    if (e == nullptr) {
      ok = false;
      continue;
    }
    args.push_back(e);
  }
  return args;
}

Expression* Lowerer::callExpr(const Node& n) {
  const Child* fn = child(n, F::Function);
  if (fn == nullptr) {
    return nullptr;
  }
  // There are no first-class functions, so a call of a call is a generator call
  // whose head is not made of generators, such as `forall(i: 1..3)(p(i))`.
  if (kind(*fn) == K::Call) {
    if (auto* inner = Expression::dynamicCast<Call>(static_cast<Expression*>(fn->value))) {
      for (unsigned int i = 0; i < inner->argCount(); i++) {
        if (Expression::isa<VarDecl>(inner->arg(i))) {
          error(inner->arg(i), "illegal expression in generator call");
          return nullptr;
        }
      }
    }
    error(*fn, "illegal expression in generator call");
    return nullptr;
  }
  bool ok = true;
  std::vector<Expression*> args = callArguments(n, ok);
  if (!ok) {
    return nullptr;
  }
  if (kind(*fn) == K::Anonymous) {
    return Call::a(loc(n), Constants::constants().ids.anon_enum_set, args);
  }
  if (kind(*fn) == K::InversedIdentifier) {
    ASTString name = calleeName(fn);
    return name.empty() ? nullptr : Call::a(loc(n), name, args);
  }
  if (kind(*fn) == K::QuotedIdentifier) {
    std::string t = text(*fn);
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
    ASTString id = identifier(fn);
    return id.empty() ? nullptr : Call::a(loc(n), id, args);
  }
  if (kind(*fn) != K::Identifier) {
    error(*fn, "invalid function name in call");
    return nullptr;
  }
  std::string name = text(*fn);
  if (is_reserved_call_name(name)) {
    error(*fn, "syntax error, `" + name + "' is a reserved keyword");
    return nullptr;
  }
  return Call::a(loc(n), ASTString(name), args);
}

Expression* Lowerer::generatorCallExpr(const Node& n) {
  const Child* fn = child(n, F::Function);
  Generators gens;
  if (!generators(n, gens, /*idLocations=*/true)) {
    return nullptr;
  }
  Expression* tmpl = expr(child(n, F::Template));
  if (tmpl == nullptr || fn == nullptr) {
    return nullptr;
  }
  auto* comp = new Comprehension(loc(n), tmpl, gens, false);
  K k = kind(*fn);
  if (k != K::InversedIdentifier && k != K::Identifier && k != K::QuotedIdentifier) {
    error(*fn, "illegal expression in generator call");
    return nullptr;
  }
  ASTString name = calleeName(fn);
  if (name.empty()) {
    return nullptr;
  }
  return Call::a(loc(n), name, {comp});
}

Expression* Lowerer::indexedAccess(const Node& n) {
  Expression* v = expr(child(n, F::Collection));
  if (v == nullptr) {
    return nullptr;
  }
  std::vector<Expression*> idx;
  for (const Child* i : children(n, F::Index)) {
    // A bare range operator token is a slice covering the whole dimension
    if (_s.isToken(i->symbol) && kind(*i) == K::Unknown) {
      std::string op = text(*i);
      if (op == "..") {
        idx.push_back(new SetLit(loc(*i), IntSetVal::a(-IntVal::infinity(), IntVal::infinity())));
      } else {
        idx.push_back(Call::a(loc(*i), ASTString("'" + op + "'"), std::vector<Expression*>()));
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

Expression* Lowerer::setLiteral(const Node& n) {
  if (text(n) == "∅") {
    return futureFeature(n, "empty set literals written as `∅'");
  }
  std::vector<Expression*> members;
  for (const Child* m : children(n, F::Member)) {
    Expression* e = expr(m);
    if (e == nullptr) {
      return nullptr;
    }
    members.push_back(e);
  }
  return new SetLit(loc(n), members);
}

Expression* Lowerer::arrayLiteral(const Node& n) {
  // Only a leading run of members may carry indices; `[0: x, y]` means the
  // array starts at index 0, so the index and value lists may differ in length.
  std::vector<Expression*> indices;
  std::vector<Expression*> values;
  for (const Child* m : children(n, F::Member)) {
    // Only a member written with an index gets a node of its own; a plain one
    // is the value itself.
    bool isIndexed = kind(*m) == K::ArrayLiteralMember;
    Expression* v = isIndexed ? expr(child(node(*m), F::Value)) : expr(m);
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
    Expression* i = expr(child(node(*m), F::Index));
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

Expression* Lowerer::arrayLiteral2d(const Node& n) {
  std::vector<Expression*> columnHeader;
  for (const Child* c : children(n, F::ColumnIndex)) {
    Expression* e = expr(c);
    if (e == nullptr) {
      return nullptr;
    }
    columnHeader.push_back(e);
  }
  std::vector<Expression*> rowHeader;
  std::vector<std::vector<Expression*>> rows;
  for (const Child* rc : children(n, F::Row)) {
    Node r = node(*rc);
    const Child* idxNode = child(r, F::Index);
    bool hasIndex = idxNode != nullptr;
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
    for (const Child* m : children(r, F::Member)) {
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

Expression* Lowerer::arrayLiteral3d(const Node& n) {
  std::vector<std::vector<std::vector<Expression*>>> slices;
  for (const Child* sc : children(n, F::Slice)) {
    std::vector<std::vector<Expression*>> rows;
    for (const Child* rc : children(node(*sc), F::Row)) {
      std::vector<Expression*> row;
      for (const Child* m : children(node(*rc), F::Member)) {
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

Expression* Lowerer::tupleLiteral(const Node& n) {
  std::vector<Expression*> members;
  for (const Child* m : children(n, F::Member)) {
    Expression* e = expr(m);
    if (e == nullptr) {
      return nullptr;
    }
    members.push_back(e);
  }
  return ArrayLit::constructTuple(loc(n), members);
}

Expression* Lowerer::recordLiteral(const Node& n) {
  std::vector<Expression*> fields;
  for (const Child* mc : children(n, F::Member)) {
    Node m = node(*mc);
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

bool Lowerer::generators(const Node& n, Generators& gens, bool idLocations) {
  for (const Child* gc : children(n, F::Generator)) {
    Node g = node(*gc);
    if (g.kind == K::AssignmentGenerator) {
      const Child* nameNode = child(g, F::Name);
      ASTString name;
      if (!patternName(nameNode, name)) {
        return false;
      }
      Expression* value = expr(child(g, F::Value));
      if (value == nullptr) {
        return false;
      }
      if (idLocations) {
        std::vector<Id*> ids{new Id(loc(*nameNode), name, nullptr)};
        gens.g.emplace_back(ids, nullptr, value);
      } else {
        std::vector<std::string> ids{std::string(name.c_str(), name.size())};
        gens.g.emplace_back(ids, nullptr, value);
      }
      const Child* where = child(g, F::Where);
      if (where != nullptr) {
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
    for (const Child* nameNode : children(g, F::Name)) {
      ASTString name;
      if (!patternName(nameNode, name)) {
        return false;
      }
      ids.emplace_back(name.c_str(), name.size());
      if (idLocations) {
        idExprs.push_back(new Id(loc(*nameNode), name, nullptr));
      }
    }
    Expression* collection = expr(child(g, F::Collection));
    if (collection == nullptr) {
      return false;
    }
    Expression* where = nullptr;
    const Child* whereNode = child(g, F::Where);
    if (whereNode != nullptr) {
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

Expression* Lowerer::comprehension(const Node& n, bool isSet) {
  Generators gens;
  if (!generators(n, gens, false)) {
    return nullptr;
  }
  Expression* tmpl = expr(child(n, F::Template));
  if (tmpl == nullptr) {
    return nullptr;
  }
  const Child* idxNode = child(n, F::Index);
  if (idxNode != nullptr) {
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

Expression* Lowerer::iteExpr(const Node& n) {
  std::vector<const Child*> conds = children(n, F::Condition);
  std::vector<const Child*> results = children(n, F::Result);
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
  const Child* elseNode = child(n, F::Else);
  if (elseNode != nullptr) {
    elseE = expr(elseNode);
    if (elseE == nullptr) {
      return nullptr;
    }
  }
  return new ITE(loc(n), ifThen, elseE);
}

Expression* Lowerer::letExpr(const Node& n) {
  std::vector<Expression*> lets;
  for (const Child* l : children(n, F::Item)) {
    auto* it = static_cast<Item*>(l->value);
    if (it == nullptr) {
      return nullptr;
    }
    if (auto* ci = Item::dynamicCast<ConstraintI>(it)) {
      // As `let_vardecl_item_list` takes the expression of a `constraint_item`
      lets.push_back(ci->e());
      continue;
    }
    VarDecl* vd = it->cast<VarDeclI>()->e();
    Expression::loc(vd, it->loc());
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

Expression* Lowerer::intLiteral(const Child& c, bool negated) {
  const char* b = _buf + c.startByte;
  const char* e = _buf + c.endByte;
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
    error(c, "invalid integer literal");
    return nullptr;
  }
  return IntLit::a(negated ? -v : v);
}

Expression* Lowerer::floatLiteral(const Child& c, bool negated) {
  const char* b = _buf + c.startByte;
  const char* e = _buf + c.endByte;
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
    error(c, "invalid float literal");
    return nullptr;
  }
  return FloatLit::a(negated ? -v : v);
}

bool Lowerer::appendStringPiece(const Child& c, std::string& out) {
  switch (kind(c)) {
    case K::StringCharacters:
      out.append(_buf + c.startByte, _buf + c.endByte);
      break;
    case K::EscapeSequence: {
      std::string e = text(c);
      if (e.size() < 2) {
        error(c, "invalid escape sequence");
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
            error(c, "invalid escape sequence");
            return false;
          }
          out += static_cast<char>(v.toInt());
          break;
        }
        case 'u':
        case 'U': {
          IntVal v;
          if (!based_to_intval(e.data() + 2, e.data() + e.size(), 16, v)) {
            error(c, "invalid escape sequence");
            return false;
          }
          // Unicode stops at U+10FFFF, and the surrogate halves are not
          // characters. Without this the 4-byte branch below truncates and
          // writes bytes that are not valid UTF-8.
          if (v < 0 || v > 0x10FFFF || (v >= 0xD800 && v <= 0xDFFF)) {
            error(c, "invalid code point in escape sequence");
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
            error(c, "invalid escape sequence");
            return false;
          }
          out += static_cast<char>(v.toInt());
          break;
        }
      }
      break;
    }
    default:
      break;
  }
  return true;
}

Expression* Lowerer::stringLiteral(const Node& n) {
  std::string s;
  for (const Child& c : n) {
    if (!appendStringPiece(c, s)) {
      return nullptr;
    }
  }
  return new StringLit(loc(n), s);
}

Expression* Lowerer::stringInterpolation(const Node& n) {
  struct Part {
    bool isString;
    std::string s;
    Expression* e;
  };
  std::vector<Part> parts;
  TSFieldId item = _s.field(F::Item);
  for (const Child& c : n) {
    K k = kind(c);
    if (k == K::StringCharacters || k == K::EscapeSequence) {
      if (parts.empty() || !parts.back().isString) {
        parts.push_back({true, {}, nullptr});
      }
      if (!appendStringPiece(c, parts.back().s)) {
        return nullptr;
      }
      continue;
    }
    if (c.field != item) {
      continue;
    }
    Expression* e = expr(&c);
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

ASTString Lowerer::identifier(const Child* c) {
  if (c == nullptr) {
    return {};
  }
  switch (kind(*c)) {
    case K::Identifier:
      return ASTString(text(*c));
    case K::QuotedIdentifier: {
      if (_pp.isFlatZinc) {
        error(*c, "quoted identifiers are not allowed in FlatZinc");
        return {};
      }
      std::string t = text(*c);
      std::string content = t.substr(1, t.size() - 2);
      if (const char* op = quoted_op_name(content)) {
        return ASTString(op);
      }
      if (content.empty()) {
        // Callers spell failure as an empty name, so this would delete the item
        error(*c, "syntax error, empty quoted identifier");
        return {};
      }
      return ASTString(content);
    }
    default:
      error(*c, "expected an identifier");
      return {};
  }
}

bool Lowerer::patternName(const Child* c, ASTString& out) {
  if (c == nullptr) {
    return false;
  }
  switch (kind(*c)) {
    case K::Identifier:
    case K::QuotedIdentifier:
      out = identifier(c);
      return !out.empty();
    case K::Anonymous:
      out = ASTString("");
      return true;
    default:
      futureFeature(*c, "destructuring patterns");
      return false;
  }
}

Expression* Lowerer::typeAsExpr(const Child* c) {
  if (c == nullptr) {
    return nullptr;
  }
  K k = kind(*c);
  if (k == K::TypeConcatenation) {
    // `a ++ b` in a position that also admits a type
    Node n = node(*c);
    Expression* lhs = typeAsExpr(child(n, F::Left));
    Expression* rhs = typeAsExpr(child(n, F::Right));
    if (lhs == nullptr || rhs == nullptr) {
      return nullptr;
    }
    return new BinOp(loc(*c), lhs, BOT_PLUSPLUS, rhs);
  }
  if (k != K::TypeBase && k != K::ConcatenatedDomain) {
    error(*c, "expected an expression, found a type");
    return nullptr;
  }
  Node n = node(*c);
  if (has(n, F::VarPar) || has(n, F::Opt) || has(n, F::Any)) {
    error(*c, "expected an expression, found a type");
    return nullptr;
  }
  const Child* domain = child(n, F::Domain);
  if (domain == nullptr) {
    return nullptr;
  }
  switch (kind(*domain)) {
    case K::PrimitiveType:
    case K::TypeInstId:
    case K::TypeInstEnumId:
      error(*domain, "expected an expression, found a type");
      return nullptr;
    case K::NewType:
      return futureFeature(*domain, "object types");
    default:
      return expr(domain);
  }
}

TypeInst* Lowerer::typeInst(const Child* c) {
  if (c == nullptr) {
    return nullptr;
  }
  switch (kind(*c)) {
    case K::TypeBase:
    case K::ConcatenatedDomain:
      return typeBase(node(*c));
    case K::ArrayType:
      return arrayTypeInst(node(*c));
    case K::ListType: {
      // `list of T` is `array[1..infinity] of T`
      TypeInst* inner = typeInst(child(node(*c), F::Type));
      if (inner == nullptr) {
        return nullptr;
      }
      TypeInst* ti = inner->isarray() ? new TypeInst(loc(*c), Type::tuple(), inner) : inner;
      std::vector<TypeInst*> ranges(1);
      ranges[0] =
          new TypeInst(loc(*c), Type(),
                       new BinOp(loc(*c), IntLit::a(1), BOT_DOTDOT, IntLit::a(IntVal::infinity())));
      ti->setRanges(ranges);
      return ti;
    }
    case K::SetType: {
      Node n = node(*c);
      TypeInst* inner = typeInst(child(n, F::Type));
      if (inner == nullptr) {
        return nullptr;
      }
      Type tt = Expression::type(inner);
      tt.st(Type::ST_SET);
      applyVarParOpt(n, tt);
      const Child* card = child(n, F::Cardinality);
      if (card != nullptr) {
        Expression* e = expr(card);
        if (e == nullptr) {
          return nullptr;
        }
        ArrayLit* marker = ArrayLit::constructTuple(loc(n), {e, inner});
        auto* ti = new TypeInst(loc(n), tt, marker);
        ti->setIsEnum(inner->isEnum());
        return ti;
      }
      inner->type(tt);
      return inner;
    }
    case K::TupleType: {
      Node n = node(*c);
      std::vector<Expression*> fields;
      for (const Child* f : children(n, F::Field)) {
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
      Node n = node(*c);
      std::vector<Expression*> fields;
      for (const Child* fc : children(n, F::Field)) {
        Node f = node(*fc);
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
      Node n = node(*c);
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
      return new TypeInst(loc(*c), Type::mkAny());
    case K::OperationType:
      return static_cast<TypeInst*>(futureFeature(*c, "function types"));
    default:
      error(*c, "internal: unhandled type kind");
      return nullptr;
  }
}

void Lowerer::applyVarParOpt(const Node& n, Type& t) const {
  if (const Child* vp = child(n, F::VarPar)) {
    if (text(*vp) == "var") {
      t.ti(Type::TI_VAR);
    }
    t.tiExplicit(true);
  }
  if (has(n, F::Opt)) {
    t.ot(Type::OT_OPTIONAL);
    t.otExplicit(true);
  }
}

TypeInst* Lowerer::typeBase(const Node& n) {
  const Child* domain = child(n, F::Domain);
  if (domain == nullptr) {
    return nullptr;
  }
  if (has(n, F::Any)) {
    // `any $X`; the lexer strips a single leading `$`
    return new TypeInst(loc(n), Type::mkAny(), new TIId(loc(*domain), text(*domain).substr(1)));
  }
  TypeInst* ti;
  switch (kind(*domain)) {
    case K::PrimitiveType: {
      std::string t = text(*domain);
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
      ti = new TypeInst(loc(n), Type::top(), new TIId(loc(*domain), text(*domain).substr(1)));
      break;
    case K::TypeInstEnumId:
      // `$$E`; likewise, so the name keeps one `$`
      ti = new TypeInst(loc(n), Type::parint(), new TIId(loc(*domain), text(*domain).substr(1)));
      break;
    case K::NewType:
      return static_cast<TypeInst*>(futureFeature(*domain, "object types"));
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

TypeInst* Lowerer::arrayTypeInst(const Node& n) {
  std::vector<TypeInst*> ranges;
  for (const Child* dc : children(n, F::Dimension)) {
    Node d = node(*dc);
    const Child* nameNode = child(d, F::Name);
    const Child* typeNode = child(d, F::Type);
    if (nameNode != nullptr) {
      // `array[i in S] of T`: the binder is smuggled through as a marker tuple
      ASTString name = identifier(nameNode);
      Expression* set = typeAsExpr(typeNode);
      if (name.empty() || set == nullptr) {
        return nullptr;
      }
      auto* binder = new Id(loc(*nameNode), name, nullptr);
      auto* rangeTi = new TypeInst(loc(*typeNode), Type(), set);
      ArrayLit* marker = ArrayLit::constructTuple(loc(d), {rangeTi, binder});
      ranges.push_back(new TypeInst(loc(d), Type(), marker));
      continue;
    }
    TypeInst* ti = typeInst(typeNode);
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

VarDecl* Lowerer::parameter(const Node& n) {
  TypeInst* ti = typeInst(child(n, F::Type));
  if (ti == nullptr) {
    return nullptr;
  }
  if (Expression::type(ti).any() && ti->domain() == nullptr) {
    error(n, "parameter declaration cannot have `any' type-inst without type-inst variable");
  }
  const Child* nameNode = child(n, F::Name);
  if (nameNode == nullptr) {
    // An unnamed parameter, as in `predicate p(int)`
    auto* anon = new VarDecl(loc(n), ti, ASTString());
    anon->toplevel(false);
    return anon;
  }
  ASTString name;
  if (!patternName(nameNode, name)) {
    return nullptr;
  }
  auto* vd = new VarDecl(loc(n), ti, new Id(loc(*nameNode), name, nullptr));
  vd->toplevel(false);
  add_annotations(vd, annotations(n));
  const Child* def = child(n, F::Default);
  if (def != nullptr) {
    Expression* e = expr(def);
    if (e == nullptr) {
      return nullptr;
    }
    vd->e(e);
  }
  return vd;
}

/// State changed while trying one grammar for a data file.
class Snapshot {
public:
  explicit Snapshot(const ParserState& pp)
      : _hadError(pp.hadError), _errors(pp.syntaxErrors.size()), _calls(pp.dataFileCalls.size()) {}
  void restore(ParserState& pp) const {
    pp.hadError = _hadError;
    pp.syntaxErrors.erase(pp.syntaxErrors.begin() + static_cast<long>(_errors),
                          pp.syntaxErrors.end());
    pp.dataFileCalls.resize(_calls);
  }

private:
  bool _hadError;
  size_t _errors;
  size_t _calls;
};

/// Parse `pp.buf` with the MiniZinc grammar, writing items to \a items.
/// Returns false and sets \a error if the grammar rejects the file.
bool parse_minizinc(ParserState& pp, Model* items, TFError& error,
                    bool* rejectedModelItem = nullptr) {
  Lowerer lowerer(pp, syms(), items);
  TFSink sink = {&lowerer,
                 [](void* payload, const TFToken* token, bool extra) -> void* {
                   return static_cast<Lowerer*>(payload)->shift(token, extra);
                 },
                 [](void* payload, const TFReduction* r) -> void* {
                   return static_cast<Lowerer*>(payload)->reduce(r);
                 },
                 nullptr};
  bool ok = tf_parse(tf_tables(), pp.buf, pp.length, &sink, nullptr, &error);
  if (rejectedModelItem != nullptr) {
    *rejectedModelItem = lowerer.rejectedModelItem();
  }
  return ok;
}

ParserLocation error_location(const ParserState& pp, const TFError& error) {
  return SourceText(pp).loc(error.byte, error.point.row, error.point.column, error.byte,
                            error.point.row, error.point.column);
}

using TreePtr = std::unique_ptr<TSTree, void (*)(TSTree*)>;
using ParserPtr = std::unique_ptr<TSParser, void (*)(TSParser*)>;

TreePtr parse_with(const TSLanguage* lang, const ParserState& pp) {
  ParserPtr parser(ts_parser_new(), ts_parser_delete);
  if (parser == nullptr) {
    throw InternalError("cannot create the tree-sitter diagnostic parser");
  }
  if (!ts_parser_set_language(parser.get(), lang)) {
    throw InternalError("tree-sitter rejected the MiniZinc grammar version");
  }
  TreePtr tree(ts_parser_parse_string(parser.get(), nullptr, pp.buf, pp.length), ts_tree_delete);
  if (tree == nullptr) {
    throw InternalError("tree-sitter could not build a diagnostic syntax tree");
  }
  return tree;
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

  TFError error;
  if (!pp.isDatafile) {
    if (parse_minizinc(pp, pp.model, error)) {
      return;
    }
    // tree-feller stops at the first error; tree-sitter's recovery says more
    TreePtr tree = parse_with(tree_sitter_minizinc(), pp);
    if (Diagnostics(pp, ts_tree_root_node(tree.get()), syms()).reportSyntaxErrors()) {
      return;
    }
    throw InternalError("tree-feller rejected '" + std::string(pp.filename) + "' at " +
                        std::to_string(error.point.row + 1) + ":" +
                        std::to_string(error.point.column + 1) + " (" + error.message +
                        "), but tree-sitter accepted it");
  }

  // A data file is committed only once a grammar accepts it, as the next attempt
  // parses the whole file again.
  Snapshot before(pp);
  {
    std::unique_ptr<Model> items(new Model);
    if (parse_datazinc(pp, items.get(), error)) {
      for (Item* i : *items) {
        pp.model->addItem(i);
      }
      return;
    }
  }
  before.restore(pp);

  // Valid MiniZinc is still accepted: a model item is an error, but an
  // expression only earns a warning.
  {
    std::unique_ptr<Model> items(new Model);
    TFError modelError;
    bool rejectedModelItem = false;
    if (parse_minizinc(pp, items.get(), modelError, &rejectedModelItem)) {
      if (!rejectedModelItem) {
        pp.addWarning(
            Location(error_location(pp, error)),
            "only data belongs in a data file; using a MiniZinc expression here is deprecated");
      }
      for (Item* i : *items) {
        pp.model->addItem(i);
      }
      return;
    }
  }
  before.restore(pp);
  add_syntax_error(pp, error_location(pp, error), error.byte,
                   std::string("syntax error, ") + error.message);
}

}  // namespace MiniZinc

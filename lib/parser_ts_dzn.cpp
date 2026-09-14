/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Lower DataZinc reductions directly into the AST. Store one pointer per array
 * member to keep large data files compact. */

#include <minizinc/model.hh>
#include <minizinc/parser.hh>

#include "parser_ts.hh"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <tree_feller.h>

extern "C" const TSLanguage* tree_sitter_datazinc(void);

namespace MiniZinc {

namespace {

/// Grammar symbols used by the lowerer, resolved once by name.
struct Syms {
  TSSymbol sourceFile = 0;
  TSSymbol assignment = 0;
  TSSymbol expression = 0;
  TSSymbol call = 0;
  TSSymbol infixOperator = 0;
  TSSymbol arrayLiteral = 0;
  TSSymbol arrayLiteralMember = 0;
  TSSymbol arrayLiteral2d = 0;
  TSSymbol arrayLiteral2dRow = 0;
  TSSymbol arrayLiteral3d = 0;
  TSSymbol arrayLiteral3dSlice = 0;
  TSSymbol arrayLiteral3dRow = 0;
  TSSymbol booleanLiteral = 0;
  TSSymbol setLiteral = 0;
  TSSymbol stringLiteral = 0;
  TSSymbol stringContent = 0;
  TSSymbol escapeSequence = 0;
  TSSymbol stringCharacters = 0;
  TSSymbol tupleLiteral = 0;
  TSSymbol recordLiteral = 0;
  TSSymbol recordMember = 0;
  TSSymbol identifier = 0;
  TSSymbol quotedIdentifier = 0;
  TSSymbol integerLiteral = 0;
  TSSymbol floatLiteral = 0;
  TSSymbol infinity = 0;
  TSSymbol absent = 0;
  TSSymbol anonymous = 0;
  TSSymbol callRepeat = 0;
  TSSymbol arrayRepeat = 0;
  TSSymbol array2dRepeat1 = 0;
  TSSymbol array2dRepeat2 = 0;
  TSSymbol array2dRowRepeat = 0;
  TSSymbol array3dRepeat = 0;
  TSSymbol array3dSliceRepeat = 0;
  TSSymbol setRepeat = 0;
  TSSymbol recordRepeat = 0;
  TSSymbol sourceRepeat = 0;

  explicit Syms(const TFLanguage* lang) {
    static const struct {
      const char* name;
      TSSymbol Syms::*field;
    } WANTED[] = {
        {"source_file", &Syms::sourceFile},
        {"assignment", &Syms::assignment},
        {"_expression", &Syms::expression},
        {"call", &Syms::call},
        {"infix_operator", &Syms::infixOperator},
        {"array_literal", &Syms::arrayLiteral},
        {"array_literal_member", &Syms::arrayLiteralMember},
        {"array_literal_2d", &Syms::arrayLiteral2d},
        {"array_literal_2d_row", &Syms::arrayLiteral2dRow},
        {"array_literal_3d", &Syms::arrayLiteral3d},
        {"array_literal_3d_slice", &Syms::arrayLiteral3dSlice},
        {"array_literal_3d_row", &Syms::arrayLiteral3dRow},
        {"boolean_literal", &Syms::booleanLiteral},
        {"set_literal", &Syms::setLiteral},
        {"string_literal", &Syms::stringLiteral},
        {"_string_content", &Syms::stringContent},
        {"escape_sequence", &Syms::escapeSequence},
        {"string_characters", &Syms::stringCharacters},
        {"tuple_literal", &Syms::tupleLiteral},
        {"record_literal", &Syms::recordLiteral},
        {"record_member", &Syms::recordMember},
        {"identifier", &Syms::identifier},
        {"quoted_identifier", &Syms::quotedIdentifier},
        {"integer_literal", &Syms::integerLiteral},
        {"float_literal", &Syms::floatLiteral},
        {"infinity", &Syms::infinity},
        {"absent", &Syms::absent},
        {"anonymous", &Syms::anonymous},
        {"call_repeat1", &Syms::callRepeat},
        {"array_literal_repeat1", &Syms::arrayRepeat},
        {"array_literal_2d_repeat1", &Syms::array2dRepeat1},
        {"array_literal_2d_repeat2", &Syms::array2dRepeat2},
        {"array_literal_2d_row_repeat1", &Syms::array2dRowRepeat},
        {"array_literal_3d_repeat1", &Syms::array3dRepeat},
        {"array_literal_3d_slice_repeat1", &Syms::array3dSliceRepeat},
        {"set_literal_repeat1", &Syms::setRepeat},
        {"record_literal_repeat1", &Syms::recordRepeat},
        {"source_file_repeat1", &Syms::sourceRepeat},
    };
    for (unsigned int s = 0; s <= UINT16_MAX; s++) {
      const char* name = tf_language_symbol_name(lang, static_cast<TSSymbol>(s));
      if (name == nullptr) {
        break;
      }
      for (const auto& w : WANTED) {
        if (this->*(w.field) == 0 && strcmp(name, w.name) == 0) {
          this->*(w.field) = static_cast<TSSymbol>(s);
        }
      }
    }
    for (const auto& w : WANTED) {
      if (this->*(w.field) == 0) {
        throw InternalError(std::string("DataZinc grammar has no rule '") + w.name + "'");
      }
    }
  }
};

/// Encode a repetition as its number of stack entries.
void* count_value(size_t n) { return reinterpret_cast<void*>(n); }
size_t value_count(void* v) { return reinterpret_cast<size_t>(v); }

/// Keep surrounding literals valid after a member fails to lower.
Expression* or_placeholder(void* v) {
  auto* e = static_cast<Expression*>(v);
  return e != nullptr ? e : Constants::constants().absent;
}

/// Return the first non-comment child.
const TFNode* structural_child(const TFReduction* r) {
  for (uint32_t i = 0; i < r->node_count; i++) {
    if (!r->children[i].extra) {
      return &r->children[i];
    }
  }
  return nullptr;
}

class Lowerer {
public:
  Lowerer(ParserState& pp, const Syms& syms, Model* items)
      : _pp(pp),
        _s(syms),
        _buf(pp.buf),
        _len(pp.length),
        _items(items),
        _filename(ASTString(pp.filename)) {
    _ascii = true;
    for (unsigned int i = 0; i < _len; i++) {
      if (static_cast<unsigned char>(_buf[i]) >= 0x80) {
        _ascii = false;
        break;
      }
    }
  }

  void* reduce(const TFReduction* r);

private:
  unsigned int codePointColumn(unsigned int lineStart, unsigned int byte) const;
  ParserLocation loc(unsigned int startByte, TFPoint start, unsigned int endByte,
                     TFPoint end) const;
  ParserLocation loc(const TFReduction* r) const {
    return loc(r->start_byte, r->start_point, r->end_byte, r->end_point);
  }
  ParserLocation loc(const TFNode& n) const {
    return loc(n.start_byte, n.start_point, n.end_byte, n.end_point);
  }

  void error(const ParserLocation& l, unsigned int startByte, const std::string& msg);
  void error(const TFNode& n, const std::string& msg) { error(loc(n), n.start_byte, msg); }

  std::string text(const TFNode& n) const { return {_buf + n.start_byte, _buf + n.end_byte}; }
  /// Return an identifier's AST spelling, or an empty string on failure.
  ASTString identifier(const TFNode& n);

  bool isMember(TSSymbol s) const {
    return s == _s.expression || s == _s.arrayLiteralMember || s == _s.recordMember;
  }
  bool isMemberRepeat(TSSymbol s) const {
    return s == _s.arrayRepeat || s == _s.setRepeat || s == _s.callRepeat || s == _s.recordRepeat ||
           s == _s.array2dRowRepeat || s == _s.array2dRepeat1;
  }
  size_t runTotal(const TFReduction* r) const;
  /// Collect this rule's members in source order. Valid until the next call.
  const std::vector<Expression*>& members(const TFReduction* r);

  Expression* expression(const TFReduction* r);
  Expression* intLiteral(const TFNode& n);
  Expression* floatLiteral(const TFNode& n);
  std::string escape(const TFNode& n);
  Expression* stringLiteral(const TFReduction* r, const ParserLocation& l);
  Expression* arrayLiteral(const TFReduction* r, const ParserLocation& l);
  Expression* arrayLiteral2d(const TFReduction* r, const ParserLocation& l);
  Expression* arrayLiteral3d(const TFReduction* r, const ParserLocation& l);
  Expression* recordMember(const TFReduction* r, const ParserLocation& l);
  Expression* recordLiteral(const TFReduction* r, const ParserLocation& l);
  Expression* callExpr(const TFReduction* r, const ParserLocation& l);
  Expression* infixExpr(const TFReduction* r, const ParserLocation& l);
  void* row2d(const TFReduction* r);
  void* row3d(const TFReduction* r);
  void* slice3d(const TFReduction* r);
  void* assignment(const TFReduction* r, const ParserLocation& l);
  void noteDataCall(Call* c);

  ParserState& _pp;
  const Syms& _s;
  const char* _buf;
  unsigned int _len;
  /// Holds items until parsing succeeds and roots them for GC.
  Model* _items;
  ASTString _filename;
  bool _ascii;

  /// List members awaiting collection, in source order.
  std::vector<Expression*> _exprs;
  /// Indexed members and their positions in `_exprs`.
  std::vector<std::pair<size_t, Expression*>> _indexed;
  struct Row {
    Expression* index;
    size_t count;
    ParserLocation loc;
    unsigned int startByte;
  };
  std::vector<Row> _rows;
  std::vector<size_t> _slices;
  std::vector<std::string> _pieces;
  /// Reused to avoid reallocating large member lists.
  std::vector<Expression*> _scratch;

  mutable unsigned int _colLineStart = 0;
  mutable unsigned int _colByte = 0;
  mutable unsigned int _colCount = 0;
};

unsigned int Lowerer::codePointColumn(unsigned int lineStart, unsigned int byte) const {
  if (_ascii) {
    return byte - lineStart;
  }
  // Continue from the previous column to avoid rescanning long lines.
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

ParserLocation Lowerer::loc(unsigned int startByte, TFPoint start, unsigned int endByte,
                            TFPoint end) const {
  // TFPoint columns are byte offsets.
  unsigned int firstColumn = codePointColumn(startByte - start.column, startByte) + 1;
  unsigned int lastColumn = codePointColumn(endByte - end.column, endByte);
  if (end.row == start.row && lastColumn < firstColumn) {
    lastColumn = firstColumn;
  }
  return {_filename, start.row + 1 + _pp.lineOffset, firstColumn, end.row + 1 + _pp.lineOffset,
          lastColumn};
}

void Lowerer::error(const ParserLocation& l, unsigned int startByte, const std::string& msg) {
  _pp.hadError = true;
  std::vector<ASTString> includeStack;
  for (Model* m = _pp.model; m != nullptr; m = m->parent()) {
    if (m->parent() != nullptr) {
      includeStack.push_back(m->filename());
    }
  }
  _pp.syntaxErrors.emplace_back(Location(l),
                                _pp.getCurrentLine(startByte, static_cast<int>(l.firstColumn()),
                                                   static_cast<int>(l.lastColumn())),
                                includeStack, msg);
}

ASTString Lowerer::identifier(const TFNode& n) {
  if (n.symbol == _s.identifier) {
    return ASTString(text(n));
  }
  std::string t = text(n);
  std::string content = t.substr(1, t.size() - 2);
  if (const char* op = quoted_op_name(content)) {
    return ASTString(op);
  }
  if (content.empty()) {
    error(n, "syntax error, empty quoted identifier");
    return {};
  }
  return ASTString(content);
}

size_t Lowerer::runTotal(const TFReduction* r) const {
  size_t total = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    if (isMemberRepeat(r->children[i].symbol)) {
      total += value_count(r->children[i].value);
    }
  }
  return total;
}

const std::vector<Expression*>& Lowerer::members(const TFReduction* r) {
  size_t total = runTotal(r);
  size_t base = _exprs.size() - total;
  size_t cursor = 0;
  std::vector<Expression*>& out = _scratch;
  out.clear();
  out.reserve(total + 2);
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (isMemberRepeat(c.symbol)) {
      size_t n = value_count(c.value);
      out.insert(out.end(), _exprs.begin() + static_cast<long>(base + cursor),
                 _exprs.begin() + static_cast<long>(base + cursor + n));
      cursor += n;
    } else if (isMember(c.symbol)) {
      out.push_back(or_placeholder(c.value));
    }
  }
  _exprs.resize(base);
  return out;
}

Expression* Lowerer::expression(const TFReduction* r) {
  const TFNode* child = structural_child(r);
  if (child == nullptr) {
    return nullptr;
  }
  const TFNode& c = *child;
  if (c.symbol == _s.integerLiteral) {
    return intLiteral(c);
  }
  if (c.symbol == _s.floatLiteral) {
    return floatLiteral(c);
  }
  if (c.symbol == _s.identifier || c.symbol == _s.quotedIdentifier) {
    ASTString id = identifier(c);
    return id.empty() ? nullptr : new Id(loc(c), id, nullptr);
  }
  if (c.symbol == _s.absent) {
    return Constants::constants().absent;
  }
  if (c.symbol == _s.anonymous) {
    return new AnonVar(loc(c));
  }
  if (c.symbol == _s.infinity) {
    // DataZinc folds the sign into the token, so `-infinity` is one node
    return IntLit::a(_buf[c.start_byte] == '-' ? -IntVal::infinity() : IntVal::infinity());
  }
  return static_cast<Expression*>(c.value);
}

Expression* Lowerer::intLiteral(const TFNode& n) {
  const char* b = _buf + n.start_byte;
  const char* e = _buf + n.end_byte;
  bool negated = false;
  if (b != e && *b == '-') {
    negated = true;
    b++;
  }
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

Expression* Lowerer::floatLiteral(const TFNode& n) {
  const char* b = _buf + n.start_byte;
  const char* e = _buf + n.end_byte;
  bool negated = false;
  if (b != e && *b == '-') {
    negated = true;
    b++;
  }
  // strtod requires a terminated string.
  char stack[64];
  std::string heap;
  const char* s;
  auto len = static_cast<size_t>(e - b);
  if (len < sizeof(stack)) {
    memcpy(stack, b, len);
    stack[len] = '\0';
    s = stack;
  } else {
    heap.assign(b, e);
    s = heap.c_str();
  }
#ifdef _WIN32
  double v = _strtod_l(s, nullptr, _pp.cLocale);
#else
  double v = strtod_l(s, nullptr, _pp.cLocale);
#endif
  // Deliberately no ERANGE check: denormals such as 4.9e-324 are accepted.
  if (!std::isfinite(v)) {
    error(n, "invalid float literal");
    return nullptr;
  }
  return FloatLit::a(negated ? -v : v);
}

std::string Lowerer::escape(const TFNode& n) {
  std::string e = text(n);
  std::string out;
  if (e.size() < 2) {
    error(n, "invalid escape sequence");
    return out;
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
        return out;
      }
      out += static_cast<char>(v.toInt());
      break;
    }
    case 'u':
    case 'U': {
      IntVal v;
      if (!based_to_intval(e.data() + 2, e.data() + e.size(), 16, v)) {
        error(n, "invalid escape sequence");
        return out;
      }
      if (v < 0 || v > 0x10FFFF || (v >= 0xD800 && v <= 0xDFFF)) {
        error(n, "invalid code point in escape sequence");
        return out;
      }
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
      IntVal v;
      if (!based_to_intval(e.data() + 1, e.data() + e.size(), 8, v)) {
        error(n, "invalid escape sequence");
        return out;
      }
      out += static_cast<char>(v.toInt());
      break;
    }
  }
  return out;
}

Expression* Lowerer::stringLiteral(const TFReduction* r, const ParserLocation& l) {
  size_t n = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    if (r->children[i].symbol == _s.stringContent) {
      n += value_count(r->children[i].value);
    }
  }
  std::string out;
  for (size_t i = _pieces.size() - n; i < _pieces.size(); i++) {
    out += _pieces[i];
  }
  _pieces.resize(_pieces.size() - n);
  return new StringLit(l, out);
}

Expression* Lowerer::arrayLiteral(const TFReduction* r, const ParserLocation& l) {
  size_t base = _exprs.size() - runTotal(r);
  const std::vector<Expression*>& values = members(r);

  // Select only indices recorded for this literal.
  size_t first = _indexed.size();
  while (first > 0 && _indexed[first - 1].first >= base) {
    first--;
  }
  size_t indexCount = _indexed.size() - first;
  // `[0: x, y]` permits one leading index followed by plain values.
  bool wellFormed = indexCount <= 1 || indexCount == values.size();
  for (size_t i = 0; wellFormed && i < indexCount; i++) {
    wellFormed = _indexed[first + i].first - base == i;
  }
  if (!wellFormed) {
    _indexed.resize(first);
    error(l, r->start_byte, "invalid array literal, mixing indexed and non-indexed values");
    return nullptr;
  }
  std::vector<Expression*> indices;
  indices.reserve(indexCount);
  for (size_t i = first; i < _indexed.size(); i++) {
    Expression* idx = or_placeholder(_indexed[i].second);
    auto* tup = Expression::dynamicCast<ArrayLit>(idx);
    if (tup != nullptr && tup->isTuple() && tup->size() == 1) {
      idx = (*tup)[0];
    }
    indices.push_back(idx);
  }
  _indexed.resize(first);

  if (indices.empty()) {
    return new ArrayLit(l, values);
  }
  const auto* tuple = Expression::dynamicCast<ArrayLit>(indices[0]);
  if (tuple == nullptr) {
    for (const auto* t : indices) {
      if (Expression::isa<ArrayLit>(t)) {
        error(l, r->start_byte, "syntax error, non-uniform indexed array literal");
        return nullptr;
      }
    }
    return Call::a(l, "arrayNd", {new ArrayLit(l, indices), new ArrayLit(l, values)});
  }
  if (indices.size() != values.size()) {
    error(l, r->start_byte, "syntax error, non-uniform indexed array literal");
    return nullptr;
  }
  std::vector<std::vector<Expression*>> dims(tuple->size());
  for (const auto* t : indices) {
    const auto* tup = Expression::dynamicCast<ArrayLit>(t);
    if (tup == nullptr || tup->size() != dims.size()) {
      error(l, r->start_byte, "syntax error, non-uniform indexed array literal");
      return nullptr;
    }
    for (unsigned int i = 0; i < dims.size(); i++) {
      dims[i].push_back((*tup)[i]);
    }
  }
  std::vector<Expression*> args(dims.size());
  for (unsigned int i = 0; i < dims.size(); i++) {
    args[i] = new ArrayLit(l, dims[i]);
  }
  args.push_back(new ArrayLit(l, values));
  return Call::a(l, "arrayNd", args);
}

void* Lowerer::row2d(const TFReduction* r) {
  // The first expression is an index only when followed by `:`.
  bool hasIndex = false;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (!c.extra && c.symbol != _s.expression && c.symbol != _s.array2dRowRepeat &&
        text(c) == ":") {
      hasIndex = true;
      break;
    }
  }
  size_t total = runTotal(r);
  size_t base = _exprs.size() - total;
  size_t cursor = 0;
  Expression* index = nullptr;
  bool firstExpression = true;
  std::vector<Expression*>& ordered = _scratch;
  ordered.clear();
  ordered.reserve(total + 1);
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.array2dRowRepeat) {
      size_t n = value_count(c.value);
      ordered.insert(ordered.end(), _exprs.begin() + static_cast<long>(base + cursor),
                     _exprs.begin() + static_cast<long>(base + cursor + n));
      cursor += n;
    } else if (c.symbol == _s.expression) {
      if (firstExpression && hasIndex) {
        index = or_placeholder(c.value);
      } else {
        ordered.push_back(or_placeholder(c.value));
      }
      firstExpression = false;
    }
  }
  _exprs.resize(base);
  _exprs.insert(_exprs.end(), ordered.begin(), ordered.end());
  _rows.push_back({index, ordered.size(), loc(r), r->start_byte});
  return nullptr;
}

Expression* Lowerer::arrayLiteral2d(const TFReduction* r, const ParserLocation& l) {
  size_t rowCount = 0;
  size_t columnCount = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.arrayLiteral2dRow) {
      rowCount++;
    } else if (c.symbol == _s.array2dRepeat2) {
      rowCount += value_count(c.value);
    } else if (c.symbol == _s.array2dRepeat1) {
      columnCount += value_count(c.value);
    }
  }
  std::vector<Row> mine(_rows.end() - static_cast<long>(rowCount), _rows.end());
  _rows.resize(_rows.size() - rowCount);

  size_t memberTotal = 0;
  for (const Row& row : mine) {
    memberTotal += row.count;
  }
  size_t base = _exprs.size() - memberTotal;
  size_t columnBase = base - columnCount;
  std::vector<Expression*> columnHeader(_exprs.begin() + static_cast<long>(columnBase),
                                        _exprs.begin() + static_cast<long>(base));
  std::vector<Expression*> rowHeader;
  std::vector<std::vector<Expression*>> grid;
  size_t at = base;
  for (const Row& row : mine) {
    bool hasIndex = row.index != nullptr;
    // Every row must use the same indexing form and width.
    if (!grid.empty() && hasIndex == rowHeader.empty()) {
      _exprs.resize(columnBase);
      error(row.loc, row.startByte,
            "syntax error, mixing indexed and non-indexed sub-arrays in 2d array literal");
      return nullptr;
    }
    if (hasIndex) {
      rowHeader.push_back(row.index);
    }
    std::vector<Expression*> cells(_exprs.begin() + static_cast<long>(at),
                                   _exprs.begin() + static_cast<long>(at + row.count));
    at += row.count;
    if (!grid.empty() && cells.size() != grid.back().size()) {
      _exprs.resize(columnBase);
      error(row.loc, row.startByte,
            "syntax error, all sub-arrays of 2d array literal must have the same length");
      return nullptr;
    }
    grid.push_back(std::move(cells));
  }
  _exprs.resize(columnBase);

  if (!columnHeader.empty() && !grid.empty() && grid[0].size() != columnHeader.size()) {
    error(l, r->start_byte,
          "syntax error, sub-array of 2d array literal has different length from index row");
    return nullptr;
  }
  if (columnHeader.empty() && rowHeader.empty()) {
    return new ArrayLit(l, grid);
  }
  std::vector<Expression*> flat;
  for (auto& row : grid) {
    for (auto* e : row) {
      flat.push_back(e);
    }
  }
  if (rowHeader.empty()) {
    size_t n = columnHeader.empty() ? 0 : flat.size() / columnHeader.size();
    rowHeader.resize(n);
    for (unsigned int i = 0; i < n; i++) {
      rowHeader[i] = IntLit::a(i + 1);
    }
  } else if (columnHeader.empty()) {
    size_t n = rowHeader.empty() ? 0 : flat.size() / rowHeader.size();
    columnHeader.resize(n);
    for (unsigned int i = 0; i < n; i++) {
      columnHeader[i] = IntLit::a(i + 1);
    }
  }
  return Call::a(
      l, "array2d",
      {new ArrayLit(l, rowHeader), new ArrayLit(l, columnHeader), new ArrayLit(l, flat)});
}

void* Lowerer::row3d(const TFReduction* r) {
  // A 3d row reuses the 2d row repetition.
  size_t total = runTotal(r);
  size_t n = total;
  for (uint32_t i = 0; i < r->node_count; i++) {
    if (r->children[i].symbol == _s.expression) {
      _exprs.insert(_exprs.end() - static_cast<long>(total), or_placeholder(r->children[i].value));
      n++;
    }
  }
  _rows.push_back({nullptr, n, loc(r), r->start_byte});
  return nullptr;
}

void* Lowerer::slice3d(const TFReduction* r) {
  size_t n = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.arrayLiteral3dRow) {
      n++;
    } else if (c.symbol == _s.array3dSliceRepeat) {
      n += value_count(c.value);
    }
  }
  _slices.push_back(n);
  return nullptr;
}

Expression* Lowerer::arrayLiteral3d(const TFReduction* r, const ParserLocation& l) {
  size_t sliceCount = 0;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.arrayLiteral3dSlice) {
      sliceCount++;
    } else if (c.symbol == _s.array3dRepeat) {
      sliceCount += value_count(c.value);
    }
  }
  std::vector<size_t> mine(_slices.end() - static_cast<long>(sliceCount), _slices.end());
  _slices.resize(_slices.size() - sliceCount);
  size_t rowTotal = 0;
  for (size_t n : mine) {
    rowTotal += n;
  }
  std::vector<Row> myRows(_rows.end() - static_cast<long>(rowTotal), _rows.end());
  _rows.resize(_rows.size() - rowTotal);

  size_t memberTotal = 0;
  for (const Row& row : myRows) {
    memberTotal += row.count;
  }
  size_t base = _exprs.size() - memberTotal;

  std::vector<std::pair<int, int>> dims(3);
  dims[0] = {1, static_cast<int>(mine.size())};
  dims[1] = {1, mine.empty() ? 0 : static_cast<int>(mine[0])};
  dims[2] = {1, myRows.empty() ? 0 : static_cast<int>(myRows[0].count)};
  for (size_t n : mine) {
    if (static_cast<int>(n) != dims[1].second) {
      _exprs.resize(base);
      error(l, r->start_byte,
            "syntax error, all sub-arrays of 3d array literal must have the same length");
      return nullptr;
    }
  }
  for (const Row& row : myRows) {
    if (static_cast<int>(row.count) != dims[2].second) {
      _exprs.resize(base);
      error(l, r->start_byte,
            "syntax error, all sub-arrays of 3d array literal must have the same length");
      return nullptr;
    }
  }
  std::vector<Expression*> flat(_exprs.begin() + static_cast<long>(base), _exprs.end());
  _exprs.resize(base);
  return new ArrayLit(l, flat, dims);
}

Expression* Lowerer::recordMember(const TFReduction* r, const ParserLocation& l) {
  ASTString name;
  Expression* value = nullptr;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.identifier || c.symbol == _s.quotedIdentifier) {
      name = identifier(c);
    } else if (c.symbol == _s.expression) {
      value = static_cast<Expression*>(c.value);
    }
  }
  if (name.empty() || value == nullptr) {
    return nullptr;
  }
  return new VarDecl(l, new TypeInst(l, Type()), name, value);
}

Expression* Lowerer::recordLiteral(const TFReduction* r, const ParserLocation& l) {
  const std::vector<Expression*>& fields = members(r);
  for (const auto* f : fields) {
    if (f == nullptr) {
      return nullptr;  // a member already reported why
    }
  }
  if (fields.empty()) {
    error(l, r->start_byte, "syntax error, empty record literal");
    return nullptr;
  }
  ArrayLit* al = ArrayLit::constructTuple(l, fields);
  Expression::type(al, Type::record());
  return al;
}

void Lowerer::noteDataCall(Call* c) {
  for (const char* f : DATA_FUNCTIONS) {
    if (c->id() == f) {
      return;
    }
  }
  _pp.dataFileCalls.push_back(c);
}

Expression* Lowerer::callExpr(const TFReduction* r, const ParserLocation& l) {
  const TFNode* fn = nullptr;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.identifier || c.symbol == _s.quotedIdentifier || c.symbol == _s.anonymous) {
      fn = &c;
      break;
    }
  }
  const std::vector<Expression*>& args = members(r);
  if (fn == nullptr) {
    error(l, r->start_byte, "invalid function name in call");
    return nullptr;
  }
  // parser_ts_mzn.cpp:callExpr, minus the forms the DataZinc grammar cannot produce.
  if (fn->symbol == _s.anonymous) {
    return Call::a(l, Constants::constants().ids.anon_enum_set, args);
  }
  if (fn->symbol == _s.quotedIdentifier) {
    std::string t = text(*fn);
    std::string name = t.substr(1, t.size() - 2);
    if (const QuotedOp* q = quoted_op(name)) {
      if (q->bot == -1) {
        if (args.size() != 1) {
          error(l, r->start_byte, "syntax error, unary operator with two arguments");
          return nullptr;
        }
        return new UnOp(l, UOT_NOT, args[0]);
      }
      if (args.size() != 2) {
        error(l, r->start_byte, "syntax error, binary operator with unary argument list");
        return nullptr;
      }
      auto bot = static_cast<BinOpType>(q->bot);
      if (bot == BOT_DOTDOT && Expression::isa<IntLit>(args[0]) &&
          Expression::isa<IntLit>(args[1])) {
        return new SetLit(l, IntSetVal::a(IntLit::v(Expression::cast<IntLit>(args[0])),
                                          IntLit::v(Expression::cast<IntLit>(args[1]))));
      }
      return new BinOp(l, args[0], bot, args[1]);
    }
    // Open ranges remain calls.
    ASTString id = identifier(*fn);
    if (id.empty()) {
      return nullptr;
    }
    Call* c = Call::a(l, id, args);
    noteDataCall(c);
    return c;
  }
  std::string name = text(*fn);
  if (is_reserved_call_name(name)) {
    error(*fn, "syntax error, `" + name + "' is a reserved keyword");
    return nullptr;
  }
  Call* c = Call::a(l, ASTString(name), args);
  noteDataCall(c);
  return c;
}

Expression* Lowerer::infixExpr(const TFReduction* r, const ParserLocation& l) {
  Expression* lhs = nullptr;
  Expression* rhs = nullptr;
  const TFNode* op = nullptr;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if (c.symbol == _s.expression) {
      (lhs == nullptr ? lhs : rhs) = static_cast<Expression*>(c.value);
    } else if (!c.extra) {
      op = &c;
    }
  }
  if (lhs == nullptr || rhs == nullptr || op == nullptr) {
    return nullptr;
  }
  std::string o = text(*op);
  if (o == "++") {
    return new BinOp(l, lhs, BOT_PLUSPLUS, rhs);
  }
  if (o == "union" || o == "∪") {
    return new BinOp(l, lhs, BOT_UNION, rhs);
  }
  if (o == "..") {
    if (Expression::isa<IntLit>(lhs) && Expression::isa<IntLit>(rhs)) {
      return new SetLit(l, IntSetVal::a(IntLit::v(Expression::cast<IntLit>(lhs)),
                                        IntLit::v(Expression::cast<IntLit>(rhs))));
    }
    return new BinOp(l, lhs, BOT_DOTDOT, rhs);
  }
  error(l, r->start_byte, "internal: unhandled operator '" + o + "'");
  return nullptr;
}

void* Lowerer::assignment(const TFReduction* r, const ParserLocation& l) {
  ASTString name;
  Expression* value = nullptr;
  for (uint32_t i = 0; i < r->node_count; i++) {
    const TFNode& c = r->children[i];
    if ((c.symbol == _s.identifier || c.symbol == _s.quotedIdentifier) && name.empty()) {
      name = identifier(c);
    } else if (c.symbol == _s.expression) {
      value = static_cast<Expression*>(c.value);
    }
  }
  if (name.empty() || value == nullptr) {
    return nullptr;
  }
  _items->addItem(new AssignI(l, name, value));
  return nullptr;
}

void* Lowerer::reduce(const TFReduction* r) {
  TSSymbol sym = r->symbol;
  if (sym == _s.expression) {
    return expression(r);
  }

  // Repetitions carry stack-entry counts, not copied lists.
  if (isMemberRepeat(sym) || sym == _s.array2dRepeat2 || sym == _s.array3dRepeat ||
      sym == _s.array3dSliceRepeat || sym == _s.stringContent) {
    size_t n = 0;
    for (uint32_t i = 0; i < r->node_count; i++) {
      const TFNode& c = r->children[i];
      if (c.symbol == sym) {
        n += value_count(c.value);
      } else if (sym == _s.stringContent) {
        if (c.symbol == _s.stringCharacters) {
          _pieces.emplace_back(text(c));
          n++;
        } else if (c.symbol == _s.escapeSequence) {
          _pieces.push_back(escape(c));
          n++;
        }
      } else if (sym == _s.array2dRepeat2) {
        n += static_cast<size_t>(c.symbol == _s.arrayLiteral2dRow);
      } else if (sym == _s.array3dRepeat) {
        n += static_cast<size_t>(c.symbol == _s.arrayLiteral3dSlice);
      } else if (sym == _s.array3dSliceRepeat) {
        n += static_cast<size_t>(c.symbol == _s.arrayLiteral3dRow);
      } else if (isMember(c.symbol)) {
        _exprs.push_back(or_placeholder(c.value));
        n++;
      }
    }
    return count_value(n);
  }

  if (sym == _s.escapeSequence) {
    return nullptr;  // consumed by `_string_content`
  }
  if (sym == _s.booleanLiteral) {
    const TFNode* c = structural_child(r);
    return Constants::constants().boollit(c != nullptr && _buf[c->start_byte] == 't');
  }
  if (sym == _s.arrayLiteralMember) {
    // `index: value`, with the index kept beside the value so that
    // `array_literal` can see which of its members had one.
    Expression* index = nullptr;
    Expression* value = nullptr;
    for (uint32_t i = 0; i < r->node_count; i++) {
      if (r->children[i].symbol != _s.expression) {
        continue;
      }
      (index == nullptr ? index : value) = static_cast<Expression*>(r->children[i].value);
    }
    _indexed.emplace_back(_exprs.size(), index);
    return value;
  }

  ParserLocation l = loc(r);
  if (sym == _s.stringLiteral) {
    return stringLiteral(r, l);
  }
  if (sym == _s.arrayLiteral) {
    return arrayLiteral(r, l);
  }
  if (sym == _s.setLiteral) {
    const TFNode* open = structural_child(r);
    if (open != nullptr && text(*open) == "∅") {
      error(l, r->start_byte,
            "empty set literals written as `∅' are not supported by this version of MiniZinc");
      return nullptr;
    }
    return new SetLit(l, members(r));
  }
  if (sym == _s.tupleLiteral) {
    return ArrayLit::constructTuple(l, members(r));
  }
  if (sym == _s.recordMember) {
    return recordMember(r, l);
  }
  if (sym == _s.recordLiteral) {
    return recordLiteral(r, l);
  }
  if (sym == _s.call) {
    return callExpr(r, l);
  }
  if (sym == _s.infixOperator) {
    return infixExpr(r, l);
  }
  if (sym == _s.arrayLiteral2dRow) {
    return row2d(r);
  }
  if (sym == _s.arrayLiteral2d) {
    return arrayLiteral2d(r, l);
  }
  if (sym == _s.arrayLiteral3dRow) {
    return row3d(r);
  }
  if (sym == _s.arrayLiteral3dSlice) {
    return slice3d(r);
  }
  if (sym == _s.arrayLiteral3d) {
    return arrayLiteral3d(r, l);
  }
  if (sym == _s.assignment) {
    return assignment(r, l);
  }
  return nullptr;  // `source_file` and its repetition: the items are already in
}

/// Load the shared, read-only DataZinc tables once.
const TFLanguage* datazinc_tables() {
  struct Holder {
    TFLanguage* lang;
    Holder() {
      const char* message = nullptr;
      lang = tf_language_load(tree_sitter_datazinc(), &message);
      if (lang == nullptr) {
        throw InternalError(message != nullptr ? message : "cannot load the DataZinc tables");
      }
    }
    ~Holder() { tf_language_free(lang); }
  };
  static const Holder holder;
  return holder.lang;
}

const Syms& syms() {
  static const Syms s(datazinc_tables());
  return s;
}

}  // namespace

bool parse_datazinc(ParserState& pp, Model* items, TFError& error) {
  Lowerer lowerer(pp, syms(), items);
  TFSink sink = {&lowerer, nullptr,
                 [](void* payload, const TFReduction* r) -> void* {
                   return static_cast<Lowerer*>(payload)->reduce(r);
                 },
                 nullptr};
  return tf_parse(datazinc_tables(), pp.buf, pp.length, &sink, nullptr, &error);
}

}  // namespace MiniZinc

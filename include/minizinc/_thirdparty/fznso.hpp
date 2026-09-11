// FZnSO — header-only C++17 bindings.
//
// Wraps the C interface in `fznso_types.h` so that a C++ program can build a
// model, dynamically load a solver library and run it, without touching the vtables
// or raw handles directly.
//
// This header covers the *consumer* side. Implementing a solver in C++ and
// exporting it across the interface is covered by `fznso_export.hpp`.

#ifndef FZNSO_HPP
#define FZNSO_HPP

#include "fznso_types.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iterator>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#ifdef _WIN32
// Defined only if the including program has not already chosen; `windows.h` is
// needed for the loader calls, and its macros would otherwise collide.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

/// Write a type to a stream, as `list of var opt set of int`.
///
/// At global scope rather than in `fznso` because `FznsoType` is a C struct
/// declared there: the global namespace is its only associated one, so this is
/// where argument-dependent lookup goes looking. A `fznso::Type` streams too,
/// converting to `FznsoType` on the way in.
inline std::ostream& operator<<(std::ostream& out, const FznsoType& type) {
	// Qualifiers go outermost first.
	if (type.list_of) {
		out << "list of ";
	}
	if (type.decision) {
		out << "var ";
	}
	if (type.opt) {
		out << "opt ";
	}
	if (type.set_of) {
		out << "set of ";
	}
	switch (type.base) {
	case FznsoTypeBaseBool:
		return out << "bool";
	case FznsoTypeBaseInt:
		return out << "int";
	case FznsoTypeBaseFloat:
		return out << "float";
	case FznsoTypeBaseString:
		return out << "string";
	}
	return out << "?";
}

namespace fznso {

// `FznsoDecisionIdx` and `FznsoConstraintIdx` are both `size_t` in C, so they
// are wrapped here to keep them distinct types.

/// The index of a decision variable within a model.
struct Decision {
	std::size_t index = 0;

	friend bool operator==(Decision a, Decision b) { return a.index == b.index; }
	friend bool operator!=(Decision a, Decision b) { return a.index != b.index; }
};

/// The index of a constraint within a model.
struct Constraint {
	std::size_t index = 0;

	friend bool operator==(Constraint a, Constraint b) { return a.index == b.index; }
	friend bool operator!=(Constraint a, Constraint b) { return a.index != b.index; }
};

/// An inclusive `[min, max]` range.
template <class T>
struct Range {
	T min;
	T max;

	friend bool operator==(const Range& a, const Range& b) {
		return a.min == b.min && a.max == b.max;
	}
};

/// Builds a [`FznsoType`] from a base type and the qualifiers you name.
///
/// `FznsoType` is a C struct, so it cannot carry the builder methods itself;
/// this holds one and converts back implicitly, so it can be used anywhere a
/// `FznsoType` is expected, including the `constexpr` capability tables a
/// solver declares. Every flag starts clear and is only set by the method that
/// names it, which is what keeps positional `bool`s out of the call:
///
/// ```
/// // `list of var int`
/// FznsoType arg = fznso::Type{FznsoTypeBaseInt}.list(true).decision(true);
/// // `var set of int`, or plain `var int`, depending on `of_sets`
/// FznsoType var = fznso::Type{FznsoTypeBaseInt}.decision(true).set(of_sets);
/// ```
class Type {
public:
	constexpr explicit Type(FznsoTypeBase base)
		: raw_{false, false, false, false, base} {}

	/// Whether this type is a list of its element type.
	constexpr Type list(bool list_of) const {
		Type t = *this;
		t.raw_.list_of = list_of;
		return t;
	}
	/// Whether this type is a decision variable rather than a parameter.
	constexpr Type decision(bool decision) const {
		Type t = *this;
		t.raw_.decision = decision;
		return t;
	}
	/// Whether this type is over sets of its base type rather than single values.
	constexpr Type set(bool set_of) const {
		Type t = *this;
		t.raw_.set_of = set_of;
		return t;
	}
	/// Whether this type also permits the absent value.
	constexpr Type opt(bool opt) const {
		Type t = *this;
		t.raw_.opt = opt;
		return t;
	}

	constexpr operator FznsoType() const { return raw_; }

private:
	FznsoType raw_;
};

/// The type as `<<` writes it, for when a `std::string` is what you actually
/// need — building a message by concatenation, say. Prefer streaming.
inline std::string to_string(const FznsoType& type) {
	std::ostringstream out;
	out << type;
	return out.str();
}

/// Borrow a string as the interface's string handle.
///
/// Identifiers and strings cross the interface as a pointer/length pair, never
/// as a null-terminated C string, so nothing here needs to be re-encoded or
/// null-terminated. `constexpr`, so the capability lists a solver declares can
/// name their identifiers directly.
constexpr FznsoStr str(std::string_view s) { return FznsoStr{s.data(), s.size()}; }

namespace detail {

/// Turns a `size()`/`operator[]` pair into something range-`for` can walk.
template <class Owner>
class IndexIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = decltype(std::declval<const Owner&>()[std::size_t{}]);
	using difference_type = std::ptrdiff_t;
	using pointer = void;
	using reference = value_type;

	IndexIterator(const Owner* owner, std::size_t index) : owner_(owner), index_(index) {}

	value_type operator*() const { return (*owner_)[index_]; }
	IndexIterator& operator++() {
		++index_;
		return *this;
	}
	bool operator==(const IndexIterator& other) const { return index_ == other.index_; }
	bool operator!=(const IndexIterator& other) const { return index_ != other.index_; }

private:
	const Owner* owner_;
	std::size_t index_;
};

/// Adds `begin()`/`end()` to a type providing `size()` and `operator[]`.
template <class Owner>
struct Iterable {
	IndexIterator<Owner> begin() const {
		return {static_cast<const Owner*>(this), 0};
	}
	IndexIterator<Owner> end() const {
		return {static_cast<const Owner*>(this), static_cast<const Owner*>(this)->size()};
	}
};

/// Marks an optional `run` callback the caller is not supplying, so that a null
/// thunk reaches the solver rather than one that does nothing.
struct Absent {};

/// Views a `FznsoStr` as a `string_view`; a null pointer becomes empty.
inline std::string_view to_view(FznsoStr s) {
	return s.ptr == nullptr ? std::string_view{} : std::string_view{s.ptr, s.len};
}

// --- Dynamic library loading -------------------------------------------------
//
// The two platforms are wrapped here so that `Library` itself needs no
// conditional compilation. Paths are UTF-8 on both.

#ifdef _WIN32

using LibraryHandle = ::HMODULE;

/// Widens a UTF-8 path for `LoadLibraryW`.
///
/// `LoadLibraryA` would interpret the path in the active code page, so a path
/// containing non-ASCII characters would not open.
inline std::wstring widen(const char* text) {
	int needed = ::MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
	if (needed <= 0) {
		return {};
	}
	std::wstring wide(static_cast<std::size_t>(needed) - 1, L'\0');
	::MultiByteToWideChar(CP_UTF8, 0, text, -1, wide.data(), needed);
	return wide;
}

inline LibraryHandle open_library(const char* path) {
	std::wstring wide = widen(path);
	if (wide.empty()) {
		return nullptr;
	}
	return ::LoadLibraryW(wide.c_str());
}

inline void close_library(LibraryHandle handle) { ::FreeLibrary(handle); }

inline void* find_symbol(LibraryHandle handle, const char* name) {
	return reinterpret_cast<void*>(::GetProcAddress(handle, name));
}

/// The most recent loader error, as a human-readable string.
inline std::string last_error() {
	::DWORD code = ::GetLastError();
	char* buffer = nullptr;
	::DWORD length = ::FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, code, 0, reinterpret_cast<char*>(&buffer), 0, nullptr);
	std::string message =
		length == 0 ? "error " + std::to_string(code) : std::string{buffer, length};
	if (buffer != nullptr) {
		::LocalFree(buffer);
	}
	while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
		message.pop_back();
	}
	return message;
}

#else

using LibraryHandle = void*;

inline LibraryHandle open_library(const char* path) {
	return ::dlopen(path, RTLD_NOW | RTLD_LOCAL);
}

inline void close_library(LibraryHandle handle) { ::dlclose(handle); }

inline void* find_symbol(LibraryHandle handle, const char* name) {
	// A cleared error is the only reliable way to tell a null symbol from a
	// missing one.
	::dlerror();
	void* symbol = ::dlsym(handle, name);
	return ::dlerror() == nullptr ? symbol : nullptr;
}

/// The most recent loader error, as a human-readable string.
inline std::string last_error() {
	const char* error = ::dlerror();
	return error == nullptr ? std::string{"unknown error"} : std::string{error};
}

#endif

/// Derive a solver's library name from its path, as a view into `path`: the base
/// name with any leading `lib` and everything from the first `.` removed, so
/// `.../libgecode.6.2.1.dylib`, `libgecode.so.6` and `gecode.dll` all yield
/// `gecode`. The view is valid for as long as `path` is.
///
/// See the [`Library`] constructor for the naming rules this assumes.
inline std::string_view name_from_path(const char* path) {
	std::string_view name{path};
	if (std::size_t slash = name.find_last_of("/\\"); slash != std::string_view::npos) {
		name.remove_prefix(slash + 1);
	}
	if (name.substr(0, 3) == "lib") {
		name.remove_prefix(3);
	}
	if (std::size_t dot = name.find('.'); dot != std::string_view::npos) {
		name = name.substr(0, dot);
	}
	return name;
}

/// Whether `name` can spell the `fznso_<name>_...` entry points.
///
/// The name is pasted straight into a symbol, so it must be a C identifier:
/// ASCII letters, digits and underscores, not starting with a digit. This
/// rejects what would otherwise surface as a baffling missing-symbol error — an
/// empty name from a dotfile, or a `-` from a versioned Windows file name such
/// as `gecode-6.dll`.
/// The file-name marker that identifies a shared library on this platform.
constexpr std::string_view lib_marker() {
#if defined(_WIN32)
	return ".dll";
#elif defined(__APPLE__)
	return ".dylib";
#else
	return ".so";
#endif
}

inline bool is_valid_name(std::string_view name) {
	auto ident_char = [](char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
		       c == '_';
	};
	if (name.empty() || (!((name[0] >= 'a' && name[0] <= 'z') ||
	                       (name[0] >= 'A' && name[0] <= 'Z') || name[0] == '_'))) {
		return false;
	}
	for (char c : name) {
		if (!ident_char(c)) {
			return false;
		}
	}
	return true;
}

/// Order two file names so embedded numbers compare numerically, putting
/// `libx.so.10` after `libx.so.9`. Leading zeros are insignificant.
inline bool natural_less(std::string_view a, std::string_view b) {
	std::size_t i = 0;
	std::size_t j = 0;
	while (i < a.size() && j < b.size()) {
		if (std::isdigit(static_cast<unsigned char>(a[i])) &&
		    std::isdigit(static_cast<unsigned char>(b[j]))) {
			std::size_t ai = i;
			std::size_t bj = j;
			while (ai < a.size() && std::isdigit(static_cast<unsigned char>(a[ai]))) {
				++ai;
			}
			while (bj < b.size() && std::isdigit(static_cast<unsigned char>(b[bj]))) {
				++bj;
			}
			std::string_view da = a.substr(i, ai - i);
			std::string_view db = b.substr(j, bj - j);
			da.remove_prefix(std::min(da.find_first_not_of('0'), da.size()));
			db.remove_prefix(std::min(db.find_first_not_of('0'), db.size()));
			if (da.size() != db.size()) {
				return da.size() < db.size();
			}
			if (da != db) {
				return da < db;
			}
			i = ai;
			j = bj;
		} else {
			if (a[i] != b[j]) {
				return a[i] < b[j];
			}
			++i;
			++j;
		}
	}
	return (a.size() - i) < (b.size() - j);
}

/// The version a solver file name carries, or empty if none. The file name with
/// the leading `lib`, the solver name, the platform marker and surrounding dots
/// removed, so `libgecode.so.6.2.1` and `libgecode.6.2.1.dylib` both give
/// `6.2.1`.
inline std::string version_from_path(const char* path) {
	std::string_view file{path};
	if (std::size_t slash = file.find_last_of("/\\"); slash != std::string_view::npos) {
		file.remove_prefix(slash + 1);
	}
	if (file.substr(0, 3) == "lib") {
		file.remove_prefix(3);
	}
	std::size_t name_end = file.find('.');
	if (name_end == std::string_view::npos) {
		return {};
	}
	std::string rest{file.substr(name_end)};
	std::string_view marker = lib_marker();
	if (std::size_t m = rest.find(marker); m != std::string::npos) {
		rest.erase(m, marker.size());
	}
	std::size_t begin = rest.find_first_not_of('.');
	if (begin == std::string::npos) {
		return {};
	}
	std::size_t end = rest.find_last_not_of('.');
	return rest.substr(begin, end - begin + 1);
}

/// Whether `requested` selects `actual`, matching whole dotted components so
/// that `6` selects `6.2.1` but not `60`. Empty matches any version.
inline bool version_matches(std::string_view requested, std::string_view actual) {
	// Compare component by component; the request selects `actual` if every one
	// of its components equals the matching component of `actual`.
	auto next = [](std::string_view& s, bool& done) -> std::string_view {
		if (done) {
			return {};
		}
		std::size_t dot = s.find('.');
		std::string_view head = s.substr(0, dot);
		if (dot == std::string_view::npos) {
			done = true;
		} else {
			s.remove_prefix(dot + 1);
		}
		return head;
	};
	bool r_done = requested.empty();
	bool a_done = false;
	while (!r_done) {
		std::string_view rc = next(requested, r_done);
		if (a_done) {
			return false; // request is more specific than `actual`
		}
		std::string_view ac = next(actual, a_done);
		if (rc != ac) {
			return false;
		}
	}
	return true;
}

/// Read an environment variable, or empty if unset.
inline std::string env(const char* name) {
	const char* value = std::getenv(name);
	return value == nullptr ? std::string{} : std::string{value};
}

} // namespace detail

class Value;
class ValueSource;
class AnnotationSource;
class IntRanges;
class FloatRanges;
class ValueList;

/// A [`Value`] resolved into its payload.
///
/// `std::monostate` is the absent value. Sets and lists keep borrowing the
/// value they came from rather than being copied out.
using ValueVariant = std::variant<std::monostate, bool, std::int64_t, double, std::string_view,
                               Decision, Constraint, IntRanges, FloatRanges, ValueList>;

/// A borrowed value handed across the interface.
///
/// The typed accessors are only valid for the matching `kind()`; prefer
/// `variant()`, which resolves the kind and the payload together.
///
/// The converting constructors *borrow* their argument — the value points at it
/// and reads it back on demand, without copying — so the argument must outlive
/// the value. This is how a value already held elsewhere is handed across the
/// interface. A default-constructed value is absent.
///
/// A type that decides its shape at runtime is handed over by deriving from
/// [`ValueSource`] and constructing a `Value` from it. To build an *owned*
/// value instead — one that outlives the storage it came from — use
/// [`OwnedValue`].
class Value {
public:
	/// The absent value.
	Value();
	explicit Value(FznsoValueRef raw) : raw_(raw) {}

	/// Borrow a boolean.
	explicit Value(const bool& value);
	/// Borrow a 64-bit integer.
	explicit Value(const std::int64_t& value);
	/// Borrow a 64-bit float.
	explicit Value(const double& value);
	/// Borrow a string.
	explicit Value(const std::string& value);
	/// Reference a decision variable (carried in the handle, nothing borrowed).
	explicit Value(Decision value);
	/// Reference a constraint (carried in the handle, nothing borrowed).
	explicit Value(Constraint value);
	/// Borrow an integer set as an ordered list of inclusive ranges.
	explicit Value(const std::vector<Range<std::int64_t>>& ranges);
	/// Borrow a float set as an ordered list of inclusive ranges.
	explicit Value(const std::vector<Range<double>>& ranges);
	/// Borrow a list, whose elements are themselves scalars or [`ValueSource`]s.
	template <class T>
	explicit Value(const std::vector<T>& items);
	/// Borrow a [`ValueSource`], deciding the value's shape at runtime.
	///
	/// Dispatch is static: the vtable is specialised to `V`, so `V`'s own
	/// `kind()` and accessors are called directly.
	template <class V, std::enable_if_t<std::is_base_of<ValueSource, V>::value, int> = 0>
	explicit Value(const V& source);

	/// Which payload this value holds.
	FznsoValueKind kind() const { return raw_.methods->kind(raw_.data); }

	bool as_bool() const { return raw_.methods->as_bool(raw_.data); }
	std::int64_t as_int() const { return raw_.methods->as_int(raw_.data); }
	double as_float() const { return raw_.methods->as_float(raw_.data); }
	Decision as_decision() const { return Decision{raw_.methods->as_decision(raw_.data)}; }
	Constraint as_constraint() const { return Constraint{raw_.methods->as_constraint(raw_.data)}; }

	/// The string payload. Not null-terminated; the length comes from `len`.
	std::string_view as_string() const {
		const char* ptr = raw_.methods->as_string(raw_.data);
		return ptr == nullptr ? std::string_view{} : std::string_view{ptr, size()};
	}

	/// The number of list elements, set ranges, or string bytes.
	std::size_t size() const { return raw_.methods->len(raw_.data); }
	bool empty() const { return size() == 0; }

	/// The list element at `index`.
	///
	/// Indexing a value means list indexing; the ranges of a set are reached
	/// through `int_range`/`float_range` instead, so that `[]` has one meaning.
	Value operator[](std::size_t index) const {
		return Value{raw_.methods->get_element(raw_.data, index)};
	}
	/// The integer-set range at `index`.
	Range<std::int64_t> int_range(std::size_t index) const {
		FznsoIntRange r = raw_.methods->get_range_int(raw_.data, index);
		return {r.min, r.max};
	}
	/// The float-set range at `index`.
	Range<double> float_range(std::size_t index) const {
		FznsoFloatRange r = raw_.methods->get_range_float(raw_.data, index);
		return {r.min, r.max};
	}

	/// Resolve this value into its payload as a variant.
	ValueVariant variant() const;

	/// The underlying handle, for passing back across the interface.
	const FznsoValueRef& raw() const { return raw_; }

private:
	FznsoValueRef raw_{};
};

/// The ranges of an integer set, borrowed from the value that produced them.
class IntRanges : public detail::Iterable<IntRanges> {
public:
	explicit IntRanges(Value value) : value_(value) {}
	std::size_t size() const { return value_.size(); }
	bool empty() const { return size() == 0; }
	Range<std::int64_t> operator[](std::size_t index) const { return value_.int_range(index); }

private:
	Value value_;
};

/// The ranges of a float set, borrowed from the value that produced them.
class FloatRanges : public detail::Iterable<FloatRanges> {
public:
	explicit FloatRanges(Value value) : value_(value) {}
	std::size_t size() const { return value_.size(); }
	bool empty() const { return size() == 0; }
	Range<double> operator[](std::size_t index) const { return value_.float_range(index); }

private:
	Value value_;
};

/// The elements of a list value, borrowed from the value that produced them.
class ValueList : public detail::Iterable<ValueList> {
public:
	explicit ValueList(Value value) : value_(value) {}
	std::size_t size() const { return value_.size(); }
	bool empty() const { return size() == 0; }
	Value operator[](std::size_t index) const { return value_[index]; }

private:
	Value value_;
};

inline ValueVariant Value::variant() const {
	switch (kind()) {
	case FznsoValueAbsent:
		return std::monostate{};
	case FznsoValueBool:
		return as_bool();
	case FznsoValueInt:
		return as_int();
	case FznsoValueFloat:
		return as_float();
	case FznsoValueString:
		return as_string();
	case FznsoValueDecision:
		return as_decision();
	case FznsoValueConstraint:
		return as_constraint();
	case FznsoValueSetInt:
		return IntRanges{*this};
	case FznsoValueSetFloat:
		return FloatRanges{*this};
	case FznsoValueList:
		return ValueList{*this};
	}
	return std::monostate{};
}

namespace detail {
/// Reports that an accessor was called that does not match the value's kind.
[[noreturn]] inline void value_source_misuse(const char* accessor) {
	std::fprintf(stderr, "fznso::ValueSource::%s called for a value of another kind\n", accessor);
	std::abort();
}
} // namespace detail

/// Produce a [`Value`] from a type that decides its shape at runtime.
///
/// Implement this when a single downstream type can hold any kind — an
/// interpreter's expression, a variant AST node — and you want to hand it across
/// the interface without copying its payload into a [`OwnedValue`]. Construct a
/// [`Value`] from it; the value borrows this source, which must outlive it.
///
/// Implement `kind()` and the accessors your kinds use; the rest abort if
/// called, since a well-formed value only has its matching accessor invoked.
class ValueSource {
public:
	virtual ~ValueSource() = default;

	/// Which payload this value holds. Every other method must agree with it.
	virtual FznsoValueKind kind() const = 0;

	virtual bool as_bool() const { detail::value_source_misuse("as_bool"); }
	virtual std::int64_t as_int() const { detail::value_source_misuse("as_int"); }
	virtual double as_float() const { detail::value_source_misuse("as_float"); }
	/// The string payload; the returned view must outlive the value.
	virtual std::string_view as_string() const { detail::value_source_misuse("as_string"); }
	virtual Decision as_decision() const { detail::value_source_misuse("as_decision"); }
	virtual Constraint as_constraint() const { detail::value_source_misuse("as_constraint"); }

	/// The number of list elements or set ranges; zero for anything else.
	///
	/// A string's length is taken from `as_string`, so a string source need not
	/// implement this — only list and set sources do.
	virtual std::size_t size() const { return 0; }
	virtual Range<std::int64_t> int_range(std::size_t) const {
		detail::value_source_misuse("int_range");
	}
	virtual Range<double> float_range(std::size_t) const {
		detail::value_source_misuse("float_range");
	}
	/// The list element at `index`; the returned value must outlive this one.
	virtual Value element(std::size_t) const { detail::value_source_misuse("element"); }
};

/// An annotation attached to a decision, constraint or objective.
class AnnotationRef : public detail::Iterable<AnnotationRef> {
public:
	explicit AnnotationRef(FznsoAnnotationRef raw) : raw_(raw) {}

	/// Borrow an [`AnnotationSource`]. The source must outlive this reference.
	///
	/// Dispatch is static: the vtable is specialised to `A`, so `A`'s own methods
	/// are called directly.
	template <class A, std::enable_if_t<std::is_base_of<AnnotationSource, A>::value, int> = 0>
	explicit AnnotationRef(const A& source);

	/// The annotation's identifier.
	std::string_view ident() const { return detail::to_view(raw_.methods->ident(raw_.data)); }
	/// The number of arguments.
	std::size_t size() const { return raw_.methods->argument_len(raw_.data); }
	/// The argument at `index`.
	Value operator[](std::size_t index) const {
		return Value{raw_.methods->argument(raw_.data, index)};
	}

	/// The underlying handle, for passing back across the interface.
	const FznsoAnnotationRef& raw() const { return raw_; }

private:
	FznsoAnnotationRef raw_;
};

/// Produce an [`AnnotationRef`] from a type of your own.
///
/// The annotation counterpart to [`ValueSource`]: implement this to present a
/// downstream annotation type across the interface, and borrow it with
/// `AnnotationRef{source}`. The reference borrows the source, which must outlive
/// it.
class AnnotationSource {
public:
	virtual ~AnnotationSource() = default;

	/// The annotation's identifier.
	virtual std::string_view ident() const = 0;
	/// The number of arguments.
	virtual std::size_t size() const = 0;
	/// The argument at `index`.
	virtual Value argument(std::size_t index) const = 0;
};

struct OwnedValue;

/// An owned value: the counterpart to [`Value`], which only borrows.
///
/// Holds any of the shapes a value can take. Because it owns its storage, it can
/// outlive whatever it was built from — which is what a model needs, since it is
/// queried long after it is built. Construct one from a [`Value`] to copy a
/// borrowed value into owned storage.
///
/// It is itself a [`ValueSource`], so a `Value` borrowing it is just
/// `Value{owned}`.
struct OwnedValue final : ValueSource {
	/// The absent value.
	struct Absent {};

	std::variant<Absent, bool, std::int64_t, double, std::string, Decision, Constraint,
	             std::vector<Range<std::int64_t>>, std::vector<Range<double>>,
	             std::vector<OwnedValue>>
		payload;

	OwnedValue() : payload(Absent{}) {}
	OwnedValue(bool v) : payload(v) {}
	OwnedValue(std::int64_t v) : payload(v) {}
	OwnedValue(double v) : payload(v) {}
	OwnedValue(std::string v) : payload(std::move(v)) {}
	OwnedValue(const char* v) : payload(std::string{v}) {}
	OwnedValue(Decision v) : payload(v) {}
	OwnedValue(Constraint v) : payload(v) {}
	OwnedValue(std::vector<Range<std::int64_t>> v) : payload(std::move(v)) {}
	OwnedValue(std::vector<Range<double>> v) : payload(std::move(v)) {}
	OwnedValue(std::vector<OwnedValue> v) : payload(std::move(v)) {}

	/// An optional value: `std::nullopt` is absent, a present value takes the
	/// kind of `T`.
	template <class T>
	OwnedValue(std::optional<T> v)
		: OwnedValue(v.has_value() ? OwnedValue{std::move(*v)} : OwnedValue{}) {}

	/// Copy a borrowed [`Value`] into owned storage, recursively.
	explicit OwnedValue(const Value& value);

	/// An integer set covering a single inclusive range.
	static OwnedValue int_range(std::int64_t min, std::int64_t max) {
		return OwnedValue{std::vector<Range<std::int64_t>>{{min, max}}};
	}
	/// A float set covering a single inclusive range.
	static OwnedValue float_range(double min, double max) {
		return OwnedValue{std::vector<Range<double>>{{min, max}}};
	}

	// --- ValueSource ---

	FznsoValueKind kind() const override {
		switch (payload.index()) {
		case 1:
			return FznsoValueBool;
		case 2:
			return FznsoValueInt;
		case 3:
			return FznsoValueFloat;
		case 4:
			return FznsoValueString;
		case 5:
			return FznsoValueDecision;
		case 6:
			return FznsoValueConstraint;
		case 7:
			return FznsoValueSetInt;
		case 8:
			return FznsoValueSetFloat;
		case 9:
			return FznsoValueList;
		default:
			return FznsoValueAbsent;
		}
	}
	bool as_bool() const override { return std::get<bool>(payload); }
	std::int64_t as_int() const override { return std::get<std::int64_t>(payload); }
	double as_float() const override { return std::get<double>(payload); }
	std::string_view as_string() const override { return std::get<std::string>(payload); }
	Decision as_decision() const override { return std::get<Decision>(payload); }
	Constraint as_constraint() const override { return std::get<Constraint>(payload); }
	std::size_t size() const override {
		switch (payload.index()) {
		case 7:
			return std::get<std::vector<Range<std::int64_t>>>(payload).size();
		case 8:
			return std::get<std::vector<Range<double>>>(payload).size();
		case 9:
			return std::get<std::vector<OwnedValue>>(payload).size();
		default:
			return 0;
		}
	}
	Range<std::int64_t> int_range(std::size_t i) const override {
		return std::get<std::vector<Range<std::int64_t>>>(payload)[i];
	}
	Range<double> float_range(std::size_t i) const override {
		return std::get<std::vector<Range<double>>>(payload)[i];
	}
	Value element(std::size_t i) const override {
		return Value{std::get<std::vector<OwnedValue>>(payload)[i]};
	}
};

inline OwnedValue::OwnedValue(const Value& value) {
	switch (value.kind()) {
	case FznsoValueBool:
		payload = value.as_bool();
		break;
	case FznsoValueInt:
		payload = value.as_int();
		break;
	case FznsoValueFloat:
		payload = value.as_float();
		break;
	case FznsoValueString:
		payload = std::string{value.as_string()};
		break;
	case FznsoValueDecision:
		payload = value.as_decision();
		break;
	case FznsoValueConstraint:
		payload = value.as_constraint();
		break;
	case FznsoValueSetInt: {
		std::vector<Range<std::int64_t>> ranges;
		for (std::size_t i = 0; i < value.size(); ++i) {
			ranges.push_back(value.int_range(i));
		}
		payload = std::move(ranges);
		break;
	}
	case FznsoValueSetFloat: {
		std::vector<Range<double>> ranges;
		for (std::size_t i = 0; i < value.size(); ++i) {
			ranges.push_back(value.float_range(i));
		}
		payload = std::move(ranges);
		break;
	}
	case FznsoValueList: {
		std::vector<OwnedValue> items;
		for (std::size_t i = 0; i < value.size(); ++i) {
			items.emplace_back(value[i]);
		}
		payload = std::move(items);
		break;
	}
	case FznsoValueAbsent:
		break;
	}
}

/// An owned annotation: the counterpart to [`AnnotationRef`], which only
/// borrows. Its arguments are [`OwnedValue`]s.
///
/// It is itself an [`AnnotationSource`], so an `AnnotationRef` borrowing it is
/// just `AnnotationRef{owned}`.
struct OwnedAnnotation final : AnnotationSource {
	std::string ident_;
	std::vector<OwnedValue> arguments_;

	OwnedAnnotation() = default;
	OwnedAnnotation(std::string ident, std::vector<OwnedValue> arguments)
		: ident_(std::move(ident)), arguments_(std::move(arguments)) {}

	std::string_view ident() const override { return ident_; }
	std::size_t size() const override { return arguments_.size(); }
	Value argument(std::size_t index) const override { return Value{arguments_[index]}; }
};

/// The read-only view of a model that a solver is given.
///
/// This is the interface, not an implementation: subclass it to expose a model
/// you already hold in some other form, or use [`LayeredModel`] to build one.
/// A model is organised into *layers*, matching the incremental-solving concept
/// in the interface.
class Model {
public:
	virtual ~Model() = default;

	// --- Layers ---

	/// The number of layers currently in the model.
	virtual std::size_t layer_count() const = 0;
	/// How many layers are permanently committed, and so can never be popped.
	virtual std::size_t layer_permanent() const = 0;
	/// How many layers are unchanged since the last run.
	virtual std::size_t layer_unchanged() const = 0;
	/// How many permanent layers have been marked redundant.
	virtual std::size_t layer_redundant_count() const = 0;
	/// The layer index of the `n`-th redundant layer.
	virtual std::size_t layer_redundant_index(std::size_t n) const = 0;

	// --- Decision variables ---

	/// The total number of decision variables.
	virtual std::size_t decision_count() const = 0;
	/// One past the last decision index belonging to layers `0..=layer`.
	virtual std::size_t decision_layer_end(std::size_t layer) const = 0;
	/// The type of a decision variable.
	///
	/// What the variable *is*, as opposed to `decision_domain`, which says
	/// which values it may take: an absent domain is equally a `var bool` and
	/// an unbounded `var int`, and an integer range list is equally a `var int`
	/// and the upper bound of a `var set of int`. Build one with
	/// [`decision_type`].
	virtual FznsoType decision_type(Decision decision) const = 0;
	/// The domain of a decision variable.
	virtual Value decision_domain(Decision decision) const = 0;
	/// The name of a decision variable, if it has one.
	virtual std::optional<std::string_view> decision_name(Decision decision) const = 0;
	/// Whether a decision variable is functionally defined by a constraint.
	virtual bool decision_defined(Decision decision) const = 0;
	/// Whether a solution may be asked for a decision variable's value.
	///
	/// A solver must give every such variable a value in every solution it
	/// reports, and is free to leave the others open. Having a name does not
	/// make a variable needed: names exist only for debugging.
	virtual bool decision_in_solution(Decision decision) const = 0;
	/// The number of annotations on a decision variable.
	virtual std::size_t decision_annotation_count(Decision decision) const = 0;
	/// The `index`-th annotation on a decision variable.
	virtual AnnotationRef decision_annotation(Decision decision, std::size_t index) const = 0;

	// --- Constraints ---

	/// The total number of constraints.
	virtual std::size_t constraint_count() const = 0;
	/// One past the last constraint index belonging to layers `0..=layer`.
	virtual std::size_t constraint_layer_end(std::size_t layer) const = 0;
	/// The identifier of a constraint.
	virtual std::string_view constraint_ident(Constraint constraint) const = 0;
	/// The number of arguments of a constraint.
	virtual std::size_t constraint_argument_count(Constraint constraint) const = 0;
	/// The `index`-th argument of a constraint.
	virtual Value constraint_argument(Constraint constraint, std::size_t index) const = 0;
	/// The decision variable this constraint functionally defines, if any.
	virtual std::optional<Decision> constraint_defines(Constraint constraint) const = 0;
	/// The number of annotations on a constraint.
	virtual std::size_t constraint_annotation_count(Constraint constraint) const = 0;
	/// The `index`-th annotation on a constraint.
	virtual AnnotationRef constraint_annotation(Constraint constraint, std::size_t index) const = 0;

	// --- Objective ---

	/// The objective identifier, or empty for a satisfaction problem.
	virtual std::string_view objective_ident() const = 0;
	/// The objective's argument.
	virtual Value objective_arg() const = 0;
	/// The number of annotations on the objective.
	virtual std::size_t objective_annotation_count() const = 0;
	/// The `index`-th annotation on the objective.
	virtual AnnotationRef objective_annotation(std::size_t index) const = 0;
};

namespace detail {
/// The vtable of thunks specialised to a concrete model type `M`.
template <class M>
const FznsoModelMethods& model_methods();
} // namespace detail

/// The handle for a model, to pass across the interface.
///
/// `model` must be a concrete [`Model`] implementation, not an abstract
/// `const Model&`: the thunks name its methods on `M`, so a solver's queries
/// dispatch directly rather than through the virtual interface. The handle
/// borrows `model`, which must outlive it.
template <class M>
FznsoModelRef model_ref(const M& model) {
	static_assert(std::is_base_of<Model, M>::value, "model must derive from fznso::Model");
	static_assert(!std::is_abstract<M>::value, "model_ref needs a concrete model type");
	return FznsoModelRef{reinterpret_cast<const FznsoModel*>(&model), &detail::model_methods<M>()};
}

/// Presents a model received across the interface as a [`Model`].
///
/// This is what a solver implemented in C++ reads its input through.
class ModelRefAdapter final : public Model {
public:
	explicit ModelRefAdapter(FznsoModelRef raw) : raw_(raw) {}

	std::size_t layer_count() const override { return raw_.methods->layer_len(raw_.data); }
	std::size_t layer_permanent() const override {
		return raw_.methods->layer_permanent(raw_.data);
	}
	std::size_t layer_unchanged() const override {
		return raw_.methods->layer_unchanged(raw_.data);
	}
	std::size_t layer_redundant_count() const override {
		return raw_.methods->layer_redundant_len(raw_.data);
	}
	std::size_t layer_redundant_index(std::size_t n) const override {
		return raw_.methods->layer_redundant_index(raw_.data, n);
	}

	std::size_t decision_count() const override { return raw_.methods->decision_len(raw_.data); }
	std::size_t decision_layer_end(std::size_t layer) const override {
		return raw_.methods->decision_layer_end(raw_.data, layer);
	}
	FznsoType decision_type(Decision d) const override {
		return raw_.methods->decision_type(raw_.data, d.index);
	}
	Value decision_domain(Decision d) const override {
		return Value{raw_.methods->decision_domain(raw_.data, d.index)};
	}
	std::optional<std::string_view> decision_name(Decision d) const override {
		FznsoStr name = raw_.methods->decision_name(raw_.data, d.index);
		if (name.ptr == nullptr) {
			return std::nullopt;
		}
		return detail::to_view(name);
	}
	bool decision_defined(Decision d) const override {
		return raw_.methods->decision_defined(raw_.data, d.index);
	}
	bool decision_in_solution(Decision d) const override {
		return raw_.methods->decision_in_solution(raw_.data, d.index);
	}
	std::size_t decision_annotation_count(Decision d) const override {
		return raw_.methods->decision_annotation_len(raw_.data, d.index);
	}
	AnnotationRef decision_annotation(Decision d, std::size_t index) const override {
		return AnnotationRef{raw_.methods->decision_annotation(raw_.data, d.index, index)};
	}

	std::size_t constraint_count() const override { return raw_.methods->constraint_len(raw_.data); }
	std::size_t constraint_layer_end(std::size_t layer) const override {
		return raw_.methods->constraint_layer_end(raw_.data, layer);
	}
	std::string_view constraint_ident(Constraint c) const override {
		return detail::to_view(raw_.methods->constraint_ident(raw_.data, c.index));
	}
	std::size_t constraint_argument_count(Constraint c) const override {
		return raw_.methods->constraint_argument_len(raw_.data, c.index);
	}
	Value constraint_argument(Constraint c, std::size_t index) const override {
		return Value{raw_.methods->constraint_argument(raw_.data, c.index, index)};
	}
	std::optional<Decision> constraint_defines(Constraint c) const override {
		Value defines{raw_.methods->constraint_defines(raw_.data, c.index)};
		if (defines.kind() != FznsoValueDecision) {
			return std::nullopt;
		}
		return defines.as_decision();
	}
	std::size_t constraint_annotation_count(Constraint c) const override {
		return raw_.methods->constraint_annotation_len(raw_.data, c.index);
	}
	AnnotationRef constraint_annotation(Constraint c, std::size_t index) const override {
		return AnnotationRef{raw_.methods->constraint_annotation(raw_.data, c.index, index)};
	}

	std::string_view objective_ident() const override {
		return detail::to_view(raw_.methods->objective_ident(raw_.data));
	}
	Value objective_arg() const override { return Value{raw_.methods->objective_arg(raw_.data)}; }
	std::size_t objective_annotation_count() const override {
		return raw_.methods->objective_annotation_len(raw_.data);
	}
	AnnotationRef objective_annotation(std::size_t index) const override {
		return AnnotationRef{raw_.methods->objective_annotation(raw_.data, index)};
	}

private:
	FznsoModelRef raw_;
};

/// An in-memory [`Model`], built up in layers.
///
/// Decisions and constraints are added to the top layer, which can be pushed
/// and popped until it is marked permanent.
class LayeredModel final : public Model {
public:
	LayeredModel() : layers_(1), permanent_(1) {}

	// The handle it hands out points into this object, so it must not move.
	LayeredModel(const LayeredModel&) = delete;
	LayeredModel& operator=(const LayeredModel&) = delete;

	/// Add a decision variable to the top layer.
	///
	/// `type` says what the variable is — build one with [`decision_type`] —
	/// and `domain` which values it may take, which may be absent.
	Decision add_decision(FznsoType type, OwnedValue domain,
	                      std::optional<std::string> name = std::nullopt, bool defined = false,
	                      bool in_solution = true,
	                      std::vector<OwnedAnnotation> annotations = {}) {
		Decision idx{decision_count()};
		layers_.back().decisions.push_back(DecisionData{type, std::move(domain), std::move(name),
		                                                defined, in_solution,
		                                                std::move(annotations)});
		return idx;
	}

	/// Add a constraint to the top layer.
	Constraint add_constraint(std::string ident, std::vector<OwnedValue> arguments,
	                          std::optional<Decision> defines = std::nullopt,
	                          std::vector<OwnedAnnotation> annotations = {}) {
		Constraint idx{constraint_count()};
		OwnedValue defines_value;
		if (defines.has_value()) {
			defines_value = OwnedValue{*defines};
		}
		layers_.back().constraints.push_back(ConstraintData{
			std::move(ident), std::move(arguments), std::move(defines_value), std::move(annotations)});
		return idx;
	}

	/// Set the objective. Pass an empty identifier for a satisfaction problem.
	void set_objective(std::string ident, OwnedValue argument = {},
	                   std::vector<OwnedAnnotation> annotations = {}) {
		objective_ = ObjectiveData{std::move(ident), std::move(argument), std::move(annotations)};
	}

	/// Push a new, empty layer.
	void push_layer() { layers_.emplace_back(); }

	/// Pop the top layer. Throws if it is permanent.
	void pop_layer() {
		if (layers_.size() <= permanent_) {
			throw std::logic_error{"cannot pop a permanent layer"};
		}
		layers_.pop_back();
	}

	/// Mark every current layer as permanent, so it can never be popped.
	void mark_permanent() { permanent_ = layers_.size(); }

	/// Mark a permanent layer as redundant, letting the solver discard it.
	void mark_redundant(std::size_t layer) {
		if (layer >= permanent_) {
			throw std::out_of_range{"layer is not permanent"};
		}
		for (std::size_t existing : redundant_) {
			if (existing == layer) {
				return;
			}
		}
		redundant_.push_back(layer);
	}

	/// Declare how many layers are unchanged since the last run.
	void set_unchanged(std::size_t count) { unchanged_ = count; }

	std::size_t layer_count() const override { return layers_.size(); }
	std::size_t layer_permanent() const override { return permanent_; }
	std::size_t layer_unchanged() const override { return unchanged_; }
	std::size_t layer_redundant_count() const override { return redundant_.size(); }
	std::size_t layer_redundant_index(std::size_t n) const override { return redundant_[n]; }

	std::size_t decision_count() const override {
		std::size_t total = 0;
		for (const LayerData& layer : layers_) {
			total += layer.decisions.size();
		}
		return total;
	}
	std::size_t decision_layer_end(std::size_t layer) const override {
		std::size_t total = 0;
		for (std::size_t i = 0; i <= layer && i < layers_.size(); ++i) {
			total += layers_[i].decisions.size();
		}
		return total;
	}
	FznsoType decision_type(Decision d) const override { return decision_at(d.index).type; }
	Value decision_domain(Decision d) const override {
		return Value{decision_at(d.index).domain};
	}
	std::optional<std::string_view> decision_name(Decision d) const override {
		const std::optional<std::string>& name = decision_at(d.index).name;
		if (!name.has_value()) {
			return std::nullopt;
		}
		return std::string_view{*name};
	}
	bool decision_defined(Decision d) const override { return decision_at(d.index).defined; }
	bool decision_in_solution(Decision d) const override {
		return decision_at(d.index).in_solution;
	}
	std::size_t decision_annotation_count(Decision d) const override {
		return decision_at(d.index).annotations.size();
	}
	AnnotationRef decision_annotation(Decision d, std::size_t index) const override {
		return AnnotationRef{decision_at(d.index).annotations[index]};
	}

	std::size_t constraint_count() const override {
		std::size_t total = 0;
		for (const LayerData& layer : layers_) {
			total += layer.constraints.size();
		}
		return total;
	}
	std::size_t constraint_layer_end(std::size_t layer) const override {
		std::size_t total = 0;
		for (std::size_t i = 0; i <= layer && i < layers_.size(); ++i) {
			total += layers_[i].constraints.size();
		}
		return total;
	}
	std::string_view constraint_ident(Constraint c) const override {
		return constraint_at(c.index).ident;
	}
	std::size_t constraint_argument_count(Constraint c) const override {
		return constraint_at(c.index).arguments.size();
	}
	Value constraint_argument(Constraint c, std::size_t index) const override {
		return Value{constraint_at(c.index).arguments[index]};
	}
	std::optional<Decision> constraint_defines(Constraint c) const override {
		const OwnedValue& defines = constraint_at(c.index).defines;
		if (!std::holds_alternative<Decision>(defines.payload)) {
			return std::nullopt;
		}
		return std::get<Decision>(defines.payload);
	}
	std::size_t constraint_annotation_count(Constraint c) const override {
		return constraint_at(c.index).annotations.size();
	}
	AnnotationRef constraint_annotation(Constraint c, std::size_t index) const override {
		return AnnotationRef{constraint_at(c.index).annotations[index]};
	}

	std::string_view objective_ident() const override { return objective_.ident; }
	Value objective_arg() const override { return Value{objective_.argument}; }
	std::size_t objective_annotation_count() const override {
		return objective_.annotations.size();
	}
	AnnotationRef objective_annotation(std::size_t index) const override {
		return AnnotationRef{objective_.annotations[index]};
	}

private:
	struct DecisionData {
		FznsoType type;
		OwnedValue domain;
		std::optional<std::string> name;
		bool defined;
		bool in_solution;
		std::vector<OwnedAnnotation> annotations;
	};
	struct ConstraintData {
		std::string ident;
		std::vector<OwnedValue> arguments;
		OwnedValue defines;
		std::vector<OwnedAnnotation> annotations;
	};
	struct ObjectiveData {
		std::string ident;
		OwnedValue argument;
		std::vector<OwnedAnnotation> annotations;
	};
	struct LayerData {
		std::vector<DecisionData> decisions;
		std::vector<ConstraintData> constraints;
	};

	const DecisionData& decision_at(std::size_t index) const {
		for (const LayerData& layer : layers_) {
			if (index < layer.decisions.size()) {
				return layer.decisions[index];
			}
			index -= layer.decisions.size();
		}
		throw std::out_of_range{"decision index out of range"};
	}
	const ConstraintData& constraint_at(std::size_t index) const {
		for (const LayerData& layer : layers_) {
			if (index < layer.constraints.size()) {
				return layer.constraints[index];
			}
			index -= layer.constraints.size();
		}
		throw std::out_of_range{"constraint index out of range"};
	}

	std::vector<LayerData> layers_;
	ObjectiveData objective_{};
	std::size_t permanent_ = 1;
	std::size_t unchanged_ = 0;
	std::vector<std::size_t> redundant_;
};

namespace detail {

/// Publishes a `string_view` as the interface's string handle.
inline FznsoStr from_view(std::string_view s) { return FznsoStr{s.data(), s.size()}; }

/// Reports that a value vtable slot was called that this value does not
/// implement, and stops the process.
[[noreturn]] inline void value_slot_misuse(const char* accessor) {
	std::fprintf(stderr, "fznso: value accessor %s called for a value of another kind\n", accessor);
	std::abort();
}

/// A complete value vtable, describing the absent value.
///
/// Every slot is filled: the interface declares them as plain function
/// pointers, so a null slot is not a valid entry — a consumer in another
/// language may reject the whole table rather than read the null as
/// "unsupported". Slots a value does not implement therefore point at an
/// aborting stub instead of being left null.
///
/// Build a value's table by copying this and overriding only the slots that
/// value actually implements; the absent value is simply the unmodified
/// default.
inline FznsoValueMethods default_value_methods() {
	FznsoValueMethods m{};
	m.kind = [](const FznsoValue*) { return FznsoValueAbsent; };
	m.len = [](const FznsoValue*) -> std::size_t { return 0; };
	m.as_decision = [](const FznsoValue*) -> FznsoDecisionIdx { value_slot_misuse("as_decision"); };
	m.as_constraint = [](const FznsoValue*) -> FznsoConstraintIdx {
		value_slot_misuse("as_constraint");
	};
	m.as_int = [](const FznsoValue*) -> std::int64_t { value_slot_misuse("as_int"); };
	m.as_float = [](const FznsoValue*) -> double { value_slot_misuse("as_float"); };
	m.as_string = [](const FznsoValue*) -> const char* { value_slot_misuse("as_string"); };
	m.as_bool = [](const FznsoValue*) -> bool { value_slot_misuse("as_bool"); };
	m.get_range_int = [](const FznsoValue*, std::size_t) -> FznsoIntRange {
		value_slot_misuse("get_range_int");
	};
	m.get_range_float = [](const FznsoValue*, std::size_t) -> FznsoFloatRange {
		value_slot_misuse("get_range_float");
	};
	m.get_element = [](const FznsoValue*, std::size_t) -> FznsoValueRef {
		value_slot_misuse("get_element");
	};
	return m;
}

/// A decision reference carried *in* the handle's pointer word rather than
/// behind it, so that synthesising one needs no storage.
///
/// The pointer is never dereferenced; only `as_decision` reads it back.
inline FznsoValueRef decision_value(Decision decision) {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueDecision; };
		m.as_decision = [](const FznsoValue* v) -> FznsoDecisionIdx {
			return reinterpret_cast<std::uintptr_t>(v);
		};
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(
							 static_cast<std::uintptr_t>(decision.index)),
	                     &table};
}

/// A constraint reference, carried in the pointer word like [`decision_value`].
inline FznsoValueRef constraint_value(Constraint constraint) {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueConstraint; };
		m.as_constraint = [](const FznsoValue* v) -> FznsoConstraintIdx {
			return reinterpret_cast<std::uintptr_t>(v);
		};
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(
							 static_cast<std::uintptr_t>(constraint.index)),
	                     &table};
}

/// The absent value, which likewise needs no storage.
inline FznsoValueRef absent_value() {
	static const FznsoValueMethods table = default_value_methods();
	return FznsoValueRef{nullptr, &table};
}

// The following borrow a native C++ object: the handle points at it and the
// vtable reads it back, so nothing is copied. The borrowed object must outlive
// every use of the handle.

inline FznsoValueRef int_value(const std::int64_t& v) {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueInt; };
		m.as_int = [](const FznsoValue* p) { return *reinterpret_cast<const std::int64_t*>(p); };
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(&v), &table};
}

inline FznsoValueRef float_value(const double& v) {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueFloat; };
		m.as_float = [](const FznsoValue* p) { return *reinterpret_cast<const double*>(p); };
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(&v), &table};
}

inline FznsoValueRef bool_value(const bool& v) {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueBool; };
		m.as_bool = [](const FznsoValue* p) { return *reinterpret_cast<const bool*>(p); };
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(&v), &table};
}

inline FznsoValueRef string_value(const std::string& s) {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueString; };
		m.len = [](const FznsoValue* p) { return reinterpret_cast<const std::string*>(p)->size(); };
		m.as_string = [](const FznsoValue* p) {
			return reinterpret_cast<const std::string*>(p)->data();
		};
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(&s), &table};
}

inline FznsoValueRef int_set_value(const std::vector<Range<std::int64_t>>& ranges) {
	using Ranges = std::vector<Range<std::int64_t>>;
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueSetInt; };
		m.len = [](const FznsoValue* p) { return reinterpret_cast<const Ranges*>(p)->size(); };
		m.get_range_int = [](const FznsoValue* p, std::size_t i) {
			const Range<std::int64_t>& r = (*reinterpret_cast<const Ranges*>(p))[i];
			return FznsoIntRange{r.min, r.max};
		};
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(&ranges), &table};
}

inline FznsoValueRef float_set_value(const std::vector<Range<double>>& ranges) {
	using Ranges = std::vector<Range<double>>;
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) { return FznsoValueSetFloat; };
		m.len = [](const FznsoValue* p) { return reinterpret_cast<const Ranges*>(p)->size(); };
		m.get_range_float = [](const FznsoValue* p, std::size_t i) {
			const Range<double>& r = (*reinterpret_cast<const Ranges*>(p))[i];
			return FznsoFloatRange{r.min, r.max};
		};
		return m;
	}();
	return FznsoValueRef{reinterpret_cast<const FznsoValue*>(&ranges), &table};
}

} // namespace detail

inline Value::Value() : raw_(detail::absent_value()) {}
inline Value::Value(const bool& value) : raw_(detail::bool_value(value)) {}
inline Value::Value(const std::int64_t& value) : raw_(detail::int_value(value)) {}
inline Value::Value(const double& value) : raw_(detail::float_value(value)) {}
inline Value::Value(const std::string& value) : raw_(detail::string_value(value)) {}
inline Value::Value(Decision value) : raw_(detail::decision_value(value)) {}
inline Value::Value(Constraint value) : raw_(detail::constraint_value(value)) {}
inline Value::Value(const std::vector<Range<std::int64_t>>& ranges)
	: raw_(detail::int_set_value(ranges)) {}
inline Value::Value(const std::vector<Range<double>>& ranges)
	: raw_(detail::float_set_value(ranges)) {}

namespace detail {
/// The vtable for a list borrowing a `std::vector<T>`; each element is itself
/// borrowed by constructing a [`Value`] from it, so `T` may be a scalar or a
/// [`ValueSource`].
template <class T>
const FznsoValueMethods& list_methods() {
	static const FznsoValueMethods table = [] {
		FznsoValueMethods m = default_value_methods();
		m.kind = [](const FznsoValue*) noexcept { return FznsoValueList; };
		m.len = [](const FznsoValue* p) noexcept {
			return reinterpret_cast<const std::vector<T>*>(p)->size();
		};
		m.get_element = [](const FznsoValue* p, std::size_t i) noexcept {
			return Value{(*reinterpret_cast<const std::vector<T>*>(p))[i]}.raw();
		};
		return m;
	}();
	return table;
}
} // namespace detail

template <class T>
Value::Value(const std::vector<T>& items)
	: raw_(FznsoValueRef{reinterpret_cast<const FznsoValue*>(&items), &detail::list_methods<T>()}) {}

namespace detail {
/// The value vtable specialised to a concrete [`ValueSource`] type `V`.
///
/// The thunks cast to `V` and name its methods, so they dispatch directly even
/// though [`ValueSource`] is a virtual interface.
template <class V>
const FznsoValueMethods& value_source_methods() {
	static const FznsoValueMethods table = {
		[](const FznsoValue* p) noexcept { return reinterpret_cast<const V*>(p)->V::kind(); },
		[](const FznsoValue* p) noexcept -> std::size_t {
			const V& v = *reinterpret_cast<const V*>(p);
			return v.V::kind() == FznsoValueString ? v.V::as_string().size() : v.V::size();
		},
		[](const FznsoValue* p) noexcept -> FznsoDecisionIdx {
			return reinterpret_cast<const V*>(p)->V::as_decision().index;
		},
		[](const FznsoValue* p) noexcept -> FznsoConstraintIdx {
			return reinterpret_cast<const V*>(p)->V::as_constraint().index;
		},
		[](const FznsoValue* p) noexcept { return reinterpret_cast<const V*>(p)->V::as_int(); },
		[](const FznsoValue* p) noexcept { return reinterpret_cast<const V*>(p)->V::as_float(); },
		[](const FznsoValue* p) noexcept { return reinterpret_cast<const V*>(p)->V::as_string().data(); },
		[](const FznsoValue* p) noexcept { return reinterpret_cast<const V*>(p)->V::as_bool(); },
		[](const FznsoValue* p, std::size_t i) noexcept {
			Range<std::int64_t> r = reinterpret_cast<const V*>(p)->V::int_range(i);
			return FznsoIntRange{r.min, r.max};
		},
		[](const FznsoValue* p, std::size_t i) noexcept {
			Range<double> r = reinterpret_cast<const V*>(p)->V::float_range(i);
			return FznsoFloatRange{r.min, r.max};
		},
		[](const FznsoValue* p, std::size_t i) noexcept {
			return reinterpret_cast<const V*>(p)->V::element(i).raw();
		},
	};
	return table;
}
} // namespace detail

template <class V, std::enable_if_t<std::is_base_of<ValueSource, V>::value, int>>
Value::Value(const V& source)
	: raw_(FznsoValueRef{reinterpret_cast<const FznsoValue*>(&source),
                         &detail::value_source_methods<V>()}) {}

namespace detail {
/// The annotation vtable specialised to a concrete [`AnnotationSource`] type
/// `A`; the thunks name `A`'s methods, so they dispatch directly.
template <class A>
const FznsoAnnotationMethods& annotation_source_methods() {
	static const FznsoAnnotationMethods table = {
		[](const FznsoAnnotation* p) noexcept { return from_view(reinterpret_cast<const A*>(p)->A::ident()); },
		[](const FznsoAnnotation* p) noexcept { return reinterpret_cast<const A*>(p)->A::size(); },
		[](const FznsoAnnotation* p, std::size_t i) noexcept {
			return reinterpret_cast<const A*>(p)->A::argument(i).raw();
		},
	};
	return table;
}
} // namespace detail

template <class A, std::enable_if_t<std::is_base_of<AnnotationSource, A>::value, int>>
AnnotationRef::AnnotationRef(const A& source)
	: raw_(FznsoAnnotationRef{reinterpret_cast<const FznsoAnnotation*>(&source),
                             &detail::annotation_source_methods<A>()}) {}

namespace detail {

/// Reads back a concrete model `M` from the handle it was published as.
///
/// The returned reference has the concrete type, so the calls in the vtable
/// below bind statically rather than dispatching through [`Model`].
template <class M>
const M& model_as(const FznsoModel* model) {
	return *reinterpret_cast<const M*>(model);
}

template <class M>
const FznsoModelMethods& model_methods() {
	// Each thunk casts to the concrete `M` and names the method on `M`, so the
	// call is direct even though the queries are declared virtual on `Model`.
	static const FznsoModelMethods table = {
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::layer_count(); },
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::layer_unchanged(); },
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::layer_permanent(); },
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::layer_redundant_count(); },
		[](const FznsoModel* m, std::size_t n) noexcept { return model_as<M>(m).M::layer_redundant_index(n); },
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::decision_count(); },
		[](const FznsoModel* m, std::size_t l) noexcept { return model_as<M>(m).M::decision_layer_end(l); },
		[](const FznsoModel* m, FznsoDecisionIdx d) noexcept {
			return model_as<M>(m).M::decision_type(Decision{d});
		},
		[](const FznsoModel* m, FznsoDecisionIdx d) noexcept {
			return model_as<M>(m).M::decision_domain(Decision{d}).raw();
		},
		[](const FznsoModel* m, FznsoDecisionIdx d) noexcept {
			std::optional<std::string_view> name = model_as<M>(m).M::decision_name(Decision{d});
			return name.has_value() ? from_view(*name) : FznsoStr{nullptr, 0};
		},
		[](const FznsoModel* m, FznsoDecisionIdx d) noexcept {
			return model_as<M>(m).M::decision_defined(Decision{d});
		},
		[](const FznsoModel* m, FznsoDecisionIdx d) noexcept {
			return model_as<M>(m).M::decision_in_solution(Decision{d});
		},
		[](const FznsoModel* m, FznsoDecisionIdx d) noexcept {
			return model_as<M>(m).M::decision_annotation_count(Decision{d});
		},
		[](const FznsoModel* m, FznsoDecisionIdx d, std::size_t i) noexcept {
			return model_as<M>(m).M::decision_annotation(Decision{d}, i).raw();
		},
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::constraint_count(); },
		[](const FznsoModel* m, std::size_t l) noexcept { return model_as<M>(m).M::constraint_layer_end(l); },
		[](const FznsoModel* m, FznsoConstraintIdx c) noexcept {
			return from_view(model_as<M>(m).M::constraint_ident(Constraint{c}));
		},
		[](const FznsoModel* m, FznsoConstraintIdx c) noexcept {
			return model_as<M>(m).M::constraint_argument_count(Constraint{c});
		},
		[](const FznsoModel* m, FznsoConstraintIdx c, std::size_t i) noexcept {
			return model_as<M>(m).M::constraint_argument(Constraint{c}, i).raw();
		},
		[](const FznsoModel* m, FznsoConstraintIdx c) noexcept -> FznsoValueRef {
			std::optional<Decision> defines = model_as<M>(m).M::constraint_defines(Constraint{c});
			return defines.has_value() ? decision_value(*defines) : absent_value();
		},
		[](const FznsoModel* m, FznsoConstraintIdx c) noexcept {
			return model_as<M>(m).M::constraint_annotation_count(Constraint{c});
		},
		[](const FznsoModel* m, FznsoConstraintIdx c, std::size_t i) noexcept {
			return model_as<M>(m).M::constraint_annotation(Constraint{c}, i).raw();
		},
		[](const FznsoModel* m) noexcept { return from_view(model_as<M>(m).M::objective_ident()); },
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::objective_arg().raw(); },
		[](const FznsoModel* m) noexcept { return model_as<M>(m).M::objective_annotation_count(); },
		[](const FznsoModel* m, std::size_t i) noexcept {
			return model_as<M>(m).M::objective_annotation(i).raw();
		},
	};
	return table;
}

} // namespace detail

/// A solution reported by a solver.
class Solution {
public:
	explicit Solution(FznsoSolutionRef raw) : raw_(raw) {}

	/// The value assigned to a decision variable.
	Value operator[](Decision decision) const {
		return Value{raw_.methods->value(raw_.data, decision.index)};
	}
	/// A named statistic for this solution.
	Value statistic(std::string_view name) const {
		return Value{raw_.methods->statistic(raw_.data, str(name))};
	}

private:
	FznsoSolutionRef raw_;
};

/// How a run finished.
struct Status {
	enum class Kind {
		/// The solver explored the whole search space.
		Complete,
		/// The solver stopped early; better solutions may exist.
		Incomplete,
		/// The solver failed; `error` explains why.
		Error,
	};

	Kind kind = Kind::Complete;
	/// Set only when `kind` is `Error`.
	std::string error;

	bool complete() const { return kind == Kind::Complete; }
	bool failed() const { return kind == Kind::Error; }
};

class Library;

/// A solver instance created from a loaded [`Library`].
class DynSolver {
public:
	DynSolver(const DynSolver&) = delete;
	DynSolver& operator=(const DynSolver&) = delete;
	DynSolver(DynSolver&& other) noexcept : lib_(other.lib_), handle_(other.handle_) {
		other.handle_ = nullptr;
	}
	~DynSolver();

	/// The current value of a named option.
	Value option_get(std::string_view name) const;
	/// Set a named option. Returns the solver's message on failure.
	std::optional<std::string> option_set(std::string_view name, const Value& value);

	/// The current value of a named solver-level statistic, or absent if unknown.
	///
	/// Only statistics the library declares with the `solver` flag set are
	/// readable here; those carrying only the `solution` flag are read from a
	/// [`Solution`] instead.
	Value statistic(std::string_view name) const;

	/// Run the solver over `model`, reporting solutions and messages, and polling
	/// `should_stop` to decide whether to keep going.
	///
	/// `model` may be any [`Model`]; passing a concrete type lets `ref()` be
	/// resolved statically. The handlers are called directly, not through a
	/// `std::function`.
	///
	/// `should_stop` returns `true` to abandon the search, which yields
	/// `Status::Kind::Incomplete`. It is how a caller ends a run it has already
	/// started: return `true` from inside `on_solution` for "one solution is
	/// enough", or flip an atomic from another thread to cancel a long search.
	/// Unlike the handlers it may be polled **concurrently from several
	/// threads**, so it must be thread-safe — reading a `std::atomic<bool>`, the
	/// usual implementation, already is.
	template <class M, class OnSolution, class OnMessage, class ShouldStop>
	Status run(const M& model, OnSolution&& on_solution, OnMessage&& on_message,
	           ShouldStop&& should_stop);

	/// Run the solver to completion, reporting solutions and messages.
	///
	/// Passes no stop predicate, so the solver is told it will never be cancelled.
	template <class M, class OnSolution, class OnMessage>
	Status run(const M& model, OnSolution&& on_solution, OnMessage&& on_message) {
		return run(model, std::forward<OnSolution>(on_solution),
		           std::forward<OnMessage>(on_message), detail::Absent{});
	}

	/// Run the solver to completion, ignoring any messages.
	///
	/// Passes neither a message sink nor a stop predicate, so the solver knows to
	/// skip building diagnostics and to skip polling.
	template <class M, class OnSolution>
	Status run(const M& model, OnSolution&& on_solution) {
		return run(model, std::forward<OnSolution>(on_solution), detail::Absent{},
		           detail::Absent{});
	}

private:
	friend class Library;
	DynSolver(const Library* lib, FznsoSolver* handle) : lib_(lib), handle_(handle) {}

	const Library* lib_;
	FznsoSolver* handle_;
};

/// A dynamically loaded solver library.
///
/// The library owns every symbol a [`DynSolver`] created from it uses, so it must
/// outlive them.
class Library {
public:
	/// Thrown when a solver reports an ABI version this header cannot use. A
	/// subclass of `std::runtime_error`, so existing handlers still catch it.
	struct AbiMismatch : std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	/// The directories searched for solver libraries, in search order:
	/// `$FZNSO_SOLVER_PATH`, then the per-user directory, then the system ones.
	///
	/// Solvers live in their own `fznso` directory rather than on the normal
	/// library path: a shim is usually named after the solver it wraps, so
	/// `libgecode.so` would otherwise collide with real Gecode, and a dedicated
	/// directory is what makes [`discover`] possible at all.
	static std::vector<std::filesystem::path> search_paths() {
		std::vector<std::filesystem::path> dirs;
		std::string override_var = detail::env("FZNSO_SOLVER_PATH");
		if (!override_var.empty()) {
#if defined(_WIN32)
			char sep = ';';
#else
			char sep = ':';
#endif
			std::size_t start = 0;
			while (start <= override_var.size()) {
				std::size_t end = override_var.find(sep, start);
				if (end == std::string::npos) {
					end = override_var.size();
				}
				if (end > start) {
					dirs.emplace_back(override_var.substr(start, end - start));
				}
				start = end + 1;
			}
		}
#if defined(_WIN32)
		if (std::string local = detail::env("LOCALAPPDATA"); !local.empty()) {
			dirs.emplace_back(std::filesystem::path{local} / "fznso");
		}
#elif defined(__APPLE__)
		if (std::string home = detail::env("HOME"); !home.empty()) {
			dirs.emplace_back(std::filesystem::path{home} / "Library/Application Support/fznso");
		}
#else
		if (std::string xdg = detail::env("XDG_DATA_HOME"); !xdg.empty()) {
			dirs.emplace_back(std::filesystem::path{xdg} / "fznso");
		} else if (std::string home = detail::env("HOME"); !home.empty()) {
			dirs.emplace_back(std::filesystem::path{home} / ".local/share/fznso");
		}
#endif
#if defined(_WIN32)
		// Beside the executable is left to the caller on Windows; add nothing.
#else
		dirs.emplace_back("/usr/local/lib/fznso");
		dirs.emplace_back("/usr/lib/fznso");
#endif
		return dirs;
	}

	/// A solver library found on the search path, reported without loading it.
	struct Discovered {
		std::string name;
		std::string version; ///< Parsed from the file name; empty if none.
		std::filesystem::path path;
	};

	/// List the solver libraries on the search path, without loading any.
	///
	/// Directories are visited in [`search_paths`] order and, within a
	/// directory, the highest version of a solver comes first. Nothing is
	/// opened, so the ABI version is not checked here — [`find`] skips a
	/// candidate built for another ABI.
	static std::vector<Discovered> discover() {
		std::vector<Discovered> out;
		for (const std::filesystem::path& dir : search_paths()) {
			std::error_code ec;
			std::vector<std::pair<std::string, Discovered>> here;
			for (std::filesystem::directory_iterator it{dir, ec}, end; it != end && !ec;
			     it.increment(ec)) {
				std::string file = it->path().filename().string();
				if (file.find(detail::lib_marker()) == std::string::npos) {
					continue;
				}
				std::string_view name = detail::name_from_path(file.c_str());
				if (!detail::is_valid_name(name)) {
					continue;
				}
				here.push_back(
					{file, Discovered{std::string{name},
				                      detail::version_from_path(it->path().string().c_str()),
				                      it->path()}});
			}
			std::sort(here.begin(), here.end(), [](const auto& a, const auto& b) {
				if (a.second.name != b.second.name) {
					return detail::natural_less(a.second.name, b.second.name);
				}
				// highest version first
				return detail::natural_less(b.second.version, a.second.version);
			});
			for (auto& [file, d] : here) {
				out.push_back(std::move(d));
			}
		}
		return out;
	}

	/// Load a solver by name from the search path, choosing the newest version.
	///
	/// Candidates are tried in [`discover`] order: directory precedence first (a
	/// solver in `$FZNSO_SOLVER_PATH` or the per-user directory overrides a
	/// system one even if older), then newest version among equals. A candidate
	/// built for a different ABI is skipped and the search continues. Throws
	/// `std::runtime_error` if no matching solver loads.
	static std::unique_ptr<Library> find(std::string_view name) {
		return find_selected(name, std::string_view{}, false);
	}

	/// Load a solver by name and version from the search path.
	///
	/// `version` selects by whole dotted components, so `"6"` matches `6.2.1`;
	/// an empty string behaves like [`find`]. The newest match is chosen.
	static std::unique_ptr<Library> find_version(std::string_view name, std::string_view version) {
		return find_selected(name, version, true);
	}

	/// Dynamically load a solver library, resolving every entry point it must
	/// export.
	///
	/// The entry points are `fznso_<name>_...`, where `<name>` is recovered from
	/// the file name by stripping a leading `lib` and everything from the first
	/// `.`; this is the only supported naming, so the file name and the exported
	/// symbols must agree.
	///
	/// A solver's *name* has to satisfy both C and the platform's library naming
	/// at once, so:
	///
	/// 1. It must be a valid C identifier: ASCII letters, digits and
	///    underscores, not starting with a digit. Lowercase is the convention.
	///    No `-`, no `.`, nothing non-ASCII — those cannot appear in a symbol.
	/// 2. It must not begin with `lib`, which could not be told apart from the
	///    platform prefix: `libssat` reads back as `libssat` from
	///    `liblibssat.so` but as `ssat` from `libssat.dll`.
	/// 3. The file's base name must be `<name>`, optionally prefixed with `lib`,
	///    followed by any version and extension components. Everything from the
	///    first `.` is ignored, so `<name>` must not contain a `.`.
	///
	/// The version goes between the name and the extension on every platform
	/// (`libgecode.6.2.1.so`, `libgecode.6.2.1.dylib`, `gecode.6.2.1.dll`),
	/// rather than following each platform's native library convention, so one
	/// rule covers all three. Nothing links against a solver — it is opened by
	/// path from a directory off the linker search path — so the ELF
	/// `libfoo.so.MAJOR` soname scheme buys nothing here; the native form is
	/// still parsed, so a distribution-packaged `libgecode.so.6` also loads.
	///
	/// Several versions of one solver can sit side by side (`libgecode.6.so`,
	/// `libgecode.7.so`). They export the same symbols, but libraries are opened
	/// privately (`RTLD_LOCAL`, and Windows resolves per module), so several can
	/// be loaded at once; pass the exact path, or use `find_version`, to choose
	/// one.
	///
	/// Throws `std::runtime_error` if the file name yields an unusable name, or
	/// if the library cannot be opened or is missing a symbol.
	explicit Library(const char* path) {
		std::string_view name = detail::name_from_path(path);
		if (!detail::is_valid_name(name)) {
			throw std::runtime_error{std::string{"cannot derive a solver name from `"} + path +
			                         "`: `" + std::string{name} +
			                         "` is not a valid FZnSO solver name (expected "
			                         "[A-Za-z_][A-Za-z0-9_]*, so that it can spell the "
			                         "fznso_<name>_ entry points)"};
		}
		load(path, name);
	}

	Library(const Library&) = delete;
	Library& operator=(const Library&) = delete;
	~Library() {
		if (handle_ != nullptr) {
			detail::close_library(handle_);
		}
	}

	/// Create a solver instance.
	DynSolver create_solver() const { return DynSolver{this, solver_create_()}; }

	FznsoConstraintList constraint_types() const { return constraint_list_(); }
	FznsoTypeList decision_types() const { return decision_list_(); }
	FznsoObjectiveList objectives() const { return objective_list_(); }
	FznsoOptionList options() const { return option_list_(); }
	FznsoStatisticList statistics() const { return statistic_list_(); }

private:
	friend class DynSolver;

	using ConstraintListFn = FznsoConstraintList (*)();
	using TypeListFn = FznsoTypeList (*)();
	using ObjectiveListFn = FznsoObjectiveList (*)();
	using OptionListFn = FznsoOptionList (*)();
	using StatisticListFn = FznsoStatisticList (*)();
	using CreateFn = FznsoSolver* (*)();
	using FreeFn = void (*)(FznsoSolver*);
	using OptionGetFn = FznsoValueRef (*)(const FznsoSolver*, FznsoStr);
	using StatisticFn = FznsoValueRef (*)(const FznsoSolver*, FznsoStr);
	using OptionSetFn = bool (*)(FznsoSolver*, FznsoStr, FznsoValueRef);
	using ReadErrorFn = void (*)(FznsoSolver*, void*, void (*)(void*, FznsoStr));
	// `on_message` and `should_stop` may be null, which tells the solver nobody is
	// listening / nothing will cancel, so it can skip that work entirely.
	using RunFn = FznsoStatus (*)(FznsoSolver*, FznsoModelRef, void*,
	                              void (*)(void*, FznsoSolutionRef),
	                              void (*)(void*, FznsoStr, FznsoValueRef),
	                              bool (*)(void*));

	/// Open `path` and resolve every entry point, each named `fznso_<name>_...`.
	void load(const char* path, std::string_view name) {
		handle_ = detail::open_library(path);
		if (handle_ == nullptr) {
			throw std::runtime_error{std::string{"could not open "} + path + ": " +
			                         detail::last_error()};
		}
		try {
			std::string p{"fznso_"};
			p += name;
			// Reject an incompatible ABI before touching any other entry point.
			auto abi_version = symbol<std::uint32_t (*)()>((p + "_abi_version").c_str());
			if (std::uint32_t found = abi_version(); found != FZNSO_ABI_VERSION) {
				throw AbiMismatch{"solver ABI version " + std::to_string(found) +
				                  " is incompatible with this library's ABI version " +
				                  std::to_string(FZNSO_ABI_VERSION)};
			}
			constraint_list_ = symbol<ConstraintListFn>((p + "_constraint_list").c_str());
			decision_list_ = symbol<TypeListFn>((p + "_decision_list").c_str());
			objective_list_ = symbol<ObjectiveListFn>((p + "_objective_list").c_str());
			option_list_ = symbol<OptionListFn>((p + "_option_list").c_str());
			statistic_list_ = symbol<StatisticListFn>((p + "_statistic_list").c_str());
			solver_create_ = symbol<CreateFn>((p + "_solver_create").c_str());
			solver_free_ = symbol<FreeFn>((p + "_solver_free").c_str());
			solver_option_get_ = symbol<OptionGetFn>((p + "_solver_option_get").c_str());
			solver_statistic_ = symbol<StatisticFn>((p + "_solver_statistic").c_str());
			solver_option_set_ = symbol<OptionSetFn>((p + "_solver_option_set").c_str());
			solver_read_error_ = symbol<ReadErrorFn>((p + "_solver_read_error").c_str());
			solver_run_ = symbol<RunFn>((p + "_solver_run").c_str());
		} catch (...) {
			detail::close_library(handle_);
			throw;
		}
	}

	template <class Fn>
	Fn symbol(const char* name) const {
		void* found = detail::find_symbol(handle_, name);
		if (found == nullptr) {
			throw std::runtime_error{std::string{"missing symbol "} + name + ": " +
			                         detail::last_error()};
		}
		return reinterpret_cast<Fn>(found);
	}

	/// Shared implementation of `find` and `find_version`.
	static std::unique_ptr<Library> find_selected(std::string_view name, std::string_view version,
	                                               bool by_version) {
		std::string last_mismatch;
		for (const Discovered& candidate : discover()) {
			if (candidate.name != name) {
				continue;
			}
			if (by_version && !detail::version_matches(version, candidate.version)) {
				continue;
			}
			try {
				return std::unique_ptr<Library>{new Library{candidate.path.string().c_str()}};
			} catch (const AbiMismatch& e) {
				last_mismatch = e.what(); // another ABI generation may follow
			}
		}
		if (!last_mismatch.empty()) {
			throw std::runtime_error{last_mismatch};
		}
		throw std::runtime_error{"no solver named `" + std::string{name} +
		                         "` found on the FZnSO search path"};
	}

	/// Collect the solver's error message into a string.
	std::string read_error(FznsoSolver* solver) const {
		std::string message;
		solver_read_error_(solver, &message, [](void* ctx, FznsoStr chunk) {
			static_cast<std::string*>(ctx)->append(detail::to_view(chunk));
		});
		return message;
	}

	detail::LibraryHandle handle_ = nullptr;
	ConstraintListFn constraint_list_ = nullptr;
	TypeListFn decision_list_ = nullptr;
	ObjectiveListFn objective_list_ = nullptr;
	OptionListFn option_list_ = nullptr;
	StatisticListFn statistic_list_ = nullptr;
	CreateFn solver_create_ = nullptr;
	FreeFn solver_free_ = nullptr;
	OptionGetFn solver_option_get_ = nullptr;
	StatisticFn solver_statistic_ = nullptr;
	OptionSetFn solver_option_set_ = nullptr;
	ReadErrorFn solver_read_error_ = nullptr;
	RunFn solver_run_ = nullptr;
};

inline DynSolver::~DynSolver() {
	if (handle_ != nullptr) {
		lib_->solver_free_(handle_);
	}
}

inline Value DynSolver::statistic(std::string_view name) const {
	return Value{lib_->solver_statistic_(handle_, str(name))};
}

inline Value DynSolver::option_get(std::string_view name) const {
	return Value{lib_->solver_option_get_(handle_, str(name))};
}

inline std::optional<std::string> DynSolver::option_set(std::string_view name, const Value& value) {
	if (lib_->solver_option_set_(handle_, str(name), value.raw())) {
		return std::nullopt;
	}
	return lib_->read_error(handle_);
}

template <class M, class OnSolution, class OnMessage, class ShouldStop>
Status DynSolver::run(const M& model, OnSolution&& on_solution, OnMessage&& on_message,
                      ShouldStop&& should_stop) {
	static_assert(std::is_base_of<Model, M>::value, "model must derive from fznso::Model");

	// All three callbacks share the single `context` pointer the interface
	// provides, so they are held together. References suffice: run is
	// synchronous, so the handlers outlive it. Note `should_stop` is only ever
	// read through a const reference, since it may be polled concurrently.
	struct Callbacks {
		OnSolution& on_solution;
		OnMessage& on_message;
		const ShouldStop& should_stop;
	};
	Callbacks callbacks{on_solution, on_message, should_stop};

	// A thunk is handed over only when there is something behind it; otherwise the
	// solver receives null and can skip the work.
	void (*message_thunk)(void*, FznsoStr, FznsoValueRef) = nullptr;
	if constexpr (!std::is_same_v<std::decay_t<OnMessage>, detail::Absent>) {
		message_thunk = [](void* ctx, FznsoStr scope, FznsoValueRef value) {
			static_cast<Callbacks*>(ctx)->on_message(detail::to_view(scope), Value{value});
		};
	}
	bool (*stop_thunk)(void*) = nullptr;
	if constexpr (!std::is_same_v<std::decay_t<ShouldStop>, detail::Absent>) {
		stop_thunk = [](void* ctx) -> bool {
			return static_cast<const Callbacks*>(ctx)->should_stop();
		};
	}

	FznsoStatus status = lib_->solver_run_(
		handle_, model_ref(model), &callbacks,
		[](void* ctx, FznsoSolutionRef sol) {
			static_cast<Callbacks*>(ctx)->on_solution(Solution{sol});
		},
		message_thunk, stop_thunk);

	switch (status) {
	case FznsoComplete:
		return Status{Status::Kind::Complete, {}};
	case FznsoIncomplete:
		return Status{Status::Kind::Incomplete, {}};
	case FznsoError:
		return Status{Status::Kind::Error, lib_->read_error(handle_)};
	}
	return Status{Status::Kind::Error, "unknown status"};
}

} // namespace fznso

#endif // FZNSO_HPP

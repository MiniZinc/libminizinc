/***
  @groupdef stdlib.debug Assertions and debugging functions

  These functions help debug models and check that input data conforms to
  the expectations.
*/

/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function any $T: assert(bool: b, string: msg, any $T: x);

/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of any $T: assert(bool: b, string: msg, array[$U] of any $T: x);

/** @group stdlib.debug If \a b is true, return true, otherwise abort with message \a msg. */
function bool: assert(bool: b, string: msg);
/** @group stdlib.debug Return true, when the ``--debug`` flag is set, check if \a b is true, otherwise abort with message \a msg. */
function bool: assert_dbg(bool: b, string: msg);

/** @group stdlib.debug Return \a x, and print message \a msg. */
function any $T: trace(string: msg, any $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of any $T: trace(string: msg, array [$U] of any $T: x);
/** @group stdlib.debug Return \a x, and print a string representation of it. */
function any $T: trace_exp(any $T: x);
/** @group stdlib.debug Return \a x, and print a string representation of it. */
function array[$U] of any $T: trace_exp(array[$U] of any $T: x);

/** @group stdlib.debug Return true, and print message \a msg. */
function bool: trace(string: msg);
/** @group stdlib.debug Return true, and when ``--debug`` flag is set print message \a msg. */
function bool: trace_dbg(string: msg);

/** @group stdlib.debug Return \a x, and print message \a msg to section \a section. */
function any $T: trace_to_section(string: section, string: msg, any $T: x) = let {
  constraint mzn_trace_to_section(section, msg, false);
} in x;
/** @group stdlib.debug Return \a x, and print message \a msg to section \a section. */
function array [$U] of any $T: trace_to_section(string: section, string: msg, array [$U] of any $T: x) = let {
  constraint mzn_trace_to_section(section, msg, false);
} in x;

/** @group stdlib.debug Return true, and print message \a msg to section \a section. */
function bool: trace_to_section(string: section, string: msg) = mzn_trace_to_section(section, msg, false);

/** @group stdlib.debug Return true, and print \a x to the JSON section \a section. */
function bool: trace_to_json_section(string: section, opt $T: x) = mzn_trace_to_section(section, showJSON(x) ++ "\n", true);
/** @group stdlib.debug Return true, and print \a x to the JSON section \a section. */
function bool: trace_to_json_section(string: section, array [$U] of opt $T: x) = mzn_trace_to_section(section, showJSON(x) ++ "\n", true);

function bool: mzn_trace_to_section(string: section, string: x, bool: json) :: mzn_internal_representation;

/** @group stdlib.debug Return \a x, and print message \a msg. */
function any $T: trace_stdout(string: msg, any $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of any $T: trace_stdout(string: msg, array [$U] of any $T: x);
/** @group stdlib.debug Return true, and print message \a msg. */
function bool: trace_stdout(string: msg);

/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function any $T: trace_logstream(string: msg, any $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function array [$U] of any $T: trace_logstream(string: msg, array [$U] of any $T: x);
/** @group stdlib.debug Return true, and print message \a msg to logging stream. */
function bool: trace_logstream(string: msg);
/** @group stdlib.debug Return logging stream as string */
function string: logstream_to_string();

/** @group stdlib.debug With debug build of the MiniZinc compiler, call
   MiniZinc::mzn_break_here when flattening this expression to make
   debugging easier. This annotation is ignored by the release build. */
annotation mzn_break_here;

/** @group stdlib.debug Used to attach a ``expression_name`` annotation with name \a s to an expression when --debug is enabled. */
annotation expression_name_dbg(string: s) = expression_name(s);

/** @group stdlib.debug Abort evaluation and print message \a msg. */
function bool: abort(string: msg);

/** @group stdlib.debug Set to true iff ``--debug`` flag is set. */
bool: debug_mode = mzn_internal_check_debug_mode();

/** @group stdlib.debug Returns true iff ``--debug`` flag is set. */
test mzn_internal_check_debug_mode();

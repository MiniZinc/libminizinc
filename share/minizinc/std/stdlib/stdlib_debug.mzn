/***
  @groupdef stdlib.debug Assertions and debugging functions

  These functions help debug models and check that input data conforms to
  the expectations.
*/

/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function $T: assert(bool: b, string: msg, $T: x);
/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function opt $T: assert(bool: b, string: msg, opt $T: x);
/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function var $T: assert(bool: b, string: msg, var $T: x);
/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function var opt $T: assert(bool: b, string: msg, var opt $T: x);

/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of $T: assert(bool: b, string: msg, array[$U] of $T: x);
/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of opt $T: assert(bool: b, string: msg, array[$U] of opt $T: x);
/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of var $T: assert(bool: b, string: msg, array[$U] of var $T: x);
/** @group stdlib.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of var opt $T: assert(bool: b, string: msg, array[$U] of var opt $T: x);

/** @group stdlib.debug If \a b is true, return true, otherwise abort with message \a msg. */
function bool: assert(bool: b, string: msg);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function $T: trace(string: msg, $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function opt $T: trace(string: msg, opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function var $T: trace(string: msg, var $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function var opt $T: trace(string: msg, var opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of $T: trace(string: msg, array [$U] of $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of opt $T: trace(string: msg, array [$U] of opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of var $T: trace(string: msg, array [$U] of var $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of var opt $T: trace(string: msg, array [$U] of var opt $T: x);
/** @group stdlib.debug Return true, and print message \a msg. */
function bool: trace(string: msg);

/** @group stdlib.debug Return \a x, and print message \a msg. */
function $T: trace_stdout(string: msg, $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function opt $T: trace_stdout(string: msg, opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function var $T: trace_stdout(string: msg, var $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function var opt $T: trace_stdout(string: msg, var opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of $T: trace_stdout(string: msg, array [$U] of $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of opt $T: trace_stdout(string: msg, array [$U] of opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of var $T: trace_stdout(string: msg, array [$U] of var $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg. */
function array [$U] of var opt $T: trace_stdout(string: msg, array [$U] of var opt $T: x);
/** @group stdlib.debug Return true, and print message \a msg. */
function bool: trace_stdout(string: msg);

/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function $T: trace_logstream(string: msg, $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function opt $T: trace_logstream(string: msg, opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function var $T: trace_logstream(string: msg, var $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function var opt $T: trace_logstream(string: msg, var opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function array [$U] of $T: trace_logstream(string: msg, array [$U] of $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function array [$U] of opt $T: trace_logstream(string: msg, array [$U] of opt $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function array [$U] of var $T: trace_logstream(string: msg, array [$U] of var $T: x);
/** @group stdlib.debug Return \a x, and print message \a msg to logging stream. */
function array [$U] of var opt $T: trace_logstream(string: msg, array [$U] of var opt $T: x);
/** @group stdlib.debug Return true, and print message \a msg to logging stream. */
function bool: trace_logstream(string: msg);
/** @group stdlib.debug Return logging stream as string */
function string: logstream_to_string();

/** @group stdlib.debug With debug build of the MiniZinc compiler, call
   MiniZinc::mzn_break_here when flattening this expression to make
   debugging easier. This annotation is ignored by the release build. */
annotation mzn_break_here;

/** @group stdlib.debug Abort evaluation and print message \a msg. */
function bool: abort(string: msg);


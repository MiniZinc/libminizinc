import mzn from "tree-sitter-minizinc/grammar.js"

const MZN_RULES = mzn.grammar.rules

// DataZinc is a strict subset of MiniZinc, but instead extending the MiniZinc
// grammar and overriding what differs, we name the rules that carry over. This
// avoids all MiniZinc rules making it into `src/grammar.json`.
//
// Rules are emitted in MiniZinc's declaration order. Two tokens that match the
// same text are resolved by that order.
const INHERITED = new Set([
	"_identifier",
	"_string_content",
	"absent",
	"anonymous",
	"array_literal",
	"array_literal_2d",
	"array_literal_2d_row",
	"array_literal_3d",
	"array_literal_3d_row",
	"array_literal_3d_slice",
	"array_literal_member",
	"assignment",
	"block_comment",
	"boolean_literal",
	"doc_comment",
	"escape_sequence",
	"file_doc_comment",
	"identifier",
	"line_comment",
	"quoted_identifier",
	"record_literal",
	"record_member",
	"set_literal",
	"string_characters",
	"string_literal",
	"tuple_literal",
])

// Replacements for MiniZinc rules of the same name, kept in MiniZinc's position.
const OVERRIDDEN = {
	source_file: ($) => sepBy(";", field("item", $.assignment)),

	_expression: ($) =>
		choice(
			$._identifier,
			$.absent,
			$.anonymous,
			$.array_literal_2d,
			$.array_literal_3d,
			$.array_literal,
			$.boolean_literal,
			$.call,
			$.float_literal,
			$.infinity,
			$.infix_operator,
			$.integer_literal,
			$.record_literal,
			$.set_literal,
			$.string_literal,
			$.tuple_literal
		),

	call: ($) =>
		seq(
			field("function", choice($._identifier, $.anonymous)),
			"(",
			sepBy(",", field("argument", $._expression)),
			")"
		),

	infix_operator: ($) => {
		const table = [
			[prec.left, 10, ".."], // PREC.range
			[prec.left, 12, "++"], // PREC.additive
			[prec.left, 7, choice("union", "∪")], // PREC.union
		]

		return choice(
			...table.map(([assoc, precedence, operator]) =>
				assoc(
					precedence,
					seq(
						field("left", $._expression),
						field("operator", operator),
						field("right", $._expression)
					)
				)
			)
		)
	},

	float_literal: () => token(seq(optional("-"), MZN_RULES.float_literal)),
	integer_literal: () => token(seq(optional("-"), MZN_RULES.integer_literal)),
	infinity: () => token(seq(optional("-"), MZN_RULES.infinity)),
}

export default grammar({
	name: "datazinc",

	extras: ($) => [
		/\s/,
		$.line_comment,
		$.doc_comment,
		$.file_doc_comment,
		$.block_comment,
	],

	word: ($) => $.identifier,

	conflicts: ($) => [[$.array_literal_2d, $.array_literal_2d_row]],

	supertypes: ($) => [$._expression],

	rules: {
		...Object.fromEntries(
			Object.keys(MZN_RULES)
				.filter((n) => INHERITED.has(n) || n in OVERRIDDEN)
				.map((n) => [n, OVERRIDDEN[n] ?? (() => MZN_RULES[n])])
		),
	},
})

function sepBy(sep, rule) {
	return seq(repeat(seq(rule, sep)), optional(rule))
}

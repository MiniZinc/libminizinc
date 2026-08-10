/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

const PREC_ORDER = /** @type {const} */ ([
	"call",
	"annotation",
	"quoted_operator",
	"default",
	"concatenation",
	"negation",
	"exponent",
	"unary",
	"multiplicative",
	"additive",
	"range",
	"intersect",
	"union_or_diff",
	"membership",
	"comparative",
	"conjunction",
	"disjunction",
	"implication",
	"equivalence",
])

const PREC = /** @type {{ [K in typeof PREC_ORDER[number]]: number }} */ (
	PREC_ORDER.reduce(
		(res, name, idx, array) => ({ ...res, [name]: 10 * (array.length - idx) }),
		{}
	)
)

const primitive_types = ["ann", "bool", "float", "int", "string"]

const EQUIVALENCE_OPERATORS = ["<->", "⟷", "⇔"]
const IMPLICATION_OPERATORS = ["->", "→", "⇒", "<-", "←", "⇐"]
const DISJUNCTION_OPERATORS = ["\\/", "∨", "xor", "⊻"]
const CONJUNCTION_OPERATORS = ["/\\", "∧"]
// prettier-ignore
const COMPARISON_OPERATORS = [
	"=", "==", "!=", "≠", "<", "<=", "≤", ">", ">=", "≥",
	"~=", "~!=",
]
const MEMBERSHIP_OPERATORS = ["in", "∈", "subset", "⊆", "superset", "⊇"]
const UNION_DIFF_OPERATORS = ["union", "∪", "diff", "∖", "symdiff"]
const INTERSECTION_OPERATORS = ["intersect", "∩"]
const RANGE_OPERATORS = ["..", "<..", "..<", "<..<"]
const ADDITIVE_OPERATORS = ["+", "-", "~+", "~-"]
const MULTIPLICATIVE_OPERATORS = ["*", "/", "div", "mod", "~*", "~div", "~/"]
const NEGATION_OPERATORS = ["not", "¬"]
const INVERSE_OPERATORS = ["^-1", "⁻¹"]

export default grammar({
	name: "minizinc",

	extras: ($) => [
		/\s/,
		$.line_comment,
		$.doc_comment,
		$.file_doc_comment,
		$.block_comment,
	],

	word: ($) => $.identifier,

	conflicts: ($) => [
		[$._callable, $.assignment],
		[$._callable, $._pattern],
		[$._callable, $.pattern_call],
		[$._callable, $.record_member],
		[$._callable, $.inversed_identifier],
		[$._callable, $._literal],
		[$._callable, $._literal, $._pattern],
		[$._literal, $._pattern],
		[$._literal, $.pattern_numeric_literal],
		[$.array_literal_2d, $.array_literal_2d_row],
		[$._unannotated_expression, $.type_base],
		[$._expression, $.type_base],
	],

	supertypes: ($) => [$._expression, $._item, $._type],

	rules: {
		source_file: ($) => sepBy(";", field("item", $._item)),

		_item: ($) =>
			choice(
				$.annotation,
				$.assignment,
				$.constraint,
				$.declaration,
				$.enumeration,
				$.function_item,
				$.goal,
				$.include,
				$.output,
				$.predicate,
				$.type_alias,
				$.class_decl
			),

		annotation: ($) =>
			seq(
				"annotation",
				field("name", $._identifier),
				optional(field("parameters", $.annotation_parameters)),
				optional(seq("=", field("body", $._expression)))
			),

		annotation_parameters: ($) =>
			seq("(", sepBy1(",", field("parameter", $.parameter)), ")"),

		assignment: ($) =>
			seq(
				field("name", $._identifier),
				"=",
				field("definition", $._expression)
			),

		constraint: ($) =>
			seq(
				"constraint",
				optional(seq("::", field("annotation", $._adjacent_annotation))),
				field("expression", $._expression)
			),

		declaration: ($) =>
			seq(
				field("type", $._type),
				":",
				field("name", $._pattern),
				optional($._annotation_list),
				optional(seq("=", field("definition", $._expression)))
			),

		enumeration: ($) =>
			seq(
				"enum",
				field("name", $._identifier),
				optional($._annotation_list),
				optional(seq("=", sepBy1("++", field("case", $._enumeration_case))))
			),

		function_item: ($) =>
			seq(
				"function",
				field("type", $._type),
				":",
				field("name", choice($._identifier, $.inversed_identifier)),
				$._parameters,
				optional($._ann_parameter),
				optional($._annotation_list),
				optional(seq("=", field("body", $._expression)))
			),

		// `ann: name` after the parameter list captures the item's own annotations
		_ann_parameter: ($) =>
			seq("ann", ":", field("annotation_parameter", $._identifier)),

		goal: ($) =>
			seq(
				"solve",
				optional($._annotation_list),
				choice(
					field("strategy", "satisfy"),
					seq(
						field("strategy", choice("maximize", "minimize")),
						field("objective", $._expression)
					)
				)
			),

		include: ($) => seq("include", field("file", $.string_literal)),

		output: ($) =>
			seq(
				"output",
				optional(seq("::", field("annotation", $._adjacent_annotation))),
				field("expression", $._expression)
			),

		// An annotation written immediately before an expression. It must be a form
		// that cannot continue. Otherwise, `output :: "a" ["b"]` would parse as the
		// indexed access `"a"["b"]`.
		_adjacent_annotation: ($) =>
			choice(
				$.string_literal,
				$.string_interpolation,
				alias($._adjacent_annotation_call, $.call),
				$.parenthesised_expression
			),
		_adjacent_annotation_call: ($) =>
			seq(
				field("function", $._identifier),
				"(",
				sepBy(",", field("argument", $.arg_or_param)),
				")"
			),

		predicate: ($) =>
			seq(
				field("type", choice("predicate", "test")),
				field("name", choice($._identifier, $.inversed_identifier)),
				$._parameters,
				optional($._ann_parameter),
				optional($._annotation_list),
				optional(seq("=", field("body", $._expression)))
			),

		_annotation_list: ($) =>
			seq(
				repeat1(
					prec.left(
						PREC.annotation,
						seq("::", field("annotation", $._unannotated_expression))
					)
				)
			),

		_parameters: ($) =>
			seq("(", sepBy(",", field("parameter", $.parameter)), ")"),
		parameter: ($) =>
			seq(
				field("type", $._type),
				optional(
					seq(
						":",
						field("name", $._pattern),
						optional($._annotation_list),
						optional(seq("=", field("default", $._expression)))
					)
				)
			),

		_enumeration_case: ($) =>
			choice(
				$.enumeration_members,
				$.anonymous_enumeration,
				$.enumeration_constructor,
				$.array_literal
			),

		enumeration_members: ($) =>
			seq("{", sepBy(",", field("member", $._identifier)), "}"),
		anonymous_enumeration: ($) =>
			seq(
				field("name", $.anonymous),
				"(",
				sepBy1(",", field("parameter", $._type)),
				")"
			),
		enumeration_constructor: ($) =>
			seq(
				field("name", $._identifier),
				"(",
				sepBy1(",", field("parameter", $.parameter)),
				")"
			),

		type_alias: ($) =>
			seq(
				"type",
				field("name", $._identifier),
				optional($._annotation_list),
				"=",
				field("type", $._type)
			),

		class_decl: ($) =>
			seq(
				"class",
				field("name", $._identifier),
				optional(seq("extends", field("extends", $._identifier))),
				"(",
				sepBy(";", field("item", choice($.declaration, $.constraint))),
				")",
				optional($._annotation_list)
			),

		_expression: ($) =>
			choice($._unannotated_expression, $.annotated_expression),
		_unannotated_expression: ($) =>
			choice(
				$._non_concat_expression,
				alias($._concatenation, $.infix_operator)
			),
		_non_concat_expression: ($) =>
			choice(
				$._literal,

				$.array_comprehension,
				$.if_then_else,
				$.infix_operator,
				$.case_expression,
				$.lambda,
				$.let_expression,
				$.prefix_operator,
				$.postfix_operator,
				$.set_comprehension,
				$.string_interpolation,
				$._callable
			),
		// A `++` expression standing in for a type, so that a call argument keeps
		// the same shape as any other expression argument.
		_concatenated_domain: ($) =>
			field("domain", alias($._concatenation, $.infix_operator)),
		_concatenation: ($) =>
			prec.left(
				PREC.concatenation,
				seq(
					field("left", $._expression),
					field("operator", "++"),
					field("right", $._expression)
				)
			),
		_callable: ($) =>
			choice(
				$.anonymous,
				$.inversed_identifier,
				$._identifier,
				$.call,
				$.generator_call,
				$.indexed_access,
				$.tuple_access,
				$.record_access,
				$.parenthesised_expression
			),

		parenthesised_expression: ($) =>
			seq("(", field("expression", $._expression), ")"),

		array_comprehension: ($) =>
			seq(
				"[",
				optional(seq(field("index", $._expression), ":")),
				field("template", $._expression),
				"|",
				sepBy1(",", field("generator", $._generator)),
				"]"
			),

		call: ($) =>
			prec(
				PREC.call,
				seq(
					field("function", $._callable),
					"(",
					sepBy(",", field("argument", $.arg_or_param)),
					")"
				)
			),

		arg_or_param: ($) =>
			seq(
				field(
					"type",
					choice($._non_concat_type, alias($._concatenated_domain, $.type_base))
				),
				optional(
					seq(
						":",
						field("expression", $._expression),
						optional(seq("=", field("default", $._expression)))
					)
				)
			),

		generator_call: ($) =>
			prec.dynamic(
				PREC.call + 1,
				seq(
					field("function", $._callable),
					"(",
					sepBy1(",", field("generator", $._generator)),
					")",
					"(",
					field("template", $._expression),
					")"
				)
			),

		_generator: ($) => choice($.generator, $.assignment_generator),
		generator: ($) =>
			seq(
				sepBy1(",", field("name", $._pattern)),
				"in",
				field("collection", $._expression),
				optional(seq("where", field("where", $._expression)))
			),
		assignment_generator: ($) =>
			seq(
				field("name", $._pattern),
				"=",
				field("value", $._expression),
				optional(seq("where", field("where", $._expression)))
			),

		if_then_else: ($) =>
			seq(
				"if",
				field("condition", $._expression),
				"then",
				field("result", $._expression),
				repeat(
					seq(
						"elseif",
						field("condition", $._expression),
						"then",
						field("result", $._expression)
					)
				),
				optional(seq("else", field("else", $._expression))),
				"endif"
			),

		indexed_access: ($) =>
			prec(
				PREC.call,
				seq(
					field("collection", $._unannotated_expression),
					"[",
					sepBy1(
						",",
						field("index", choice(...RANGE_OPERATORS, $._expression))
					),
					"]"
				)
			),

		tuple_access: ($) =>
			prec(
				PREC.call,
				seq(
					field("tuple", $._unannotated_expression),
					".",
					field("field", alias(/[0-9]+/, $.integer_literal))
				)
			),

		record_access: ($) =>
			prec(
				PREC.call,
				seq(
					field("record", $._unannotated_expression),
					".",
					field("field", $._identifier)
				)
			),

		infix_operator: ($) => {
			const table = /** @type {const} */ ([
				[prec.left, PREC.equivalence, choice(...EQUIVALENCE_OPERATORS)],
				[prec.left, PREC.implication, choice(...IMPLICATION_OPERATORS)],
				[prec.left, PREC.disjunction, choice(...DISJUNCTION_OPERATORS)],
				[prec.left, PREC.conjunction, choice(...CONJUNCTION_OPERATORS)],
				[nonAssoc, PREC.comparative, choice(...COMPARISON_OPERATORS)],
				[nonAssoc, PREC.membership, choice(...MEMBERSHIP_OPERATORS)],
				[prec.left, PREC.union_or_diff, choice(...UNION_DIFF_OPERATORS)],
				[prec.left, PREC.intersect, choice(...INTERSECTION_OPERATORS)],
				[nonAssoc, PREC.range, choice(...RANGE_OPERATORS)],
				[prec.left, PREC.additive, choice(...ADDITIVE_OPERATORS)],
				[prec.left, PREC.multiplicative, choice(...MULTIPLICATIVE_OPERATORS)],
				[prec.left, PREC.exponent, "^"],
				[prec.left, PREC.default, "default"],
				[prec.left, PREC.quoted_operator, $.backtick_identifier],
			])

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

		annotated_expression: ($) =>
			prec(
				PREC.annotation,
				seq(
					field("expression", $._unannotated_expression),
					repeat1(
						prec.left(
							PREC.annotation,
							seq("::", field("annotation", $._unannotated_expression))
						)
					)
				)
			),

		case_expression: ($) =>
			seq(
				"case",
				field("expression", $._expression),
				"of",
				sepBy1(",", field("case", $.case_expression_case)),
				"endcase"
			),
		case_expression_case: ($) =>
			seq(field("pattern", $._pattern), "=>", field("value", $._expression)),

		lambda: ($) =>
			seq(
				"lambda",
				optional(seq(field("return_type", $._type), ":")),
				$._parameters,
				"=>",
				field("body", $._expression)
			),

		let_expression: ($) =>
			seq(
				"let",
				"{",
				field(
					"let",
					sepBy(
						choice(",", ";"),
						field("item", choice($.declaration, $.constraint))
					)
				),
				"}",
				"in",
				field("in", $._expression)
			),

		prefix_operator: ($) =>
			choice(
				prec(
					PREC.negation,
					seq(
						field("operator", choice(...NEGATION_OPERATORS)),
						field("operand", $._expression)
					)
				),
				prec(
					PREC.unary,
					seq(
						field("operator", choice("-", "+")),
						field("operand", $._expression)
					)
				),
				// TODO: Could be nonassoc, will always give type error
				prec.left(
					PREC.range,
					seq(
						field("operator", choice(...RANGE_OPERATORS)),
						field("operand", $._expression)
					)
				)
			),

		postfix_operator: ($) =>
			choice(
				// TODO: Could be nonassoc, will always give type error
				prec.right(
					PREC.range,
					seq(
						field("operand", $._expression),
						field("operator", choice(...RANGE_OPERATORS))
					)
				),
				prec(
					PREC.exponent,
					seq(
						field("operand", $._expression),
						field("operator", choice(...INVERSE_OPERATORS))
					)
				)
			),

		set_comprehension: ($) =>
			seq(
				"{",
				field("template", $._expression),
				"|",
				sepBy1(",", field("generator", $._generator)),
				"}"
			),

		// TODO: Decide if string_literal and string_interpolation should be combined
		string_interpolation: ($) =>
			seq(
				'"',
				optional(field("item", alias($._string_content, "string"))),
				repeat1(
					seq(
						"\\(",
						field("item", alias($._expression, "expression")),
						")",
						optional(field("item", alias($._string_content, "string")))
					)
				),
				'"'
			),

		_type: ($) => choice($._non_concat_type, $.type_concatenation),
		// Every type except a concatenation. Call arguments use this: `a ++ b`
		// there is an expression, never a type concatenation.
		_non_concat_type: ($) =>
			choice(
				$.array_type,
				$.set_type,
				$.tuple_type,
				$.record_type,
				$.operation_type,
				$.type_base,
				$.any_type,
				$.list_type
			),
		type_concatenation: ($) =>
			prec.left(
				PREC.concatenation,
				seq(field("left", $._type), "++", field("right", $._type))
			),
		array_type: ($) =>
			prec(
				PREC.concatenation + 1,
				seq(
					"array",
					"[",
					sepBy1(",", field("dimension", $.array_dimension)),
					"]",
					"of",
					field("type", $._type)
				)
			),
		array_dimension: ($) =>
			prec(
				PREC.call,
				seq(
					optional(seq(field("name", $._identifier), "in")),
					field("type", $._type)
				)
			),
		// Binds tighter than `++`, so `set of A ++ B` is `(set of A) ++ B`
		set_type: ($) =>
			prec(
				PREC.concatenation + 1,
				seq(
					optional(field("var_par", choice("var", "par"))),
					optional(field("opt", "opt")),
					"set",
					optional(seq("(", field("cardinality", $._expression), ")")),
					"of",
					field("type", $._type)
				)
			),
		tuple_type: ($) =>
			seq(
				optional(field("var_par", choice("var", "par"))),
				optional(field("opt", "opt")),
				"tuple",
				"(",
				sepBy1(",", field("field", $._type)),
				")"
			),
		record_type: ($) =>
			seq(
				optional(field("var_par", choice("var", "par"))),
				optional(field("opt", "opt")),
				"record",
				"(",
				sepBy1(",", field("field", $.record_type_field)),
				")"
			),
		record_type_field: ($) =>
			seq(field("type", $._type), ":", field("name", $._identifier)),
		operation_type: ($) =>
			seq(
				"op",
				"(",
				field("return_type", $._type),
				":",
				"(",
				sepBy(",", field("parameter", $._type)),
				")",
				")"
			),
		type_base: ($) =>
			choice(
				seq(
					optional(field("var_par", choice("var", "par"))),
					optional(field("opt", "opt")),
					field(
						"domain",
						choice(
							$.primitive_type,
							$.new_type,
							$.type_inst_id,
							$.type_inst_enum_id,
							$._non_concat_expression,
							$.annotated_expression
						)
					)
				),
				seq(field("any", "any"), field("domain", $.type_inst_id))
			),
		primitive_type: ($) => choice(...primitive_types),
		type_inst_id: ($) => /\$[A-Za-z][A-Za-z0-9_]*/,
		type_inst_enum_id: ($) => /\$\$[A-Za-z][A-Za-z0-9_]*/,
		new_type: ($) => seq("new", field("type", $._identifier)),
		any_type: ($) => "any",
		list_type: ($) =>
			prec(PREC.concatenation + 1, seq("list", "of", field("type", $._type))),

		_literal: ($) =>
			choice(
				$.absent,
				$.anonymous,
				$.array_literal_2d,
				$.array_literal_3d,
				$.array_literal,
				$.boolean_literal,
				$.float_literal,
				$.infinity,
				$.integer_literal,
				$.set_literal,
				$.string_literal,
				$.tuple_literal,
				$.record_literal
			),

		absent: ($) => "<>",
		anonymous: ($) => "_",
		// Only an indexed member needs the wrapper; a plain one stands for itself,
		// as in `array_literal_2d_row`. Wrapping every member costs a subtree
		// each, which is a quarter of the tree for a data file that is one long
		// array.
		array_literal: ($) =>
			seq(
				"[",
				sepBy(
					",",
					field("member", choice($._expression, $.array_literal_member))
				),
				"]"
			),
		array_literal_member: ($) =>
			seq(field("index", $._expression), ":", field("value", $._expression)),
		array_literal_2d: ($) =>
			seq(
				"[|",
				optional(
					seq(
						choice(
							repeat1(seq(field("column_index", $._expression), ":")),
							field("row", $.array_literal_2d_row)
						),
						repeat(seq("|", field("row", $.array_literal_2d_row))),
						optional("|")
					)
				),
				"|]"
			),
		array_literal_2d_row: ($) =>
			seq(
				optional(seq(field("index", $._expression), ":")),
				sepBy1(",", field("member", $._expression))
			),
		array_literal_3d: ($) =>
			seq(
				"[|",
				choice(
					seq("|", "|"),
					sepBy1(",", field("slice", $.array_literal_3d_slice))
				),
				"|]"
			),
		array_literal_3d_slice: ($) =>
			seq("|", sepBy1("|", field("row", $.array_literal_3d_row)), "|"),
		array_literal_3d_row: ($) => sepBy1(",", field("member", $._expression)),
		boolean_literal: ($) => choice("true", "false"),
		float_literal: ($) =>
			token(
				choice(
					/\d+\.\d+/,
					/\d+(\.\d+)?[Ee][+-]?\d+/,
					/0[xX]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.?)[pP][+-]?[0-9]+/
				)
			),
		integer_literal: ($) =>
			token(choice(/[0-9]+/, /0[xX][0-9a-fA-F]+/, /0b[01]+/, /0o[0-7]+/)),
		infinity: ($) => choice("infinity", "∞"),
		set_literal: ($) =>
			choice("∅", seq("{", sepBy(",", field("member", $._expression)), "}")),

		string_literal: ($) => seq('"', optional($._string_content), '"'),
		_string_content: ($) =>
			repeat1(field("content", choice($.string_characters, $.escape_sequence))),
		string_characters: ($) => token.immediate(prec(1, /[^"\n\\]+/)),
		escape_sequence: ($) => {
			const simpleEscape = [
				["\\'", "'"],
				['\\"', '"'],
				["\\\\", "\\"],
				["\\r", "\r"],
				["\\n", "\n"],
				["\\t", "\t"],
			]
			return choice(
				field("escape", choice(...simpleEscape.map(([e, v]) => alias(e, v)))),
				seq("\\", field("escape", alias(/[0-7]{1,3}/, "octal"))),
				seq("\\x", field("escape", alias(/[0-9a-fA-F]{1,2}/, "hexadecimal"))),
				seq("\\u", field("escape", alias(/[0-9a-fA-F]{4}/, "hexadecimal"))),
				seq("\\U", field("escape", alias(/[0-9a-fA-F]{8}/, "hexadecimal")))
			)
		},

		tuple_literal: ($) =>
			seq(
				"(",
				field("member", $._expression),
				",",
				sepBy(",", field("member", $._expression)),
				")"
			),
		record_literal: ($) =>
			seq("(", sepBy(",", field("member", $.record_member)), ")"),
		record_member: ($) =>
			seq(field("name", $._identifier), ":", field("value", $._expression)),

		identifier: ($) => /[\p{L}_][\p{L}\p{N}_]*/u,
		quoted_identifier: ($) => /'[^']*'/,
		inversed_identifier: ($) =>
			seq(field("identifier", $._identifier), choice(...INVERSE_OPERATORS)),
		backtick_identifier: ($) => /`[^`\n]*`/,
		_identifier: ($) => choice($.identifier, $.quoted_identifier),

		_pattern: ($) =>
			choice(
				$._identifier,
				$.absent,
				$.anonymous,
				$.boolean_literal,
				$.string_literal,
				$.pattern_numeric_literal,
				$.pattern_call,
				$.pattern_tuple,
				$.pattern_record
			),
		pattern_numeric_literal: ($) =>
			seq(
				optional(field("negative", "-")),
				field("value", choice($.integer_literal, $.float_literal, $.infinity))
			),
		pattern_call: ($) =>
			seq(
				field("identifier", $._identifier),
				"(",
				sepBy(",", field("argument", $._pattern)),
				")"
			),
		pattern_tuple: ($) =>
			seq(
				"(",
				field("field", $._pattern),
				",",
				sepBy(",", field("field", $._pattern)),
				")"
			),
		pattern_record: ($) =>
			seq("(", sepBy1(",", field("field", $.pattern_record_field)), ")"),
		pattern_record_field: ($) =>
			seq(field("name", $._identifier), ":", field("value", $._pattern)),

		line_comment: ($) => token(seq("%", /.*/)),
		doc_comment: ($) => token(prec(1, /\/\*\*([^*]|\*+[^*\/])*\*+\//)),
		file_doc_comment: ($) => token(prec(2, /\/\*\*\*([^*]|\*+[^*\/])*\*+\//)),
		block_comment: ($) => token(/\/\*([^*]|\*+[^*\/])*\*+\//),
	},
})

function sepBy(sep, rule) {
	return seq(repeat(seq(rule, sep)), optional(rule))
}

function sepBy1(sep, rule) {
	return seq(rule, repeat(seq(sep, rule)), optional(sep))
}

function nonAssoc(p, rule) {
	// Bump precedence by 5 to indicate this is non associative
	return prec.left(p + 5, rule)
}

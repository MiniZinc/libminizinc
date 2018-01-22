##@mainpage 	MiniZinc Python Interface
# @author		Tai Tran
# @supervisor	Guido Tack


#@Things to add:
# Improve the interface so that there is no need to assign MZN_STDLIB_DIR when starting python
# args_ret_dict: some functions type checking are not written
# Hide some internal functions such as flatten or type_presentation

#@Suggestion:
# Remove the user-defined name when declaring variable
# Consider replacing it with i, where i is the number of arguments created
#	for example: calling [a,b,c] = m.Variable(1,100,3)

'''
@Usage:
definition:
	lb: lower bound,
	ub: upper bound,
	set:minizinc.Model.Set)


* Declaration *

minizinc.Model:
	accepts 1 optional argument:
		a string or list of strings indicating libraries to be included.
	returns:
		model to be used, for example:
			m = minizinc.Model
			i = m.Variable
			s = m.Set
			m.Constraint
			m.satisfy
			etc..
minizinc.Model.Set:
	accepts multiple arguments:
		item1, item2, ... itemn
		item?: a number - single element
			or a list/tuple of 2 elements - elements ranging from lb to ub
	returns:
		an object to be used
	functions:
		push - push more elements onto the set
		clear - clear all elements in the set
minizinc.Model.Variable:
	accepts up to 2 arguments:
		<None>			: a boolean variable
		ub				: a variable ranging from 0 to ub - 1
		lb, ub			: a variable ranging from lb to ub
		set				: a variable based on the set
minizinc.Model.Array:
minizinc.Model.Construct:




* Functions *
minizinc.Model.satisfy
minizinc.Model.next
'''

import sys
import minizinc_internal
import predicate
import annotation
#import inspect

if sys.version < '3':
	integer_types = (int, long, )
	python_types = (int, long, float, bool, str)
	int_t = long
	def longify(x):
		return long(x)
else:
	integer_types = (int, )
	python_types = (int, float, bool, str)
	int_t = int
	def longify(x):
		return x

# turns a complex structure of items into a list of items
def flatten(x):
	result = []
	for el in x:
		if isinstance(el, (list, tuple)):
			result.extend(flatten(el))
		else:
			result.append(el)
	return result

# nicely presents the type of a variable
def type_presentation(x):
	if x is None:
		return "Any_Type"
	if type(x) is type:
		name = x.__name__
		if name[:9] == 'minizinc.':
			name = name[9:]
		return name
	else:
		if type(x) is tuple:
			ret = '('
			for i, t in enumerate(x):
				if i != 0:
					ret += ', '
				ret += type_presentation(t)
			ret += ')'
		elif type(x) is list:
			ret = 'array' + str(len(x)) + 'd of ' + type_presentation(x[0])
		else:
			raise TypeError('Type Presentation: Unexpected type: ' + type(x))
		return ret



# All MiniZinc objects derived from here
class Expression(object):
	def __init__(self, model = None):
		self.model = model

	def __str__(self):
		return str(self.get_value())

	def is_decl(self):
		return isinstance(self, Declaration)

	def is_pre(self):
		return isinstance(self, Predicate)

	def evaluate(self, var):
		if isinstance(var, Expression):
			ret = var.get_value()
			if ret == None:
				raise ValueError("'" + var.name + "'Variable value is not set")
			return ret
		else:
			return var

	def eval_type(self, args):
		if isinstance(args, Expression):
			return args.type
		elif type(args) in integer_types:
			return int_t
		elif type(args) in (float, bool, str):
			return type(args)
		elif type(args) is list:
			t = None
			for i in args:
				if t is None:
					t = self.eval_type(i)
				else:
					type_i = self.eval_type(i)
					if t != type_i:
						raise TypeError("Type of arguments in an array must be the same: expected: " +
							type_presentation((t,)) + ", received: " + type_presentation((type_i,)) )
			return [t]
		elif type(args) is tuple:
			t = []
			for i in args:
				t.append(self.eval_type(i))
			return tuple(t)
		else:
			raise TypeError("Unexpected Type: " + type_presentation(type(args)) )

	def __and__(self, pred):
		return And( self, pred )

	def __rand__(self, pred):
		return And( self, pred )

	def __or__(self, pred):
		return Or( self, pred )

	def __xor__(self, pred):
		return Xor( self, pred )

	def __ror__(self, pred):
		return Or( self, pred )

	def __add__(self, pred):
		return Add( self, pred )

	def __radd__(self, pred):
		return Add( pred, self )

	def __sub__(self, pred):
		return Sub( self, pred )

	def __rsub__(self, pred):
		return Sub( pred, self )

	def __div__(self, pred):
		return Div( self, pred )

	def __rdiv__(self, pred):
		return Div( pred, self )

	def __floordiv__(self, pred):
		return FloorDiv( self, pred )

	def __rfloordiv__(self, pred):
		return FloorDiv( pred, self )

	def __mul__(self, pred):
		return Mul( self, pred )

	def __rmul__(self, pred):
		return Mul( self, pred )

	def __mod__(self, pred):
		return Mod( self, pred )

	def __rmod__(self, pred):
		return Mod( pred, self )

	def __pow__(self, pred):
		return Pow( self, pred )

	def __rpow__(self, pred):
		return Pow( pred, self )

	def __eq__(self, pred):
		return Eq( self, pred )

	def __ne__(self, pred):
		return Ne( self, pred )	

	def __lt__(self, pred):
		return Lt( self, pred )

	def __gt__(self, pred):
		return Gt( self, pred )

	def __le__(self, pred):
		return Le( self, pred )

	def __ge__(self, pred):
		return Ge( self, pred )

	def __neg__(self):
		return Neg( self )

	def __pos__(self):
		return Pos( self )

	def __invert__(self):
		return Invert( self )

	def __abs__(self):
		return Abs( self )


# Temporary container for Expression before evaluating to minizinc_internal object
# Calling Predicate.init automatically check:
#		if the arguments belong to the same model
class Predicate(Expression):
	def __init__(self, vars, args_and_return_type_tuple = None, name = None, model = None):
		self.vars = vars
		self.name = str(name)

		def eval_model(args):
			model = None
			for val in args:
				if isinstance(val, Expression):
					if hasattr(val, 'model'):
						if val.model is not None:
							if model is None:
								model = val.model
							elif model != val.model:
								raise TypeError("Arguments in the Predicate don't belong to the same model")
			return model

		self.vars_type = self.eval_type(vars)
		self.model = eval_model(vars)
		if args_and_return_type_tuple is not None:
			for t in args_and_return_type_tuple:
				def check_type(expected, actual):
					if expected is None:
						return True
					if len(expected) != len(actual):
						return False
					for i, val in enumerate(expected):
						if (val is None):
							pass
						elif type(val) is list and type(actual[i]) is list:
							if check_type(val, actual[i]) == False:
								return False
						elif type(val) != type(actual[i]):
							return False
					return True
				if check_type(t[0], self.vars_type):
				#self.vars_type == t[0]:
					self.type = t[1]
					break
			else:
				s = "MiniZinc: function '" + self.name + "': Argument type does not match, expected: " 
				for i, t in enumerate(args_and_return_type_tuple):
					if i != 0:
						s += ' or '
					s += type_presentation(t[0])
				s += ', received: ' + type_presentation(self.vars_type)
				raise TypeError(s)
		else:
			self.type = None
		Expression.__init__(self)


class BinOp(Predicate):
	def __init__(self, vars, code, args_and_return_type_tuple, name):
		Predicate.__init__(self,vars,args_and_return_type_tuple, name)
		self.BinOpCode = code

class UnOp(Predicate):
	def __init__(self, vars, code, args_and_return_type_tuple, name):
		Predicate.__init__(self,vars, args_and_return_type_tuple, name)
		self.UnOpCode = code

class Call(Predicate):
	def __init__(self, vars, code, args_and_return_type_tuple = None, name = None, model_list = None):
		Predicate.__init__(self,vars, args_and_return_type_tuple, name)
		self.CallCode = code
		self.model_list = model_list



args_ret_dict = {}

to_assign = [ ((int_t, int_t), int_t ),
			  ((float, float), float ),
			  ((int_t, float), float ),
			  ((float, int_t), float)]
args_ret_dict['add'] = to_assign
args_ret_dict['sub'] = to_assign
args_ret_dict['mul'] = to_assign
args_ret_dict['div'] = to_assign
args_ret_dict['floordiv'] = to_assign
args_ret_dict['pow'] = to_assign


args_ret_dict['mod'] =[ ((int_t, int_t), int_t )]

to_assign = [ ((int_t, int_t), bool ), 
			  ((float, float), bool ),
			  ((bool, bool), bool ),
			  ((str, str), bool ),
			  ((minizinc_internal.VarSet, minizinc_internal.VarSet), bool)]
args_ret_dict['lt'] = to_assign
args_ret_dict['le'] = to_assign
args_ret_dict['gt'] = to_assign
args_ret_dict['ge'] = to_assign
args_ret_dict['eq'] = to_assign
args_ret_dict['ne'] = to_assign



args_ret_dict['in'] = [ ((int_t, minizinc_internal.VarSet), bool) ]


to_assign = [ ((bool, bool), bool) ]
args_ret_dict['or']  = to_assign
args_ret_dict['and'] = to_assign
args_ret_dict['xor'] = to_assign

args_ret_dict['abort'] = [ ((), bool)]
to_assign = [ ((int_t,), int_t),
			  ((float,), float) ]
args_ret_dict['abs'] = to_assign
args_ret_dict['neg'] = to_assign
args_ret_dict['pos'] = to_assign
args_ret_dict['invert'] = to_assign

args_ret_dict['array1d'] = [ (None, [int_t]) ] #the function has checked its argument already
args_ret_dict['array2d'] = [ (None, [int_t, int_t]) ]
args_ret_dict['array3d'] = [ (None, [int_t, int_t, int_t]) ]
args_ret_dict['array4d'] = [ (None, [int_t, int_t, int_t, int_t])]
args_ret_dict['array5d'] = [ (None, [int_t, int_t, int_t, int_t, int_t])]
args_ret_dict['array6d'] = [ (None, [int_t, int_t, int_t, int_t, int_t, int_t])]

to_assign = [ ((float,), float) ]
args_ret_dict['sin']	= to_assign
args_ret_dict['cos']	= to_assign
args_ret_dict['tan']	= to_assign
args_ret_dict['asin']	= to_assign
args_ret_dict['acos']	= to_assign
args_ret_dict['atan']	= to_assign
args_ret_dict['sinh']	= to_assign
args_ret_dict['cosh']	= to_assign
args_ret_dict['tanh']	= to_assign
args_ret_dict['asinh']	= to_assign
args_ret_dict['acosh']	= to_assign
args_ret_dict['atanh']	= to_assign
args_ret_dict['sqrt']	= to_assign
args_ret_dict['ln']		= to_assign
args_ret_dict['log2']	= to_assign
args_ret_dict['log10']	= to_assign
args_ret_dict['exp']	= to_assign

args_ret_dict['log'] = [ ((float, float), float) ]


args_ret_dict['assert'] = [ ((bool, str), bool) ]
args_ret_dict['bool2int'] = [ ((bool,), int_t) ]
# XXX what to do with cardinality?
args_ret_dict['card'] = None

to_assign = [ ((float,), int_t) ]
args_ret_dict['ceil'] = to_assign
args_ret_dict['floor'] = to_assign
args_ret_dict['round'] = to_assign

to_assign = [ ((int_t,), float) ]
args_ret_dict['int2float'] = to_assign
# XXX consider remove
args_ret_dict['concat'] = None


to_assign = [ ((int_t, int_t), int_t),
			  ((float, float), float),
			  (([int_t],), int_t),
			  (([float],), float) ]
args_ret_dict['min'] = to_assign
args_ret_dict['max'] = to_assign

to_assign = [ (([int_t], ), int_t),
			  (([float], ), float) ]
args_ret_dict['product'] = to_assign
args_ret_dict['sum'] = to_assign


del to_assign



class Add(BinOp):
	def __init__(self, *vars):
		BinOp.__init__(self, vars, 0, args_ret_dict['add'], '+')

	def get_symbol(self):
		return '+'

	def get_value(self):
		lhs = self.evaluate(self.vars[0])
		rhs = self.evaluate(self.vars[1])
		if isinstance(rhs, str):
			lhs = str(lhs)
		elif isinstance(lhs, str):
			rhs = str(rhs)
		return lhs + rhs

class Sub(BinOp):
	def __init__(self, *vars):
		BinOp.__init__(self, vars, 1, args_ret_dict['sub'], '-')

	def get_symbol(self):
		return '-'

	def get_value(self):
		return self.evaluate(self.vars[0]) - self.evaluate(self.vars[1])

class Mul(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 2, args_ret_dict['mul'], '*')

	def get_symbol(self):
		return '*'

	def get_value(self):
		return self.evaluate(self.vars[0]) * self.evaluate(self.vars[1])

class Div(BinOp):

	def __init__(self, *vars) :
		BinOp.__init__(self, vars, 3, args_ret_dict['div'], '/')

	def get_symbol(self):
		return '/'

	def get_value(self):
		return self.evaluate(self.vars[0]) / self.evaluate(self.vars[1])

class FloorDiv(BinOp):

	def __init__(self, *vars) :
		BinOp.__init__(self, vars, 4, args_ret_dict['floordiv'], '//')

	def get_symbol(self):
		return '//'

	def get_value(self):
		return self.evaluate(self.vars[0]) // self.evaluate(self.vars[1])


class Mod(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 5, args_ret_dict['mod'], '%')

	def get_symbol(self):
		return '%'

	def get_value(self):
		return self.evaluate(self.vars[0]) % self.evaluate(self.vars[1])


class Lt(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 6, args_ret_dict['lt'], '<')

	def get_symbol(self):
		return '<'

	def get_value(self):
		return self.evaluate(self.vars[0]) < self.evaluate(self.vars[1])

class Le(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 7, args_ret_dict['le'], '<=')

	def get_symbol(self):
		return '<='

	def get_value(self):
		return self.evaluate(self.vars[0]) <= self.evaluate(self.vars[1])

class Gt(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 8, args_ret_dict['gt'], '>')

	def get_symbol(self):
		return '>'

	def get_value(self):
		return self.evaluate(self.vars[0]) > self.evaluate(self.vars[1])

class Ge(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 9, args_ret_dict['ge'], '>=')

	def get_symbol(self):
		return '>='

	def get_value(self):
		return self.evaluate(self.vars[0]) >= self.evaluate(self.vars[1])


class Eq(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 10, args_ret_dict['eq'], '==')

	def get_symbol(self):
		return '=='

	def get_value(self):
		return self.evaluate(self.vars[0]) == self.evaluate(self.vars[1])

class Ne(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 11, args_ret_dict['ne'], '!=')

	def get_symbol(self):
		return '!='

	def get_value(self):
		return self.evaluate(self.vars[0]) != self.evaluate(self.vars[1])

class In(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 12, args_ret_dict['in'], 'in')

	def get_symbol(self):
		return 'in'

	def get_value(self):
		return self.evaluate(self.vars[0]) in self.evaluate(self.vars[1])

	def __nonzero__(self):
		return self


class Or(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 23, args_ret_dict['or'], 'or')

	def get_symbol(self):
		return 'or'

	def get_value(self):
		return self.evaluate(self.vars[0]) or self.evaluate(self.vars[1])

class And(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 24, args_ret_dict['and'], 'and')

	def get_symbol(self):
		return '&'

	def get_value(self):
		return self.evaluate(self.vars[0]) & self.evaluate(self.vars[1])

class Xor(BinOp):

	def __init__(self, *vars):
		BinOp.__init__(self, vars, 25, args_ret_dict['xor'], 'xor')

	def get_symbol(self):
		return 'xor'

	def get_value(self):
		return bool(self.evaluate(self.vars[0])) != bool(self.evaluate(self.vars[1]))

class Pow(Call):
	def __init__(self, *vars):
		Call.__init__(self, vars, "pow", args_ret_dict['pow'], '^')

	def get_symbol(self):
		return '**'

	def get_value(self):
		return self.evaluate(self.vars[0]) ** self.evaluate(self.vars[1])

class Abort(Call):
	def __init__(self, *vars):
		Call.__init__(self, vars, "abort", args_ret_dict['abort'], 'abort')

class Abs(Call):
	def __init__(self, *vars):
		Call.__init__(self, vars, "abs", args_ret_dict['abs'], 'abs')


class Array1d(Call):
	def __init__(self, dim1, array):
		Call.__init__(self,(dim1, array),"array1d", args_ret_dict['array1d'], 'array1d')

class Array2d(Call):
	def __init__(self, dim1, dim2, array):
		Call.__init__(self, (dim1, dim2, array), "array2d", args_ret_dict['array2d'], 'array2d')

class Array3d(Call):
	def __init__(self, dim1, dim2, dim3, array):
		Call.__init__(self,[dim1, dim2, dim3, array],"array3d", args_ret_dict['array3d'], 'array3d')

class Array4d(Call):
	def __init__(self, dim1, dim2, dim3, dim4, array):
		Call.__init__(self,[dim1, dim2, dim3, dim4, array],"array4d", args_ret_dict['array4d'], 'array4d')

class Array5d(Call):
	def __init__(self, dim1, dim2, dim3, dim4, dim5, array):
		Call.__init__(self,[dim1, dim2, dim3, dim4, dim5, array],"array5d", args_ret_dict['array5d'], 'array5d')

class Array6d(Call):
	def __init__(self, dim1, dim2, dim3, dim4, dim5, dim6, array):
		Call.__init__(self,[dim1, dim2, dim3, dim4, dim5, dim6, array],"array6d", args_ret_dict['array6d'], 'array6d')

class Acos(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "acos", args_ret_dict['acos'], 'acos')

class Acosh(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "acosh", args_ret_dict['acosh'], 'acosh')

class Asin(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "asin", args_ret_dict['asin'], 'asin')

class Asinh(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "asinh", args_ret_dict['asinh'], 'asinh')

class Atan(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "atan", args_ret_dict['atan'], 'atan')

class Atanh(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "atanh", args_ret_dict['atanh'], 'atanh')

class Assert(Call):
	def __init__(self, constraint, message):
		Call.__init__(self, (constraint, message), "assert", args_ret_dict['assert'], 'assert')

class Bool2Int(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "bool2int", args_ret_dict['bool2int'], 'bool2int')

class Card(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "card", args_ret_dict['card'], 'card')

class Ceil(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "ceil", args_ret_dict['ceil'], 'ceil')

class Concat(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "concat",)

class Cos(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "cos", args_ret_dict['cos'], 'cos')

class Cosh(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "cosh", args_ret_dict['cosh'], 'cosh')

class Dom(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom")

class Dom_Array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom_array")

class Dom_Size(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom_size")

class Fix(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"fix")

class Floor(Call):
	def __init__(self, var):
		Call.__init__(self, (var,),"floor", args_ret_dict['floor'], 'floor')

class Exp(Call):
	def __init__(self, var):
		Call.__init__(self, (var,),"exp", args_ret_dict['exp'], 'exp')

class Int2Float(Call):
	def __init__(self, var):
		Call.__init__(self, (var,),"int2float", args_ret_dict['int2float'], 'int2float')

class Is_Fixed(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"is_fixed")

class Join(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"join")

class Lb(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"lb")

class Lb_array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"lb_array")

class Length(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"length")

class Ln(Call):
	def __init__(self, var):
		Call.__init__(self, (var,),"ln", args_ret_dict['ln'], 'ln')

class Log(Call):
	def __init__(self, var1, var2):
		Call.__init__(self, (var1, var2),"log", args_ret_dict['log'], 'log')

class Log2(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "log2", args_ret_dict['log2'], 'log2')

class Log10(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "log10", args_ret_dict['log10'], 'log10')

class Min(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "min", args_ret_dict(['min']), 'min')

class Max(Call):
	def __init__(self, var):
		Call.__init__(self, (var,), "max", args_ret_dict(['max']), 'max')

class Product(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"product", args_ret_dict['product'], 'product')

class Round(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"round", args_ret_dict['round'], 'round')

class Set2array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"set2array")

class Sin(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"sin", args_ret_dict['sin'], 'sin')

class Sinh(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"sinh", args_ret_dict['sinh'], 'sinh')

class Sqrt(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"sqrt", args_ret_dict['sqrt'], 'sqrt')

class Sum(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"sum", args_ret_dict['sum'], 'sum')

class Tan(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"tan", args_ret_dict['tan'], 'tan')

class Tanh(Call):
	def __init__(self, var):
		Call.__init__(self, (var, ),"tanh", args_ret_dict['tanh'], 'tanh')

class Trace(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"trace")

class Ub(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ub")

class Ub_Array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ub_array")


class Neg(UnOp):
	def __init__(self, var):
		UnOp.__init__(self, (var, ), 2, args_ret_dict['neg'], 'neg')

class Pos(UnOp):
	def __init__(self, var):
		UnOp.__init__(self, (var, ), 1, args_ret_dict['pos'], 'pos')

class Invert(Predicate):
	def __init__(self, vars):
		UnOp.__init__(self, (var, ), 0, args_ret_dict['invert'], 'invert')

class Id(Expression):
	def __init__(self, name, model_list = None):
		self.name = name
		self.model_list = model_list
		self.type = minizinc_internal.Annotation


# Temporary container for Variable Declaration
class Declaration(Expression):
	def __init__(self, model):
		if not isinstance(model, Model):
			raise TypeError('Warning: First argument must be a Model Object')
		Expression.__init__(self, model)
		self.name = '"' + str(id(self)) + '"'
		self.solve_counter = -1
		self.next_counter = -1
		self.value = None

	def get_value(self):
		if self.solve_counter == self.model.solve_counter and self.next_counter == self.model.next_counter:
			return self.value
		self.solve_counter = self.model.solve_counter
		self.next_counter = self.model.next_counter
		self.value = self.model.mznsolver.get_value(self.name)
		return self.value

	def __str__(self):
		return str(self.get_value())

	def __repr__(self):
		return 'A Minizinc ' + self.__class__.__name__ + ' with the value of ' + self.__str__()


class Construct(Declaration):
	def __init__(self, model, arg1):
		Declaration.__init__(self, model)
		self.type = self.eval_type(arg1)
		self.has_minizinc_objects = False

		def unwrap(arg):
			if isinstance(arg, Expression):
				self.has_minizinc_objects = True
				return arg.obj
			elif type(arg) in (list, tuple):
				ret = []
				for val in arg:
					ret.append(unwrap(val))
				return ret
			elif type(arg) in python_types:
				return arg
			else:
				raise TypeError('Unexpected type: argument should be a single/list/tuple of MiniZinc objects or Python basic types')

		try:
			self.obj = model.mznmodel.Declaration(self.name, unwrap(arg1))
		except:
			print sys.exc_info()[0]
			raise

		if self.has_minizinc_objects:
			self.value = arg1
		else:
			self.wrapped_value = arg1
		self.class_name = 'Construct'

	'''
		imagine we have:
			a = Variable(1,100)				undefined
			b = Variable(1,100)				undefined

			c = Construct([a,b])			[ undefined, undefined ]
		when the model is solved, c's value should be changed
	'''

	def get_value(self):
		if self.has_minizinc_objects:
			def get_value_helper(arg):
				if isinstance(arg, Expression):
					return arg.get_value()
				elif type(arg) is list:
					ret = []
					for val in arg:
						ret.append(get_value_helper(val))
					return ret
				elif type(arg) in python_types:
					return arg
				else:
					raise TypeError('Internal error: Unexpected type')
			if not (self.solve_counter == self.model.solve_counter and self.next_counter == self.obj.next_counter):
				self.value = get_value_helper(self.wrapped_value)
		return self.value




class Variable(Declaration):
	def __init__(self, model, arg1=None, arg2=None):
		Declaration.__init__(self, model)
		lb, ub = False, True
		if arg1 is not None:
			typearg1 = type(arg1)
			if arg2 is None:
				if typearg1 is Set:
					lb = arg1
					ub = None
				elif typearg1 in (list,tuple):
					if len(arg1) != 2:
						raise ValueError('Requires a list or tuple of exactly 2 numbers')
					lb,ub = sorted(arg1)[0,1]
				else:
					ub = arg1 - 1
					lb = typearg1(lb)
			else:
				lb,ub = arg1, arg2

		typelb, typeub = type(lb), type(ub)
		if not typelb is Set:
			if typelb not in (bool, float, ) and typelb not in integer_types and not isinstance(typelb, Declaration):
				raise TypeError('Lower bound must be a boolean, an int, a float or a set')
			if typeub not in (bool, float, ) and typeub not in integer_types and not isinstance(typelb, Declaration):
				raise TypeError('Upper bound must be a boolean, an int or a float')
			if typelb != typeub:
				raise TypeError('Upper bound an dlower bound is of different type')
			if lb > ub:
				raise ValueError('Lower bound cannot be greater than upper bound')

		self.dim_list = []

		if typelb is bool:
			self.obj = model.mznmodel.Declaration(self.name, 10, [])
			self.type = bool
		elif typelb in integer_types:
			self.obj = model.mznmodel.Declaration(self.name, 9, [], lb, ub)
			self.type = int_t
		elif typelb is float:
			self.obj = model.mznmodel.Declaration(self.name, 11, [], lb, ub)
			self.type = float
		elif typelb is Set:
			self.obj = model.mznmodel.Declaration(self.name, 9, [], lb.obj)
			self.type = int_t
		else:
			raise TypeError('Internal: Unexpected type')

		self.class_name = 'Variable'

class VariableConstruct(Variable, Construct):
	def __init__(self, model, arg1, arg2 = None):
		Construct.__init__(self, model, arg1, arg2)

class Array(Variable):
	def __init__(self, model, argopt1, argopt2, *args):
		Declaration.__init__(self, model)
		dim_list = []
		lb = None
		ub = None

		def add_to_dim_list(i):
			if type(i) is int:
				if i > 0:
					dim_list.append([0,i-1])
				else:
					raise TypeError('Single value must be a positive integer')
			elif type(i) is list:
				if type(i[0]) in integer_types and type(i[-1]) in integer_types:
					dim_list.append([ i[0],i[-1]] )
				else:
					raise TypeError('Range boundaries must be integers')
			elif isinstance(i, Set):
				if i.continuous():
					dim_list.append([i.min(), i.max()])
				else:
					raise TypeError('Array ranges must be continuous')
			else:
				raise TypeError('Unknown type')

		if type(argopt1) is Set:
			lb = argopt1
			ub = None
			add_to_dim_list(argopt2)
		elif type(argopt1) is bool and type(argopt2) is bool:
			lb = argopt1
			ub = argopt2
		elif type(argopt2) not in integer_types:
			if type(argopt1) not in integer_types:
				raise TypeError('Range values must be integers')
			lb = 0
			ub = argopt1 - 1
			add_to_dim_list(argopt2)
		else:
			if type(argopt1) not in integer_types or type(argopt2) not in integer_types:
				raise TypeError('Lower bound and upper bound must be integers')
			lb = argopt1
			ub = argopt2

		for i in args:
			add_to_dim_list(i)

		self.lb = lb
		self.ub = ub
		if dim_list == []:
			raise AttributeError('Initialising an Array without dimension list')
		self.dim_list = dim_list
		tlb = type(argopt1)
		if tlb is bool:
			self.type = [bool] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name,10,dim_list,lb,ub)
		elif tlb in integer_types:
			self.type = [int_t] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name,9,dim_list, lb, ub)
		elif tlb is float:  #isinstance(lb, float):
			self.type = [float] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name,11,dim_list,lb,ub)
		elif tlb is Set:
			self.type = [int_t] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name,9,dim_list,lb.obj)
		else:
			raise TypeError('Unexpected type')

		self.class_name = 'Array'

	def __getitem__(self, *args):
		return ArrayAccess(self.model, self, args[0])


# XXX: to be rewritten later
class ArrayAccess(Array):
	def __init__(self, model, array, idx):
		Declaration.__init__(self, model)
		if type(idx) is not tuple:
			idx = [idx]
		else:
			idx = flatten(idx)
		if len(idx) != len(array.dim_list):
			raise IndexError('Requires exactly ' + str(len(self.dim_list)) + ' index values')
		for i, value in enumerate(idx):
			if not isinstance(value, Expression):
				if value < array.dim_list[i][0] or value > array.dim_list[i][1]:
					raise IndexError('Index at pos ' + str(i) + ' is out of range')

		self.array = array
		self.idx = idx
		self.type = array.type[0]
		self.value = None
		self.class_name = 'Array Item'

	def get_value(self):
		if hasattr(self, 'solve_counter'):
			if self.solve_counter == self.model.solve_counter and self.next_counter == self.model.next_counter:
				return self.value
			self.solve_counter = self.model.solve_counter
			self.next_counter = self.model.next_counter
			arrayvalue = self.array.get_value()
			if arrayvalue is None:
				return None
			else:
				for i,value in enumerate(self.idx):
					arrayvalue = arrayvalue[value - self.array.dim_list[i][0]]
				self.value = arrayvalue
				return self.value
		return self.value

class ArrayConstruct(Array, Construct):
	def __init__(self, model, arg1, arg2 = None):
		Construct.__init__(self, model, arg1, arg2)


class Set(Declaration):
	# Set is model independent and can be reused in multiple models
	# Thus, Set.model = None
	def __init__(self, *args):
		self.model = None
		self.name = '"' + str(id(self)) + '"'
		'''
		lb, ub = None, None
		set_list = None

		if argopt2 is not None:
			lb,ub = argopt1, argopt2
		else:
			if type(argopt1) is list:
				set_list = argopt1
			else:
				ub = argopt1 - 1
				lb = 0

		if set_list is None:
			if not (type(lb) in integer_types and type(ub) in integer_types):
				raise TypeError('Lower bound and upper bound must be integers')
			set_list = [[lb, ub]]
		'''
		self.obj = minizinc_internal.Set(*args)
		self.type = minizinc_internal.Set
		self.class_name = 'Set'

	def push(self, *args):
		self.obj.push(*args)

	def clear(self):
		self.obj.clear()

	def continuous(self):
		return self.obj.continuous()

	def min(self):
		return self.obj.min()

	def max(self):
		return self.obj.max()

	def get_value(self):
		return self.obj

	def __iter__(self):
		return self.obj.__iter__()

	def __contains__(self, val):
		return self.obj.contains(val)

class VarSet(Variable):
	def __init__(self, model, argopt1, argopt2 = None):
		self.model = model
		self.name = '"' + str(id(self)) + '"'

		lb, ub = None, None
		set_list = None
		if argopt2 is not None:
			lb,ub = argopt1, argopt2
		else:
			if type(argopt1) is list:
				set_list = argopt1
			elif type(argopt1) is Set:
				lb = argopt1.min()
				ub = argopt1.max()
			else:
				ub = argopt1 - 1
				lb = 0

		if set_list is None:
			if not (type(lb) in integer_types and type(ub) in integer_types):
				raise TypeError('Lower bound and upper bound must be integers')

		model.mznmodel.Declaration(self.name, 12, [], lb, ub)
		self.type = minizinc_internal.VarSet
		self.class_name = 'Var Set'

	'''	Python automatically evaluate a __contains__ return object to boolean
		thus, we have to use separate In class instead of
			for i in VarSet
	def __contains__(self, argopt):
		return In([argopt, self])
	'''

variable_model_dict = {}
function_model_dict = {}
name_model_dict = {}
def handlerFunctionClosure(name, args_list, model_list):
	def handlerFunction(*args):
		return Call(args, name, args_list, name, model_list)
	return handlerFunction

def handlerFunction(name, model_list):
	return Id(name, model_list)

def init(args = None, model = None):
	def assign_usable_names_to_model(name, model):
		if name in name_model_dict:
			if model is None:
				name_model_dict[name] = None
			else:
				name_model_dict[name].append(model)
			return False
		else:
			if model is None:
				name_model_dict[name] = None
			else:
				name_model_dict[name] = [model]
			return True

	names = minizinc_internal.retrieveNames(args)
	for name, args_and_return_type_tuple in names["boolfuncs"].items():
		if assign_usable_names_to_model(name, model):
			setattr(predicate, name, handlerFunctionClosure(name, args_and_return_type_tuple, name_model_dict[name]))

	for name, args_and_return_type_tuple in names["annfuncs"].items():
		if assign_usable_names_to_model(name, model):
			setattr(predicate, name, handlerFunctionClosure(name, args_and_return_type_tuple, name_model_dict[name]))

	for name in names["annvars"]:
		if assign_usable_names_to_model(name, model):
			setattr(annotation, name, handlerFunction(name, name_model_dict[name]))

init()

class Model(object):
	def __init__(self, args = None):
		self.loaded = False
		self.mznsolver = None
		self.mznmodel = minizinc_internal.Model(args)
		if args:
			init(args, self)
		self.solve_counter = -1
		self.next_counter = -1
		

	'''
	# not used anymore
	def get_name(self,var):
		itemList = [var_name for var_name, var_val in self.frame if var_val is var]
		if len(itemList) == 1:
			return itemList[0]
		elif len(itemList) == 0:
			raise LookupError('Internal Error: variable name not found')
		else:
			raise LookupError('The object pointed to was assigned to different names')
	'''
	
	def add_recursive(self, expr):
		if isinstance(expr, (tuple, list)):
		    for i in expr:
		        self.add_recursive(i)
		else:
			if issubclass(type(expr), Expression):
				if expr.is_pre():
					self.mznmodel.Constraint(self.evaluate(expr))
				else:
					raise(TypeError, "Unexpected Expression")
			else:
				self.mznmodel.Constraint(expr)

	def Constraint(self, *expr):
		minizinc_internal.lock()
		if len(expr)>0:
			self.loaded = True
			#self.frame = inspect.currentframe().f_back.f_locals.items()
			self.add_recursive(expr)
			#del self.frame
		minizinc_internal.unlock()

	def evaluate(self, expr):
		if not isinstance(expr, Expression):
			if isinstance(expr, (list, tuple)):
				for i,item in enumerate(expr):
					expr[i] = self.evaluate(item)
			return expr
		if isinstance(expr, Id):
			if expr.model_list is not None:
				if not self in expr.model_list:
					raise TypeError("The annotation '" + expr.name + "' does not belong to the model")
			return minizinc_internal.Id(expr.name)
		if isinstance(expr, Call):
			variables = []
			model = None
			for i in expr.vars:
				variables.append(self.evaluate(i))
			if expr.model_list is not None:
				if not self in expr.model_list:
					raise TypeError("The function '" + expr.name + "' does not belong to the model")
			return minizinc_internal.Call(expr.CallCode, variables, expr.type)
		elif isinstance(expr, ArrayAccess):
			for i,value in enumerate(expr.idx):
				if isinstance(value, Expression):
					expr.idx[i] = self.evaluate(value)
			return minizinc_internal.at(expr.array.obj, expr.idx)
		elif isinstance(expr, Declaration):
			if expr.model != self:
				raise TypeError("The Declaration not belongs to this model")
			return expr.obj
		elif isinstance(expr, BinOp):
			if expr.model is not None and expr.model != self:
				raise TypeError("The Declaration not belongs to this model")
			lhs = self.evaluate(expr.vars[0])
			rhs = self.evaluate(expr.vars[1])
			return minizinc_internal.BinOp(lhs, expr.BinOpCode, rhs)
		elif isinstance(expr, UnOp):
			if expr.model is not None and expr.model != self:
				raise TypeError("The Declaration not belongs to this model")
			rhs = self.evaluate(expr.vars[0])
			return minizinc_internal.UnOp(expr.UnOpCode, rhs)
		else:
			raise TypeError('Variable Type unspecified')



	def Variable(self, argopt1=None, argopt2=None):
		return Variable(self, argopt1, argopt2)

	def Array(self, argopt1, argopt2, *args):
		list_ = []
		for i in args:
			list_.append(i)
		return Array(self, argopt1, argopt2, *list_)

	def Set(self, *args):
		return Set(*args)

	def Range(self, arg1, arg2 = None):
		if (arg2 is None):
			return Set((0, arg1 - 1))
		else:
			return Set((arg1, arg2))

	def VarSet(self, argopt1, argopt2 = None):
		return VarSet(self, argopt1, argopt2)

	def Construct(self, argopt1):
		if isinstance(argopt1, list):
			return ArrayConstruct(self, argopt1)
		else:
			return VariableConstruct(self, argopt1)

	def __solve(self, code, expr, ann, data, solver, time):
		minizinc_internal.lock()

		if ann is not None:
			if not hasattr(ann, 'type') or ann.type != minizinc_internal.Annotation:
				raise TypeError('Unexpected type of annotation')
		eval_ann = self.evaluate(ann)
		eval_expr = self.evaluate(expr)

		savedModel = self.mznmodel.copy()

		# The model with declared variable cannot be deleted.
		self.mznmodel, savedModel = savedModel, self.mznmodel
		self.mznmodel.SolveItem(code, eval_ann, eval_expr)
		if data is not None:
			self.add_recursive(data)
		minizinc_internal.unlock()

		self.mznsolver = self.mznmodel.solve(solver = solver, time = time)
		self.mznmodel = savedModel
		self.solve_counter = self.solve_counter + 1
		self.next_counter = -1


	def satisfy(self, ann = None, data = None, solver = 'gecode', time = 0):
		self.__solve(0, None, ann, data, solver, time)
	def maximize(self, expr, ann = None, data = None, solver = 'gecode', time = 0):
		self.__solve(2, expr, ann, data, solver, time)
	def minimize(self, expr, ann = None, data = None, solver = 'gecode', time = 0):
		self.__solve(1, expr, ann, data, solver, time)
	def reset(self):
		self.__init__()

	def next(self):
		if self.mznsolver is None:
			raise ValueError('Model is not solved yet')
		self.status = self.mznsolver.next()
		self.next_counter = self.next_counter + 1
		return (self.status is None)

	def is_loaded(self):
		return self.loaded

	def is_solved(self):
		return self.mznsolver != None

	def set_time_limit(self, time):
		self.mznmodel.set_time_limit(time)

	def set_solver(self, solver):
		self.mznmodel.set_solver(solver)

	def _debugprint(self):
		self.mznmodel.debugprint()


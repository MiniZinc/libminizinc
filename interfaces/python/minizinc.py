##@mainpage MiniZinc Python Interface
# @author	Tai Tran, under the supervision of Guido Tack
#


import minizinc_internal
import predicate
import annotation
#import inspect

def flatten(x):
	result = []
	for el in x:
		if isinstance(el, (list, tuple)):
			result.extend(flatten(el))
		else:
			result.append(el)
	return result


# All Variable and Expression declaration derived from here
class Expression(object):
	def __init__(self, model = None):
		self.model = model

	def __str__(self):
		return str(self.get_value())

	def is_solved(self):
		if self == None:
			return False
		else:
			return self.model.is_solved()

	def is_var(self):
		return isinstance(self, Declaration)

	def is_pre(self):
		return isinstance(self, Predicate)

	def evaluate(self, var):
		if isinstance(var, Expression):
			ret = var.get_value()
			if ret == None:
				raise ValueError('Variable value it not set')
			return ret
		else:
			return var

	def __and__(self, pred):
		return And( (self, pred,) )

	def __rand__(self, pred):
		return And( (self, pred,) )

	def __or__(self, pred):
		return Or( (self, pred,) )

	def __xor__(self, pred):
		return Xor( (self, pred,) )

	def __ror__(self, pred):
		return Or( (self, pred,) )

	def __add__(self, pred):
		return Add( (self, pred,) )

	def __radd__(self, pred):
		return Add( (pred, self,) )

	def __sub__(self, pred):
		return Sub( (self, pred,) )

	def __rsub__(self, pred):
		return Sub( (pred, self,) )

	def __div__(self, pred):
		return Div( (self, pred,) )

	def __rdiv__(self, pred):
		return Div( (pred, self,) )

	def __floordiv__(self, pred):
		return FloorDiv( (self, pred,) )

	def __rfloordiv__(self, pred):
		return FloorDiv( (pred, self,) )

	def __mul__(self, pred):
		return Mul( (self, pred,) )

	def __rmul__(self, pred):
		return Mul( (self, pred,) )

	def __mod__(self, pred):
		return Mod( (self, pred,) )

	def __rmod__(self, pred):
		return Mod( (pred, self,) )

	def __pow__(self, pred):
		return Pow( (self, pred,) )

	def __rpow__(self, pred):
		return Pow( (pred, self,) )

	def __eq__(self, pred):
		#if CHECK_VAR_EQUALITY[0]
		#	raise InvalidConstraintSpecification("use == outside the model definition")
		return Eq( (self, pred,) )

	def __ne__(self, pred):
		#if CHECK_VAR_EQUALITY[0]
		#	raise InvalidConstraintSpecification("use != outside the model definition")
		return Ne( (self, pred,) )	

	def __lt__(self, pred):
		return Lt( (self, pred,) )

	def __gt__(self, pred):
		return Gt( (self, pred,) )

	def __le__(self, pred):
		return Le( (self, pred,) )

	def __ge__(self, pred):
		return Ge( (self, pred,) )

	def __neg__(self):
		return Neg( (self,) )

	def __pos__(self):
		return Pos( (self,))

	def __invert__(self):
		return Invert( (self,) )

	def __abs__(self):
		return Abs( (self,) )


# Temporary container for expression before evaluating to minizinc_internal object
class Predicate(Expression):
	def __init__(self, vars, args_and_return_type_tuple = None, model = None):
		self.vars = vars
		def eval_type(args):
			if isinstance(args, Expression):
				return args.type
			elif type(args) in (int, float, bool, str):
				return type(args)
			elif type(args) is list:
				t = None
				for i in args:
					if t is None:
						t = eval_type(i)
					else:
						if t != eval_type(i):
							raise TypeError("Predicate: Type of arguments in the list must be the same")
				return [t]
			elif type(args) is tuple:
				t = []
				for i in args:
					t.append(eval_type(i))
				return tuple(t)
			else:
				raise TypeError("Predicate: Unexpected Type (" + str(type(args)) + ")")
		self.vars_type = eval_type(vars)
		if args_and_return_type_tuple is not None:
			for i in args_and_return_type_tuple:
				if self.vars_type == i[0]:
					self.type = i[1]
					break
			else:
				s = "Argument type does not match, received " + str(self.vars_type) + ", expected: "
				for i in args_and_return_type_tuple:
					s = s + str(i[0]) + " or "
				raise TypeError(s)
		else:
			self.type = None
		Expression.__init__(self)
		'''
		model = None
		for i in vars:
			if hasattr(i,'model'):
				if model==None:
					model = i.model
				elif model != i.model:
					raise TypeError('Evaluating expression of different types')
		self.model = model
		'''


class BinOp(Predicate):
	def __init__(self, vars, code, args_and_return_type_tuple):
		Predicate.__init__(self,vars,args_and_return_type_tuple)
		self.BinOpCode = code

class UnOp(Predicate):
	def __init__(self, vars, code, args_and_return_type_tuple):
		Predicate.__init__(self,vars, args_and_return_type_tuple)
		self.UnOpCode = code

class Call(Predicate):
	def __init__(self, vars, code, args_and_return_type_tuple = None, model_list = None):
		Predicate.__init__(self,vars, args_and_return_type_tuple)
		self.CallCode = code
		self.model_list = model_list

'''
base_dict = {}
base_dict['int']   = (type(int), type(int))
base_dict['float'] = ((type(float), type(float)), type(float))
base_dict['bool']  = ((type(bool), type(bool)), type(bool))
'''

args_ret_dict = {}
args_ret_dict['add'] =[ ((int,int), int ),
						((float, float), float ) ]
args_ret_dict['sub'] = args_ret_dict['add']
args_ret_dict['mul'] = args_ret_dict['add']
args_ret_dict['div'] = args_ret_dict['add']
args_ret_dict['floordiv'] = args_ret_dict['add']
args_ret_dict['pow'] = args_ret_dict['add']
args_ret_dict['mod'] =[ ((int, int), int )]

args_ret_dict['lt'] = [ ((int, int), bool ), 
						((float, float), bool ),
						((bool, bool), bool ),
						((str, str), bool ),
						((minizinc_internal.VarSet, minizinc_internal.VarSet), bool)]
args_ret_dict['le'] = args_ret_dict['lt']
args_ret_dict['gt'] = args_ret_dict['lt']
args_ret_dict['ge'] = args_ret_dict['lt']
args_ret_dict['eq'] = args_ret_dict['lt']
args_ret_dict['ne'] = args_ret_dict['lt']
args_ret_dict['in'] = [ ((int, minizinc_internal.VarSet), bool) ]
args_ret_dict['or'] = [ ((bool, bool), bool) ]
args_ret_dict['and'] = args_ret_dict['or']
args_ret_dict['xor'] = args_ret_dict['or']


class Add(BinOp):
	def __init__(self, vars):
		BinOp.__init__(self, vars, 0, args_ret_dict['add'])

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
	def __init__(self, vars):
		BinOp.__init__(self, vars, 1, args_ret_dict['sub'])

	def get_symbol(self):
		return '-'

	def get_value(self):
		return self.evaluate(self.vars[0]) - self.evaluate(self.vars[1])

class Mul(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 2, args_ret_dict['mul'])

	def get_symbol(self):
		return '*'

	def get_value(self):
		return self.evaluate(self.vars[0]) * self.evaluate(self.vars[1])

class Div(BinOp):

	def __init__(self, vars) :
		BinOp.__init__(self, vars, 3, args_ret_dict['div'])

	def get_symbol(self):
		return '/'

	def get_value(self):
		return self.evaluate(self.vars[0]) / self.evaluate(self.vars[1])

class FloorDiv(BinOp):

	def __init__(self, vars) :
		BinOp.__init__(self, vars, 4, args_ret_dict['floordiv'])

	def get_symbol(self):
		return '//'

	def get_value(self):
		return self.evaluate(self.vars[0]) // self.evaluate(self.vars[1])


class Mod(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 5, args_ret_dict['mod'])

	def get_symbol(self):
		return '%'

	def get_value(self):
		return self.evaluate(self.vars[0]) % self.evaluate(self.vars[1])


class Lt(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 6, args_ret_dict['lt'])

	def get_symbol(self):
		return '<'

	def get_value(self):
		return self.evaluate(self.vars[0]) < self.evaluate(self.vars[1])

class Le(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 7, args_ret_dict['le'])

	def get_symbol(self):
		return '<='

	def get_value(self):
		return self.evaluate(self.vars[0]) <= self.evaluate(self.vars[1])

class Gt(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 8, args_ret_dict['gt'])

	def get_symbol(self):
		return '>'

	def get_value(self):
		return self.evaluate(self.vars[0]) > self.evaluate(self.vars[1])

class Ge(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 9, args_ret_dict['ge'])

	def get_symbol(self):
		return '>='

	def get_value(self):
		return self.evaluate(self.vars[0]) >= self.evaluate(self.vars[1])


class Eq(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 10, args_ret_dict['eq'])

	def get_symbol(self):
		return '=='

	def get_value(self):
		return self.evaluate(self.vars[0]) == self.evaluate(self.vars[1])

class Ne(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 11, args_ret_dict['ne'])

	def get_symbol(self):
		return '!='

	def get_value(self):
		return self.evaluate(self.vars[0]) != self.evaluate(self.vars[1])

class In(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 12, args_ret_dict['in'])

	def get_symbol(self):
		return 'in'

	def get_value(self):
		return self.evaluate(self.vars[0]) in self.evaluate(self.vars[1])

	def __nonzero__(self):
		return self


class Or(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 23, args_ret_dict['or'])

	def get_symbol(self):
		return 'or'

	def get_value(self):
		return self.evaluate(self.vars[0]) or self.evaluate(self.vars[1])

class And(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 24, args_ret_dict['and'])

	def get_symbol(self):
		return '&'

	def get_value(self):
		return self.evaluate(self.vars[0]) & self.evaluate(self.vars[1])

class Xor(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 25, args_ret_dict['xor'])

	def get_symbol(self):
		return 'xor'

	def get_value(self):
		return bool(self.evaluate(self.vars[0])) != bool(self.evaluate(self.vars[1]))

class Pow(Call):
	def __init__(self, vars):
		Call.__init__(self, vars, "pow", args_ret_dict['pow'])

	def get_symbol(self):
		return '**'

	def get_value(self):
		return self.evaluate(self.vars[0]) ** self.evaluate(self.vars[1])

class Abort(Call):
	def __init__(self):
		Call.__init__(self, [], "abort")

class Abs(Call):
	def __init__(self, var):
		Call.__init__(self, var, "abs")

class Acosh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"acosh")
'''
class Arrayxd():
	def __init__(self, dimsize, *args):
		if dimsize + 1 != len(args):
			raise BaseException('Array',dimsize,'d requires exactly', dimsize+1, 'arguments');
		for i in range(dimsize):
			for p[i] in args[i]:
'''

class Array1d(Call):
	def __init__(self, dim1, array):
		Call.__init__(self,[dim1, array],"array1d")

def Array2d(dim1, dim2, array):
	p = [[0 for x in range(dim1)] for y in range(dim2)]
	for i in range(dim1):
		for j in range(dim2):
			p[i][j] = array[i*dim2 + j]
	return p

class Array3d(Call):
	def __init__(self, dim1, dim2, dim3, array):
		Call.__init__(self,[dim1, dim2, dim3, array],"array3d")

class Array4d(Call):
	def __init__(self, dim1, dim2, dim3, dim4, array):
		Call.__init__(self,[dim1, dim2, dim3, dim4, array],"array4d")

class Array5d(Call):
	def __init__(self, dim1, dim2, dim3, dim4, dim5, array):
		Call.__init__(self,[dim1, dim2, dim3, dim4, dim5, array],"array5d")

class Array6d(Call):
	def __init__(self, dim1, dim2, dim3, dim4, dim5, dim6, array):
		Call.__init__(self,[dim1, dim2, dim3, dim4, dim5, dim6, array],"array6d")

class Asin(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"asin")

class Atan(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"atan")

class Assert(Call):
	def __init__(self, constraint, message):
		Call.__init__(self,[constraint, message],"assert")

class Bool2Int(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"bool2int")

class Card(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"card")

class Ceil(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ceil")

class Concat(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"concat")

class Cos(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"cos")

class Cosh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"cosh")

class Dom(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"Dom")

class Dom_Array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom_array")

class Dom_Size(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom_size")

class Fix(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"fix")

class Exp(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"exp")

class Floor(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"floor")

class Exp(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"exp")

class Int2Float(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"int2float")

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
		Call.__init__(self,[var],"ln")

class Log(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"log")

class Log2(Call):
	def __init__(self, var1, var2):
		Call.__init__(self,[var1, var2],"log2")

class Log10(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"log10")

class Min(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"min")

class Max(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"max")

class Product(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"product")

class Round(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"round")

class Set2array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"set2array")

class Sin(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sin")

class Sinh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sinh")

class Sqrt(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sqrt")

class Sum(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sum")

class Tan(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"tan")

class Tanh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"tanh")

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
	def __init__(self, vars):
		UnOp.__init__(self, vars, 2)

class Pos(UnOp):
	def __init__(self, vars):
		UnOp.__init__(self, vars, 1)

class Invert(Predicate):
	def __init__(self, vars):
		UnOp.__init__(self, vars, 0)

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
		self.solution_counter = -1

	def get_value(self):
		if hasattr(self, 'solution_counter') and self.solution_counter < self.model.solution_counter:
			self.value = self.model.mznsolver.getValue(self.name)
			return self.value
		if hasattr(self, 'value'):
			return self.value
		else:
			return None
			'''
			if self.model.is_solved():
				self.value = self.model.mznsolver.getValue(self.name)
				return self.value
			else:
				return None
			'''

	def __str__(self):
		return str(self.get_value())

	def __repr__(self):
		return 'A Minizinc Object with the value of ' + self.__str__()

class Construct(Declaration):
	def __init__(self, model, arg1, arg2 = None):
		Declaration.__init__(self, model)
		del self.solution_counter
		if arg2 != None:
			if type(arg2) is str:
				self.name = arg2
			else: 
				raise TypeError('Name of variable must be a string')
		if hasattr(arg1, 'obj'):
			self.obj = model.mznmodel.Declaration(self.name, arg1.obj)
		else:
			self.obj = model.mznmodel.Declaration(self.name, arg1)
		self.value = arg1
		self.type = self.obj.type


class Variable(Declaration):
	def __init__(self, model, arg1=None, arg2=None, arg3=None):
		Declaration.__init__(self, model)
		name = None
		lb, ub = False, True
		code = None
		if arg1 is not None:
			typearg1 = type(arg1)
			if arg2 is None:
				if typearg1 is str:
					name = arg1
				elif typearg1 is Set:
					lb = arg1
					ub = None
				elif typearg1 in (list,tuple):
					if len(arg1) != 2:
						raise ValueError('Requires a list or tuple of exactly 2 numbers')
					lb,ub = sorted(arg1)[0,1]
				else:
					ub = arg1 - 1
					lb = typearg1(lb)
			elif arg3 is None:
				typearg2 = type(arg2)
				if typearg2 is str:
					name = arg2
					if typearg1 is Set:
						lb = arg1
						ub = None
					elif typearg1 in (list, tuple):
						if len(arg1) != 2:
							raise ValueError('Requires a list or tuple of exactly 2 numbers')
						lb,ub = sorted(arg1)[0,1]
					elif typearg1 in (int, long):
						ub = arg1 - 1
						lb = typearg1(ub)
					else:
						raise TypeError('Requires a set, list or an integer')
				else:
					lb,ub = arg1, arg2
			else:
				if type(arg3) is not str:
					raise TypeError('Third argument must be a string')
				lb,ub = arg1, arg2

		typelb, typeub = type(lb), type(ub)
		if not typelb is Set:
			if typelb not in [bool, int, float] and not isinstance(typelb, Declaration):
				raise TypeError('Lower bound must be a boolean, an int, a float or a set')
			if typeub not in [bool, int, float] and not isinstance(typelb, Declaration):
				raise TypeError('Upper bound must be a boolean, an int or a float')
			if typelb != typeub:
				raise TypeError('Upper bound an dlower bound is of different type')
			if lb > ub:
				raise ValueError('Lower bound cannot be greater than upper bound')

		self.dim_list = []
		self.type = typelb
		if name is not None:
			self.name = name

		if typelb is bool:
			self.obj = model.mznmodel.Declaration(self.name, 10, [])
		elif typelb is int:
			self.obj = model.mznmodel.Declaration(self.name, 9, [], lb, ub)
		elif typelb is float:
			self.obj = model.mznmodel.Declaration(self.name, 11, [], lb, ub)
		elif typelb is Set:
			self.obj = model.mznmodel.Declaration(self.name, 9, [], lb.obj)
			self.type = int
		#elif isinstance(lb, Declaration) or isinstance(ub, Declaration):
		#	self.obj = model.mznmodel.Declaration(self.name, 12, [], lb, ub)

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
				if type(i[0]) is int and type(i[-1]) is int:
					dim_list.append([ i[0],i[-1]] )
				else:
					raise TypeError('Range boundaries must be integers')
			elif isinstance(i, Set):
				if i.continuous():
					dim_list.append([i.min(), i.max()])
				else:
					raise TypeError('Array ranges must be continuous')
			elif isinstance(i, str):
				self.name = i
			else:
				raise RuntimeError('Unknown error')

		if type(argopt1) is Set:
			lb = argopt1
			ub = None
			add_to_dim_list(argopt2)
		elif type(argopt1) is bool and type(argopt2) is bool:
			lb = argopt1
			ub = argopt2
		elif type(argopt2) is not int:
			if type(argopt1) is not int:
				raise TypeError('Range values must be integers')
			lb = 0
			ub = argopt1 - 1
			add_to_dim_list(argopt2)
		else:
			if type(argopt1) is not int or type(argopt2) is not int:
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
		elif tlb is int:
			self.type = [int] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name, 9,dim_list,lb,ub)
		elif tlb is float:  #isinstance(lb, float):
			self.type = [float] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name,11,dim_list,lb,ub)
		elif tlb is Set:
			self.type = [int] * len(dim_list)
			self.obj = model.mznmodel.Declaration(self.name, 9,dim_list,lb.obj)

	def __getitem__(self, *args):
		return ArrayAccess(self.model, self, args[0])

class ArrayAccess(Array):
	def __init__(self, model, array, idx):
		Declaration.__init__(self, model)
		self.array = array
		if type(idx) is not tuple:
			self.idx = [idx]
		else:
			self.idx = flatten(idx)
		for i in range(len(self.idx)):
			if hasattr(self.idx[i],'obj'):
				self.idx[i] = self.idx[i].obj
		self.type = array.type[0]

	def get_value(self):
		if not hasattr(self, 'solution_counter'):
			# must be a Constructed Variable
			return self.value
		if self.solution_counter < self.model.solution_counter:
			# Model has been solved at least once
			arrayvalue = self.array.get_value()
			if arrayvalue is None:
				return None
			else:
				for i in range(len(self.idx)):
					arrayvalue = arrayvalue[self.idx[i] - self.array.dim_list[i][0]]
				self.value = arrayvalue
				return arrayvalue
			return self.value
		if hasattr(self, 'value'):
			return self.value
		else:
			return None

class ArrayConstruct(Array, Construct):
	def __init__(self, model, arg1, arg2 = None):
		Construct.__init__(self, model, arg1, arg2)


class Set(Declaration):
	def __init__(self, model, argopt1, argopt2=None, argopt3=None):
		if isinstance(model, Model) == False:
			argopt3 = argopt2
			argopt2 = argopt1
			argopt1 = model
			self.model = None
		else:
			self.model = model
		self.name = '"' + str(id(self)) + '"'

		name = None
		lb, ub = None, None
		set_list = None

		if argopt3 is not None:
			lb,ub = argopt1, argopt2
			name = argopt3
		elif argopt2 is not None:
			if type(argopt2) is str:
				name = argopt2
				ub = argopt1
			else:
				lb,ub = argopt1, argopt2
		else:
			if type(argopt1) is list:
				set_list = argopt1
			else:
				ub = argopt1 - 1
				lb = 0

		if name is not None:
			if type(name) is not str:
				raise TypeError('Name must be a string');
			else:
				self.name = name
		if set_list is None:
			if not (type(lb) is int and type(ub) is int):
				raise TypeError('Lower bound and upper bound must be integers')
			set_list = [[lb,ub]]
		self.obj = minizinc_internal.Set(set_list)
		self.type = minizinc_internal.Set

	def continuous(self):
		return self.obj.continuous()

	def min(self):
		return self.obj.min()

	def max(self):
		return self.obj.max()

	def get_value(self):
		if type(self.obj) is minizinc_internal.Set:
			self.value = self.obj
			return self.value
		else:
			self.value = self.model.mznsolver.getValue(self.name)
			return self.value

	def __iter__(self):
		return self.obj.__iter__()

	def __contains__(self, val):
		return self.obj.contains(val)

	#def __str__(self):
	#	return self.obj

class VarSet(Variable):
	def __init__(self, model, argopt1, argopt2 = None, argopt3 = None):
		self.model = model
		self.name = '"' + str(id(self)) + '"'

		name = None
		lb, ub = None, None
		set_list = None
		if argopt3 is not None:
			lb,ub = argopt1, argopt2
			name = argopt3
		elif argopt2 is not None:
			if type(argopt2) is str:
				name = argopt2
				ub = argopt1
			else:
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

		if name is not None:
			if type(name) is not str:
				raise TypeError('Name must be a string');
			else:
				self.name = name
		if set_list is None:
			if not (type(lb) is int and type(ub) is int):
				raise TypeError('Lower bound and upper bound must be integers')
			set_list = [[lb,ub]]
		self.obj = model.mznmodel.Declaration(self.name, 12, [], lb, ub)
		self.type = minizinc_internal.VarSet

	def __contains__(self, argopt):
		return In([argopt, self])

variable_model_dict = {}
function_model_dict = {}
name_model_dict = {}
def handlerFunctionClosure(name, args_list, model_list):
	def handlerFunction(*args):
		return Call(args, name, args_list, model_list)
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
			'''
		if model is None:
			name_model_dict[name] = None
		else:
			if name in name_model_dict:
				if name_model_dict[name] is not None:
					name_model_dict[name].append(model)
			else:
				name_model_dict[name] = [model]
			'''

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
		self.solution_counter = -1
		


	# not used anymore
	def get_name(self,var):
		itemList = [var_name for var_name, var_val in self.frame if var_val is var]
		if len(itemList) == 1:
			return itemList[0]
		elif len(itemList) == 0:
			raise LookupError('Internal Error: variable name not found')
		else:
			raise LookupError('The object pointed to was assigned to different names')

	def Constraint(self, *expr):
		minizinc_internal.lock()
		if len(expr)>0:
			self.loaded = True
			#self.frame = inspect.currentframe().f_back.f_locals.items()
			self.add_prime(expr)
			#del self.frame
		minizinc_internal.unlock()

	def evaluate(self, expr):
		if not isinstance(expr, Expression):
			if isinstance(expr, (list, tuple)):
				model = None
				for i,item in enumerate(expr):
					expr[i], m = self.evaluate(item)
					if model is None:
						model = m
					elif m is not None and m != model:
						raise TypeError("Objects in '" + expr.name + "' must be free or belong to the same model")
				return (expr,model)
			else:
				return (expr,None)
		if isinstance(expr, Id):
			if expr.model_list is not None:
				if not self in expr.model_list:
					raise TypeError("The annotation '" + expr.name + "'' does not belong to the model")
			return (minizinc_internal.Id(expr.name), None)
		if isinstance(expr, Call):
			variables = []
			model = None
			for i in expr.vars:
				var, m = self.evaluate(i)
				if model is None:
					model = m
				elif m is not None and model != m:
					raise TypeError("Objects in '" + expr.name + "' must be free or belong to the same model")
				variables.append(var)
			if expr.model_list is not None:
				if not self in expr.model_list:
					raise TypeError("The function '" + expr.name + "'' does not belong to the model")
			return (minizinc_internal.Call(expr.CallCode, variables, expr.type), model)
		elif isinstance(expr, ArrayAccess):
			return (expr.array.obj.at(expr.idx), expr.model)
		elif isinstance(expr, Declaration):
			#if not expr.is_added:
			#	expr.is_added = True
			#	expr.name = get_name(expr)
			#	expr.obj = mznmodel.Declaration(expr.name, expr.VarCode,
			#						expr.dim_list, expr.lb, expr.ub)
			return (expr.obj, expr.model)
		elif isinstance(expr, BinOp):
			lhs, model = self.evaluate(expr.vars[0])
			rhs, model2 = self.evaluate(expr.vars[1])
			if model is None:
				model = model2
			if model2 is not None and model2 != model:
				raise TypeError("Objects in '" + expr.name + "' must be free or belong to the same model")
			return (minizinc_internal.BinOp(lhs, expr.BinOpCode, rhs), model)
		elif isinstance(expr, UnOp):
			ret, model = self.evaluate(expr.vars[0])
			return (minizinc_internal.UnOp(expr.UnOpCode, self.evaluate(expr.vars[0])), model)
		else:
			raise TypeError('Variable Type unspecified')

#Numberjack
	def add_prime(self, expr):
		if issubclass(type(expr), list):
			for exp in expr:
				self.add_prime(exp)
		elif issubclass(type(expr), tuple):
			for exp in expr:
				self.add_prime(exp)
		elif issubclass(type(expr), dict):
			for key in expr:
				self.add_prime(exp[key])
		else:
			if issubclass(type(expr), Expression):
				if (expr.is_pre()):
					obj, model = self.evaluate(expr)
					if model != None and model != self:
						raise TypeError('Expressions must belong to this model')
					self.mznmodel.Constraint(obj)
				else:
					raise(TypeError, "Unexpected expression")
			else:
				self.mznmodel.Constraint(expr)
				'''
				if isinstance(expr, bool):
					self.mznmodel.Constraint(expr)
				else:
					raise(TypeError, "Argument must be a minizinc_internal expression or python boolean")
				'''

	def Variable(self, argopt1=None, argopt2=None, argopt3=None):
		return Variable(self, argopt1, argopt2, argopt3)
		#var.name = get_name(var)
		#var.obj = m.mznmodel.Declaration(var.name, var.code,
		#						var.dim_list, var.lb, var.ub)

	def Array(self, argopt1, argopt2, *args):
		list_ = []
		for i in args:
			list_.append(i)
		return Array(self, argopt1, argopt2, *list_)

	def Set(self, argopt1, argopt2=None, argopt3=None):
		return Set(self, argopt1, argopt2, argopt3)

	def VarSet(self, argopt1, argopt2 = None, argopt3 = None):
		return VarSet(self, argopt1, argopt2, argopt3)

	def Construct(self, argopt1, argopt2 = None):
		if isinstance(argopt1, list):
			return ArrayConstruct(self, argopt1, argopt2)
		else:
			return VariableConstruct(self, argopt1, argopt2)

	def __solve(self):
		self.mznsolver = self.mznmodel.solve()

	def satisfy(self, ann = None):
		if not self.is_loaded():
			raise ValueError('Model is not loaded yet')
		#self.loaded = False
		'''
		if ann is None:
			self.mznmodel.SolveItem(0)
		else:
		'''
		minizinc_internal.lock()
		eval_ann, model = self.evaluate(ann)
		if model is not None and model != self:
			raise TypeError('Expression must be free or belong to the same model') 
		self.mznmodel.SolveItem(0, eval_ann)
		minizinc_internal.unlock()
		self.__solve()

	def optimize(self, arg, code, ann = None):
		minizinc_internal.lock()
		obj, model = self.evaluate(arg)
		if model is not None and model != self:
			raise TypeError('Expression must be free or belong to the same model')
		#self.loaded = False
		self.mznmodel.SolveItem(code, ann, obj)
		minizinc_internal.unlock()
		self.__solve()

	def maximize(self, arg, ann = None):
		self.optimize(arg, 2, ann)
	def minimize(self, arg, ann = None):
		self.optimize(arg, 1, ann)
	def reset(self):
		self.__init__()

	def next(self):
		if self.mznsolver == None:
			raise ValueError('Model is not solved yet')
		try: 
			self.mznsolver.next()
		except Exception as rt:
			template = "{0}: {1!r}"
			message = template.format(type(rt).__name__, rt.args[0])
			print message
			return False
		else:
			self.solution_counter = self.solution_counter + 1
			return True

	def is_loaded(self):
		return self.loaded

	def is_solved(self):
		return self.mznsolver != None

	def set_time_limit(self, time):
		self.mznmodel.setTimeLimit(time)

	def set_solver(self, solver):
		self.mznmodel.setSolver(solver)



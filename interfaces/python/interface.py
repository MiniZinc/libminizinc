##@mainpage MiniZinc Python Interface
# @author	Tai Tran, under the supervision of Guido Tack
#


import minizinc

def flatten(x):
	result = []
	for el in x:
		if hasattr(el, "__iter__") and not isinstance(el, basestring) and not issubclass(type(el), Expression):
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
		return isinstance(self, VarDecl)

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
		return And([self, pred])

	def __rand__(self, pred):
		return And([self, pred])

	def __or__(self, pred):
		return Or([self, pred])

	def __xor__(self, pred):
		return Xor([self, pred])

	def __ror__(self, pred):
		return Or([self, pred])

	def __add__(self, pred):
		return Add([self, pred])

	def __radd__(self, pred):
		return Add([pred, self])

	def __sub__(self, pred):
		return Sub([self, pred])

	def __rsub__(self, pred):
		return Sub([pred, self])

	def __div__(self, pred):
		return Div([self, pred])

	def __rdiv__(self, pred):
		return Div([pred, self])

	def __mul__(self, pred):
		return Mul([self, pred])

	def __rmul__(self, pred):
		return Mul([self, pred])

	def __mod__(self, pred):
		return Mod([self, pred])

	def __rmod__(self, pred):
		return Mod([pred, self])

	def __eq__(self, pred):
		#if CHECK_VAR_EQUALITY[0]
		#	raise InvalidConstraintSpecification("use == outside the model definition")
		return Eq([self, pred])

	def __ne__(self, pred):
		#if CHECK_VAR_EQUALITY[0]
		#	raise InvalidConstraintSpecification("use != outside the model definition")
		return Ne([self, pred])	

	def __lt__(self, pred):
		return Lt([self, pred])

	def __gt__(self, pred):
		return Gt([self, pred])

	def __le__(self, pred):
		return Le([self, pred])

	def __ge__(self, pred):
		return Ge([self, pred])

	def __pow__(self, pred):
		return Pow([self, pred])

	def __neg__(self):
		return Neg([self])

	def __pos__(self):
		return Pos([self])

	def __invert__(self):
		return Invert([self])

	def __abs__(self):
		return Abs([self])


# Temporary container for expression before evaluating to minizinc object
class Predicate(Expression):
	def __init__(self, vars):
		self.vars = vars
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
	def __init__(self, vars, code):
		Predicate.__init__(self,vars)
		self.BinOpCode = code

class UnOp(Predicate):
	def __init__(self, vars, code):
		Predicate.__init__(self,vars)
		self.UnOpCode = code

class Call(Predicate):
	def __init__(self, vars, code):
		Predicate.__init__(self,vars)
		self.CallCode = code

class Add(BinOp):
	def __init__(self, vars):
		BinOp.__init__(self, vars, 0)

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
		BinOp.__init__(self, vars, 1)

	def get_symbol(self):
		return '-'

	def get_value(self):
		return self.evaluate(self.vars[0]) - self.evaluate(self.vars[1])

class Mul(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 2)

	def get_symbol(self):
		return '*'

	def get_value(self):
		return self.evaluate(self.vars[0]) * self.evaluate(self.vars[1])

class Div(BinOp):

	def __init__(self, vars) :
		BinOp.__init__(self, vars, 3)

	def get_symbol(self):
		return '/'

	def get_value(self):
		return self.evaluate(self.vars[0]) / self.evaluate(self.vars[1])

class FloorDiv(BinOp):

	def __init__(self, vars) :
		BinOp.__init__(self, vars, 4)

	def get_symbol(self):
		return '//'

	def get_value(self):
		return self.evaluate(self.vars[0]) // self.evaluate(self.vars[1])


class Mod(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 5)

	def get_symbol(self):
		return '%'

	def get_value(self):
		return self.evaluate(self.vars[0]) % self.evaluate(self.vars[1])


class Lt(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 6)

	def get_symbol(self):
		return '<'

	def get_value(self):
		return self.evaluate(self.vars[0]) < self.evaluate(self.vars[1])

class Le(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 7)

	def get_symbol(self):
		return '<='

	def get_value(self):
		return self.evaluate(self.vars[0]) <= self.evaluate(self.vars[1])

class Gt(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 8)

	def get_symbol(self):
		return '>'

	def get_value(self):
		return self.evaluate(self.vars[0]) > self.evaluate(self.vars[1])

class Ge(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 9)

	def get_symbol(self):
		return '>='

	def get_value(self):
		return self.evaluate(self.vars[0]) >= self.evaluate(self.vars[1])


class Eq(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 10)

	def get_symbol(self):
		return '=='

	def get_value(self):
		return self.evaluate(self.vars[0]) == self.evaluate(self.vars[1])

class Ne(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 11)

	def get_symbol(self):
		return '!='

	def get_value(self):
		return self.evaluate(self.vars[0]) != self.evaluate(self.vars[1])


class Or(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 23)

	def get_symbol(self):
		return 'or'

	def get_value(self):
		return self.evaluate(self.vars[0]) or self.evaluate(self.vars[1])

class And(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 24)

	def get_symbol(self):
		return '&'

	def get_value(self):
		return self.evaluate(self.vars[0]) & self.evaluate(self.vars[1])

class Xor(BinOp):

	def __init__(self, vars):
		BinOp.__init__(self, vars, 25)

	def get_symbol(self):
		return 'xor'

	def get_value(self):
		return bool(self.evaluate(self.vars[0])) != bool(self.evaluate(self.vars[1]))

class Pow(Call):
	def __init__(self, vars):
		Call.__init__(self, vars, "pow")

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

def AllDiff(*args):
	vars = flatten(args)
	if len(vars) < 2:
		raise BaseException("AllDiff requires a list of at least 2 expressions.")
	return [vars[i] != vars[j] for i in range(len(vars)-1) for j in range(i+1,len(vars))]



class Neg(UnOp):
	def __init__(self, vars):
		UnOp.__init__(self, vars, 2)

class Pos(UnOp):
	def __init__(self, vars):
		UnOp.__init__(self, vars, 1)

class Invert(Predicate):
	def __init__(self, vars):
		UnOp.__init__(self, vars, 0)



# Temporary container for Variable Declaration
class VarDecl(Expression):
	def __init__(self, model):
		if not isinstance(model, Model):
			raise TypeError('Warning: First argumetn must be a Model Object')
		Expression.__init__(self, model)
		self.name = '"' + str(id(self)) + '"'

	def get_value(self):
		if hasattr(self, 'value'):
			return self.value
		else:
			if self.model.is_solved():
				self.value = self.model.mznsolution.getValue(self.name)
				return self.value
			else:
				return None

	def __str__(self):
		return str(self.get_value())

	def __repr__(self):
		return 'A Minizinc Object with the value of ' + self.__str__()

class Construct(VarDecl):
	def __init__(self, model, arg1, arg2 = None):
		VarDecl.__init__(self, model)
		if arg2 != None:
			if type(arg2) is str:
				self.name = arg2
			else: 
				raise TypeError('Name of variable must be a string')
		self.obj = model.mznmodel.Variable(self.name, arg1)
		self.value = arg1

class Set(VarDecl):
	def __init__(self, model, argopt1=None, argopt2=None, argopt3=None):
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
		elif argopt1 is not None:
			if type(argopt1) is list:
				set_list = argopt1
			else:
				ub = argopt2 - 1
				lb = 0
		else:
			raise AttributeError('Set must be initialised with arguments')

		if name is not None:
			if type(name) is not str:
				raise TypeError('Name must be a string');
			else:
				self.name = name
		if set_list is None:
			if not (type(lb) is int and type(ub) is int):
				raise TypeError('Lower bound and upper bound must be integers')
			set_list = [[lb,ub]]
		self.obj = minizinc.Set(set_list)

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

	def __str__(self):
		return self.ob

class Variable(VarDecl):
	def __init__(self, model, arg1=None, arg2=None, arg3=None):
		VarDecl.__init__(self, model)
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
			if typelb not in [bool, int, float] and not isinstance(typelb, VarDecl):
				raise TypeError('Lower bound must be a boolean, an int, a float or a set')
			if typeub not in [bool, int, float] and not isinstance(typelb, VarDecl):
				raise TypeError('Upper bound must be a boolean, an int or a float')
			if typelb != typeub:
				raise TypeError('Upper bound an dlower bound is of different type')
			if lb > ub:
				raise ValueError('Lower bound cannot be greater than upper bound')

		self.dim_list = []
		if name is not None:
			self.name = name

		if typelb is bool:
			self.obj = model.mznmodel.Variable(self.name, 10, [])
		elif typelb is int:
			self.obj = model.mznmodel.Variable(self.name, 9, [], lb, ub)
		elif typelb is float:
			self.obj = model.mznmodel.Variable(self.name, 11, [], lb, ub)
		elif typelb is Set:
			self.obj = model.mznmodel.Variable(self.name, 9, [], lb.obj)
		#elif isinstance(lb, VarDecl) or isinstance(ub, VarDecl):
		#	self.obj = model.mznmodel.Variable(self.name, 12, [], lb, ub)

class VariableConstruct(Variable, Construct):
	def __init__(self, model, arg1, arg2 = None):
		Construct.__init__(self, model, arg1, arg2)

class Array(Variable):
	def __init__(self, model, argopt1, argopt2, *args):
		VarDecl.__init__(self, model)
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
					dim_list.append(i.min(), i.max())
				raise TypeError('Array ranges must be continuous')
			elif isinstance(i, str):
				self.name = i
			else:
				raise RuntimeError('Unknown error')

		if type(argopt1) is Set:
			lb = argopt1
			ub = None
			add_to_dim_list(argopt2)

		for i in args:
			add_to_dim_list(argopt2)
			lb = argopt1
			ub = argopt2

		self.lb = lb
		self.ub = ub
		if dim_list == []:
			raise AttributeError('Initialising an Array without dimension list')
		self.dim_list = dim_list
		tlb = type(argopt1)
		if tlb is bool:
			self.obj = model.mznmodel.Variable(self.name,10,dim_list,lb,ub)
		elif tlb is int:
			self.obj = model.mznmodel.Variable(self.name, 9,dim_list,lb,ub)
		elif tlb is float:  #isinstance(lb, float):
			self.obj = model.mznmodel.Variable(self.name,11,dim_list,lb,ub)
		elif tlb is Set:
			self.obj = model.mznmodel.Variable(self.name, 9,dim_list,lb.obj)

	def __getitem__(self, *args):
		return ArrayAccess(self.model, self, args[0])

class ArrayAccess(Array):
	def __init__(self, model, array, idx):
		VarDecl.__init__(self, model)
		self.array = array
		self.idx = idx

	def get_value(self):
		if hasattr(self,'value'):
			return self.value
		else:
			arrayvalue = self.array.get_value()
			if arrayvalue is None:
				return None
			else:
				for i in self.idx:
					arrayvalue = arrayvalue[i]
				self.value = arrayvalue
				return arrayvalue

class ArrayConstruct(Array, Construct):
	def __init__(self, model, arg1, arg2 = None):
		Construct.__init__(self, model, arg1, arg2)


class Model(object):
	def __init__(self):
		self.loaded = False
		self.mznsolution = None
		self.mznmodel = minizinc.Model()


	# not used anymore
	def get_name(self,var):
		itemList = [var_name for var_name, var_val in self.frame if var_val is var]
		if len(itemList) == 1:
			return itemList[0]
		elif len(itemList) == 0:
			raise LookupError('Internal Error: variable name not found')
		else:
			raise LookupError('The object pointed to was assigned to different names')

	def add(self, *expr):
		minizinc.lock()
		if self.mznmodel == None:
			raise RuntimeError('Model has been solved, need to be reset first')
		if len(expr)>0:
			self.loaded = True
			#self.frame = inspect.currentframe().f_back.f_locals.items()
			self.add_prime(expr)
			#del self.frame
		minizinc.unlock()

	def evaluate(self, expr):
		if not isinstance(expr, Expression):
			return (expr,None)
		if isinstance(expr, Call):
			variables = []
			model = None
			for i in expr.vars:
				var, m = self.evaluate(i)
				if model is None:
					model = m
				elif m is not None and model != m:
					raise TypeError("Objects must be free or belong to the same model")
				variables.append(var)
			return (minizinc.Call(expr.CallCode, variables), model)
		elif isinstance(expr, ArrayAccess):
			return (expr.array.obj.at(expr.idx), expr.model)
		elif isinstance(expr, Variable):
			#if not expr.is_added:
			#	expr.is_added = True
			#	expr.name = self.get_name(expr)
			#	expr.obj = self.mznmodel.Variable(expr.name, expr.VarCode,
			#						expr.dim_list, expr.lb, expr.ub)
			return (expr.obj, expr.model)
		elif isinstance(expr, BinOp):
			lhs, model = self.evaluate(expr.vars[0])
			rhs, model2 = self.evaluate(expr.vars[1])
			if model is None:
				model = model2
			if model2 is not None and model2 != model:
				raise TypeError("Objects must be free or belong to the same model")
			return (minizinc.BinOp(lhs, expr.BinOpCode, rhs), model)
		elif isinstance(expr, UnOp):
			ret, model = self.evaluate(expr.vars[0])
			return (minizinc.UnOp(expr.UnOpCode, self.evaluate(expr.vars[0])), model)
		else:
			raise TypeError('Variable Type unspecified')


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
				if isinstance(expr, bool):
					self.mznmodel.Constraint(expr)
				else:
					raise(TypeError, "Argument must be a minizinc expression or python boolean")

	def Variable(self, argopt1=None, argopt2=None, argopt3=None):
		return Variable(self, argopt1, argopt2, argopt3)
		#var.name = get_name(var)
		#var.obj = m.mznmodel.Variable(var.name, var.code,
		#						var.dim_list, var.lb, var.ub)

	def Array(self, argopt1, argopt2, *args):
		list_ = []
		for i in args:
			list_.append(i)
		return Array(self, argopt1, argopt2, *list_)

	def Set(self, argopt1, argopt2=None, argopt3=None):
		return Set(self, argopt1, argopt2, argopt3)

	def Construct(self, argopt1, argopt2 = None):
		if isinstance(argopt1, list):
			return ArrayConstruct(self, argopt1, argopt2)
		else:
			return VariableConstruct(self, argopt1, argopt2)


	def solve(self):
		if not self.is_loaded():
			raise ValueError('Model is not loaded yet')
		#minizinc.unlock()
		self.is_loaded = False
		self.mznmodel.SolveItem(0)
		self.mznsolution = self.mznmodel.solve()
		self.mznmodel = None

	def optimize(self, arg, code):
		minizinc.lock()
		obj, model = self.evaluate(arg)
		if model is not None and model != self:
			raise TypeError('Expression must be free or belong to the same model')
		self.id_loaded = False
		self.mznmodel.SolveItem(code, obj)
		self.mznsolution = self.mznmodel.solve()
		self.mznmodel = None
		minizinc.unlock()

	def maximize(self, arg):
		self.optimize(arg, 2)
	def minimize(self, arg):
		self.optimize(arg, 1)
	def reset(self):
		self.__init__()

	def next(self):
		if self.mznsolution == None:
			raise ValueError('Model is not solved yet')
		return self.mznsolution.next()

	def is_loaded(self):
		return self.loaded

	def is_solved(self):
		return self.mznsolution != None

import minizinc

class Domain(list):
	def __init__(self, arg1, arg2=None):
		"""
		\internal
		This class is used to wrap the domain of variables
		in order to print them and/or iterate over values

		Initialised from a list of values, or a lower and an upper bound
		"""
		if arg2 is None:
			list.__init__(self, arg1)
			self.sort()
			self.is_bound = False
		else:
			list.__init__(self, [arg1, arg2])
			self.is_bound = True
		self.current = -1

	def next(self):
		"""
		\internal
		Returns the next value when iterating
		"""
		self.current += 1
		if self.is_bound:
			if self[0] + self.current > self[-1]:
				raise StopIteration
			else:
				return self[0] + self.current
		else:
			if self.current >= list.__len__(self):
				raise StopIteration
			else:
				return list.__getitem__(self, self.current)


	def __str__(self):
		"""
		\internal
		"""
		if self.is_bound:
			lb = self[0]
			ub = self[-1]
			if lb + 1 == ub and type(lb) is int:
				return '{' + str(lb) + ',' + str(ub) + '}'
			else:
				return '{' + str(lb) + '..' + str(ub) + '}'

		def extend(idx):
			x = self[idx]
			y = x
			idx += 1
			while idx < len(self):
				if type(self[idx]) is int and self[idx] == y + 1:
					y = self[idx]
				else:
					break
				idx += 1
			return (x, y, idx)

		ret_str = '{'
		idx = 0
		while idx < len(self):
			if idx > 0:
				ret_str += ','
			(x, y, idx) = extend(idx)
			ret_str += str(x)
			if type(x) is int and x + 1 < y:
				ret_str += ('..' + str(y))
			elif x != y:
				ret_str += (',' + str(y))

		return ret_str + '}'


## Base Expression class from which everything inherits
#
#    All variables and constraints are expressions. Variables are
#    just expressions where predicates extend the expression class to add
#    more functionality.
#
class Expression(object):
	def __init__(self, name):
		self.name = name
		self.obj = None
		self.model = None

	def __iter__(self):
		return self.get_domain()

	def initial(self):
		output = self.name()
		if self.domain_ is None:
			output += ' in ' + str(Domain(self.lb, self.ub))
		else:
			output += ' in ' + str(Domain(self.domain_))
		return output

	def name(self):
		if self.is_var():
			return self.name
		else:
			return self.operator

	def domain(self, solver=None):
		output = self.name() + ' in ' + str(self.get_domain())
		return output

	def __str__(self):
		return self.domain()

	def is_str(self):
		if hasattr(self, 'lb'):
			return not numeric(self.lb)
		return False

	def is_solved(self):
		if self == None:
			return False
		else:
			return self.obj.is_solved()

	def has_children(self):
		return hasattr(self, 'children')

	def has_parameters(self):
		return hasattr(self, 'parameters')

	def is_var(self):
		return issubclass(type(self), Variable)

	def get_domain(self):
		if self.domain_ is not None:
			return Domain(self.domain_)
		else:
			return Domain(self.lb, self.ub)

	def get_name(self):
		return self.operator

	def get_children(self):
		if self.has_children():
			return self.children
		else:
			return None

	def get_operator(self):
		return self.operator

	def get_value(self):
		#
		#
		#
		#
		#
		return None

	def __str__(self):
		return 'Expression base string'

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
		var = Sub([self, pred])
		var.name = '(' + str(self) + '-' + str(pred) + ')'
		return var

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
		if issubclass(self, Expression)
			if (isinstance(pred, (int, long, float, str, bool)))

		elif t1 == BinPredicate
			if (t0 in [int, long, float, str, bool])

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

	def __neg__(self):
		return Neg([self])




class Predicate(Expression):

	def __init__(self, op):
		Expression.__init__(self, op)
		#self.set_children(vars);

	def set_children(self, children):
		self.children = flatten(children)

	def initial(self):
		save_str = Expression.__str__
		Expression.__str__ = Expression.initial
		output = self.__str__()
		Expression.__str__ = save_str
		return output

	def name(self):
		return self.__str__()

	def __str__(self):
		save_str = Expression.__str__
		Expression.__str__ = Expression.name
		output = self.operator + "(" + ", ".join(map(str, self.children)) + ")"
		Expression.__str__ = save_str
		return output

	def domain(self, solver=None):
		save_str = Expression.__str__
		Expression.__str__ = lambda x: x.domain(solver)
		output = self.__str__()
		Expression.__str__ = save_str
		return output

class Set:
	def __init__(self, vars):
		#Predicate.__init__(self,"Minizinc Set")
		self.obj = minizinc.Set(vars)

	def __str__(self):
		return str(self.obj)



class BinPredicate(Predicate):

	def __init__(self, vars, op):
		#Predicate.__init__(self, op)
		if !all(issubclass(i,Expression) for i in vars)
			raise TypeError('Objects must be expressions')
		self.obj = minizinc.Expression(vars[0].obj, op, vars[1].obj)
		self.vars = vars
		if vars[0].model != vars[1].model:
			raise TypeError('Variables must be of the same model')
		else:
			self.model = vars[0].model

	def get_symbol(self):
		return 'x'

	def __str__(self):
		save_str = Expression.__str__
		Expression.__str__ = Expression.name
		output = '(' + str(self.vars[0]) + ' ' + self.get_symbol() + ' ' + str(self.vars[1]) + ')'
		Expression.__str__ = save_str
		return output

	def eval(self, x, y):
		try:
			return int(getattr(operator, self.operator)(x,y))
		except AttributeError:
			return int(eval(str(x) + ' ' + self.get_symbol() + ' ' + str(y)))

	def initial(self):
		output = '(' + str(self.vars[0].initial()) + ' ' + self.get_symbol() + ' ' + str(self.vars[1].initial()) + ')'
		return output



class Add(BinPredicate):
	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 0)

	def get_symbol(self):
		return '+'

class Sub(BinPredicate):
	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 1)

	def get_symbol(self):
		return '-'

class Mul(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 2)

	def get_symbol(self):
		return '*'

class Div(BinPredicate):

	def __init__(self, vars) :
		BinPredicate.__init__(self, vars, 3)

	def get_symbol(self):
		return '/'

class FloorDiv(BinPredicate):

	def __init__(self, vars) :
		BinPredicate.__init__(self, vars, 4)

	def get_symbol(self):
		return '//'


class Mod(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 5)

	def get_symbol(self):
		return '%'


class Lt(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 6)

	def get_symbol(self):
		return '<'

class Le(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 7)

	def get_symbol(self):
		return '<='

class Gt(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 8)

	def get_symbol(self):
		return '>'

class Ge(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 9)

	def get_symbol(self):
		return '>='


class Eq(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 10)

	def get_symbol(self):
		return '=='


class Ne(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 11)

	def get_symbol(self):
		return '!='


class Or(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 23)

	def get_symbol(self):
		return 'or'

class And(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 24)

	def get_symbol(self):
		return '&'

class Xor(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 25)

	def get_symbol(self):
		return 'xor'


class Neg(BinPredicate):

	def __init__(self, vars):
		vars[1] = vars[0]
		vars[0] = Variable(vars[1].model, )
		BinPredicate.__init__(self, vars, "neg")

	def __str__(self):
		return '-' + str(self.children[0])

	def decompose(self):
		return [self.children[0] * -1]		

"""
class Sum(Predicate):

    ## Sum constraint constructor
    # @param vars variables to be summed
    # @param coefs list of coefficients ([1,1,..1] by default)
    def __init__(self, vars, coefs=None):
        Predicate.__init__(self, vars, "Sum")

        if coefs is None:
            coefs = [1 for var in self.children]

        self.parameters = [coefs, 0]
        #SDG: initial bounds
        self.lb = sum(c*self.get_lb(i) if (c >= 0) else c*self.get_ub(i) for i,c in enumerate(coefs))
        self.ub = sum(c*self.get_ub(i) if (c >= 0) else c*self.get_lb(i) for i,c in enumerate(coefs))	
"""


class Variable(Expression):

	def __init__(self, argopt0, argopt1=None, argopt2=None, argopt3=None):
		'''
	0	 Variable() :- Binary variable
	1	 Variable(N) :- Variable in the domain of {0, N-1}
	1	 Variable('x') :- Binary variable called 'x'
	2	 Variable(N, 'x') :- Variable in the domain of {0, N-1} called 'x'
	2	 Variable(l,u) :- Variable in the domain of {l, u}
	3	 Variable(l,u, 'x') :- Variable in the domain of {l, u} called 'x'
	1	 Variable(list) :- Variable with domain specified as a list
	2	 Variable(list, 'x') :- Variable with domain specified as a list called 'x'
		'''
		if (isinstance(argopt0, Model) == False):
			raise TypeError("Warning: First argument must be a Model Object")
		domain = None
		lb = False
		ub = True
		name = '"' + str(id(self)) + '"'

		if argopt3 is not None:
			lb = argopt1
			ub = argopt2
			name = argopt3
		elif argopt2 is not None:
			if type(argopt2) is str:
				if numeric(argopt1):
					ub = argopt1 - 1
					lb = type(ub)(lb)  # Ensure lb has the same datatype as ub
				else:
					domain = sorted(argopt1)
					lb = domain[0]
					ub = domain[-1]
				name = argopt2
			else:
				lb = argopt1
				ub = argopt2
		elif argopt1 is not None:
			if type(argopt1) is str:
				name = argopt1
			elif numeric(argopt1):
				ub = argopt1 - 1
				lb = type(ub)(lb)  # Ensure lb has the same datatype as ub
			else:
				domain = sorted(argopt1)
				lb = domain[0]
				ub = domain[-1]

		tlb = type(lb)
		tub = type(ub)
		if tlb not in [int, long, float, str]:
			raise TypeError("Warning lower bound of %s is not an int or a float or a string" % name)
		elif tub not in [int, long, float, str]:
			raise TypeError("Warning upper bound of %s is not an int or a float or a string" % name)
		elif type(name) is not str:
			raise TypeError("Warning name variable is not a string")
		elif lb > ub:
			raise ValueError("Warning lower bound (%r) of %s greater than upper bound (%r)" % (lb, name, ub))

		Expression.__init__(self, name)
		self.domain = domain
		self.lb = lb
		self.ub = ub
		self.TypeId = 1
		self.obj = argopt0.mznmodel.Variable(name,9,0,lb,ub);
		self.model = argopt0

	def name(self):
		return id(self)

	def initial(self):
		return self.name



class Model(object):
	def __init__(self, *expr):
		self.loaded = False
		self.mznsolution = None
		self.mznmodel = minizinc.Model()
		minizinc.lock()
		if len(expr) > 0:
			self.loaded = True
			self.add_prime(expr)

	def add(self, *expr):
		self.loaded = True
		self.add_prime(expr)

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
			if (not expr.is_var):
				if expr.model != self:
					raise TypeError('Expressions must belong to this model')
				self.mznmodel.Constraint(expr.obj)
			else:
				raise(RuntimeError, "Unexpected type")

	def Variable(self, argopt1=None, argopt2=None, argopt3=None):
		return Variable(self, argopt1, argopt2, argopt3)

	def solve(self):
		if not isLoaded:
			raise ValueError('Model is not loaded yet')
		minizinc.unlock()
		self.mznmodel.SolveItem(0)
		self.mznsolution = self.mznmodel.solve()

	def next(self):
		if self.mznsolution == None:
			raise ValueError('Model is not solved yet')
		return self.mznsolution.next()

	def is_loaded(self):
		return self.loaded

	def is_solved(self):
		return self.mznsolution != None
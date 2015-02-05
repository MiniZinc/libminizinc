##@mainpage MiniZinc Python Interface
# @author	Tai Tran, under the supervision of Guido Tack
#
# \section acknowlegment_sec Acknowledgement
#
# This Interface is based on Numberjack Python Interface


import minizinc
#import inspect

def flatten(x):
    result = []
    for el in x:
        if hasattr(el, "__iter__") and not isinstance(el, basestring) and not issubclass(type(el), Expression):
            result.extend(flatten(el))
        else:
            result.append(el)
    return result

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
		#self.name = self.get_name(self)
		#print self.name
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
		return str(self.get_value())

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
		return issubclass(type(self), VarDecl)

	def is_pre(self):
		return issubclass(type(self), Predicate)

	def get_domain(self):
		if self.domain_ is not None:
			return Domain(self.domain_)
		else:
			return Domain(self.lb, self.ub)

	#def get_name(self):
	#	return self.operator

	def get_children(self):
		if self.has_children():
			return self.children
		else:
			return None

	def get_operator(self):
		return self.operator

	#def get_value(self):
	#	return None

	def evaluate(self, var):
		if isinstance(var, Expression):
			ret = var.get_value()
			if ret == None:
				raise ValueError('Variable value it not set')
			return ret
		else:
			return var

#	def __str__(self):
#		return 'Expression base string'

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


class Set(Expression):
	def __init__(self, vars):
		#Predicate.__init__(self,"Minizinc Set")
		self.obj = minizinc.Set(vars)

	def __str__(self):
		return str(self.obj)


class Predicate(Expression):

	def __init__(self, vars, op):
		'''
		for i in range(len(vars)):
			if isinstance(vars[i], Expression):
				if model == None:
					model = vars[i].model
				elif model != vars[i].model:
					raise TypeError("Objects must be free or belong to the same model")
				self.__exp.append(vars[i].obj)
			else:
				self.__exp.append(vars[i])
		'''
		self.vars = vars
		self.model = None

		#self.obj = minizinc.UnOp(op, self.exp[0])
		self.UnOp = op

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


class BinPredicate(Predicate):

	def __init__(self, vars, op):
		#Predicate.__init__(self, vars, op)
		self.BinOp = op
		self.model = None
		self.vars = vars
		#self.obj = minizinc.BinOp(self.exp[0], op, self.exp[1])

	def get_symbol(self):
		return 'x'


	def eval(self, x, y):
		try:
			return int(getattr(operator, self.operator)(x,y))
		except AttributeError:
			return int(eval(str(x) + ' ' + self.get_symbol() + ' ' + str(y)))

	def initial(self):
		output = '(' + str(self.vars[0].initial()) + ' ' + self.get_symbol() + ' ' + str(self.vars[1].initial()) + ')'
		return output

class Call(Predicate):

	def __init__(self, vars, callId):
		self.model = None
		self.callId = callId
		self.vars = vars




class Add(BinPredicate):
	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 0)

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

class Sub(BinPredicate):
	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 1)

	def get_symbol(self):
		return '-'

	def get_value(self):
		return self.evaluate(self.vars[0]) - self.evaluate(self.vars[1])

class Mul(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 2)

	def get_symbol(self):
		return '*'

	def get_value(self):
		return self.evaluate(self.vars[0]) * self.evaluate(self.vars[1])

class Div(BinPredicate):

	def __init__(self, vars) :
		BinPredicate.__init__(self, vars, 3)

	def get_symbol(self):
		return '/'

	def get_value(self):
		return self.evaluate(self.vars[0]) / self.evaluate(self.vars[1])

class FloorDiv(BinPredicate):

	def __init__(self, vars) :
		BinPredicate.__init__(self, vars, 4)

	def get_symbol(self):
		return '//'

	def get_value(self):
		return self.evaluate(self.vars[0]) // self.evaluate(self.vars[1])


class Mod(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 5)

	def get_symbol(self):
		return '%'

	def get_value(self):
		return self.evaluate(self.vars[0]) % self.evaluate(self.vars[1])


class Lt(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 6)

	def get_symbol(self):
		return '<'

	def get_value(self):
		return self.evaluate(self.vars[0]) < self.evaluate(self.vars[1])

class Le(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 7)

	def get_symbol(self):
		return '<='

	def get_value(self):
		return self.evaluate(self.vars[0]) <= self.evaluate(self.vars[1])

class Gt(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 8)

	def get_symbol(self):
		return '>'

	def get_value(self):
		return self.evaluate(self.vars[0]) > self.evaluate(self.vars[1])

class Ge(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 9)

	def get_symbol(self):
		return '>='

	def get_value(self):
		return self.evaluate(self.vars[0]) >= self.evaluate(self.vars[1])


class Eq(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 10)

	def get_symbol(self):
		return '=='

	def get_value(self):
		return self.evaluate(self.vars[0]) == self.evaluate(self.vars[1])

class Ne(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 11)

	def get_symbol(self):
		return '!='

	def get_value(self):
		return self.evaluate(self.vars[0]) != self.evaluate(self.vars[1])


class Or(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 23)

	def get_symbol(self):
		return 'or'

	def get_value(self):
		return self.evaluate(self.vars[0]) or self.evaluate(self.vars[1])

class And(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 24)

	def get_symbol(self):
		return '&'

	def get_value(self):
		return self.evaluate(self.vars[0]) & self.evaluate(self.vars[1])

class Xor(BinPredicate):

	def __init__(self, vars):
		BinPredicate.__init__(self, vars, 25)

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

class abort(Call):
	def __init__(self):
		Call.__init__(self, [], "abort")

class abs(Call):
	def __init__(self, var):
		Call.__init__(self, var, "abs")

class acosh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"acosh")

class asin(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"asin")

class atan(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"atan")

class assert_(Call):
	def __init__(self, constraint, message):
		Call.__init__(self,[constraint, message],"assert")

class bool2int(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"bool2int")

class card(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"card")

class ceil(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ceil")

class concat(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"concat")

class cos(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"cos")

class Cosh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"cosh")

class dom(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"Dom")

class dom_array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom_array")

class dom_size(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"dom_size")

class fix(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"fix")

class exp(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"exp")

class floor(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"floor")

class exp(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"exp")

class int2float(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"int2float")

class is_fixed(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"is_fixed")

class join(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"join")

class lb(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"lb")

class lb_array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"lb_array")

class length(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"length")

class ln(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ln")

class log(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"log")

class log2(Call):
	def __init__(self, var1, var2):
		Call.__init__(self,[var1, var2],"log2")

class log10(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"log10")

class min(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"min")

class max(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"max")

class product(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"product")

class round(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"round")

class set2array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"set2array")

#class show(Call):
#	def __init__(self, var):
#		Call.__init__(self,[var],"show")

class sin(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sin")

class sinh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sinh")

class sqrt(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sqrt")

class sum(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"sum")

class tan(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"tan")

class tanh(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"tanh")

class trace(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"trace")

class ub(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ub")

class ub_array(Call):
	def __init__(self, var):
		Call.__init__(self,[var],"ub_array")

def AllDiff(*args):
	vars = flatten(args)
	if len(vars) < 2:
		raise BaseException("AllDiff requires a list of at least 2 expressions.")
	return [vars[i] != vars[j] for i in range(len(vars)-1) for j in range(i+1,len(vars))]



class Neg(Predicate):

	def __init__(self, vars):
		Predicate.__init__(self, vars, 2)
	#def __str__(self):
	#	return '-' + str(self.children[0])


	#def decompose(self):
	#	return [self.children[0] * -1]		


class Pos(Predicate):

	def __init__(self, vars):
		Predicate.__init__(self, vars, 1)
	#def __str__(self):
	#	return '-' + str(self.children[0])

class Invert(Predicate):

	def __init__(self, vars):
		Predicate.__init__(self, vars, 0)
	#def __str__(self):
	#	return '-' + str(self.children[0])

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

class VarDecl(Expression):
	def __init__(self, model):
		if isinstance(model, Model) == False:
			raise TypeError("Warning: First argument must be a Model Object")
		self.model = model
		self.name = '"' + str(id(self)) + '"'

	def get_value(self):
		if hasattr(self,'value'):
			return self.value
		else:
			if self.model.is_solved():
				self.value = self.model.mznsolution.getValue(self.name)
				return self.value
			else:
				return None

	def __str__(self):
		temp = self.get_value()
		if temp == None:
			return "VALUE NOT SET"
		else:
			return str(temp)

	def __repr__(self):
		return 'A Minizinc Object with the value of ' + self.__str__()


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

	def get_value(self):
		return self.obj

	def __str__(self):
		return self.obj

class Variable(VarDecl):

	def __init__(self, model, argopt1=None, argopt2=None, argopt3=None):
		VarDecl.__init__(self, model)
		domain = None
		lb = False
		ub = True
		#self.name = get_name(self)
		#get_name(self)
		name = None

		if argopt3 is not None:
			lb = argopt1
			ub = argopt2
			name = argopt3
		elif argopt2 is not None:
			if type(argopt2) is str:
				if type(argopt1) is (list, tuple):
					domain = sorted(argopt1)
					lb = domain[0]
					ub = domain[-1]
				elif isinstance(argopt1, Set):
					lb = argopt1.min()
					ub = argopt1.max()
				else:
					raise TypeError('Undefined syntax')
				name = argopt2
			else:
				lb = argopt1
				ub = argopt2
		elif argopt1 is not None:
			if type(argopt1) is str:
				name = argopt1
			elif type(argopt1) in [int, long, float]:
				ub = argopt1 - 1
				lb = type(ub)(lb)  # Ensure lb has the same datatype as ub
			elif isinstance(argopt1,Set):
				ub = argopt1.max()
				lb = argopt1.min()
			else:
				domain = sorted(argopt1)
				lb = domain[0]
				ub = domain[-1]

		tlb = type(lb)
		tub = type(ub)
		if tlb not in [bool, int, long, float, str]:
			raise TypeError("Warning lower bound of %s is not a boolean, an int, a float or a string" % name)
		elif tub not in [bool, int, long, float, str]:
			raise TypeError("Warning upper bound of %s is not a boolean, an int, a float or a string" % name)
		elif tlb != tub:
			raise TypeError("Warning lower bound and upper bound value are not of the same type")
		elif name is not None and type(name) is not str:
			raise TypeError("Warning name variable is not a string")
		elif lb > ub:
			raise ValueError("Warning lower bound (%r) of %s greater than upper bound (%r)" % (lb, name, ub))
		self.domain = domain
		self.lb = lb
		self.ub = ub
		self.dim_list = []
		if name is not None:
			self.name = name
		if tlb is bool:
			self.obj = model.mznmodel.Variable(self.name,10,[],lb,ub)
			#self.VarCode = 10
		elif tlb is int:
			self.obj = model.mznmodel.Variable(self.name, 9,[],lb,ub)
			#self.VarCode = 9
		elif tlb is float:  
			self.obj = model.mznmodel.Variable(self.name,11,[],lb,ub)
			#self.VarCode = 11
		elif isinstance(lb, Variable) or isinstance(ub, Variable):
			self.obj = model.mznmodel.Variable(self.name,12,[],lb,ub)
			#self.VarCode = 12
		#self.is_added = False

	def name(self):
		return id(self)

	def initial(self):
		return self.name

	def set_value(self, arg):
		self.obj.setValue(arg)


class VarConstruct(Variable):

	def __init__(self, model, argopt1):
		VarDecl.__init__(self, model)
		self.obj = model.mznmodel.Variable(self.name, argopt1)
		self.value = argopt1

class Array(Variable):

	def __init__(self, model, argopt1, argopt2=None, *args):
		VarDecl.__init__(self, model)
		dim_list = []
		for i in args:
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
				if i.is_continuous():
					dim_list.append(i.min(), i.max())
				raise TypeError('Array ranges must be continuous')
			elif isinstance(i, str):
				self.name = i
			else:
				raise RuntimeError('Unknown error')
		lb = argopt1
		ub = argopt2
		self.lb = lb
		self.ub = ub
		self.dim_list = dim_list
		#self.is_added = False
		tlb = type(argopt1)
		if tlb is bool:
			self.obj = model.mznmodel.Variable(self.name,10,dim_list,lb,ub)
			#self.VarCode = 10
		elif tlb is int:
			self.obj = model.mznmodel.Variable(self.name, 9,dim_list,lb,ub)
			#self.VarCode = 9
		elif tlb is float:  #isinstance(lb, float):
			self.obj = model.mznmodel.Variable(self.name,11,dim_list,lb,ub)
			#self.VarCode = 11

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
			return (minizinc.Call(expr.callId, variables), model)
		elif isinstance(expr, ArrayAccess):
			return (expr.array.obj.at(expr.idx), expr.model)
		elif isinstance(expr, Variable):
			#if not expr.is_added:
			#	expr.is_added = True
			#	expr.name = self.get_name(expr)
			#	expr.obj = self.mznmodel.Variable(expr.name, expr.VarCode,
			#						expr.dim_list, expr.lb, expr.ub)
			return (expr.obj, expr.model)
		elif isinstance(expr, BinPredicate):
			lhs, model = self.evaluate(expr.vars[0])
			rhs, model2 = self.evaluate(expr.vars[1])
			if model is None:
				model = model2
			if model2 is not None and model2 != model:
				raise TypeError("Objects must be free or belong to the same model")
			return (minizinc.BinOp(lhs, expr.BinOp, rhs), model)
		elif isinstance(expr, Predicate):
			ret, model = self.evaluate(expr.vars[0])
			return (minizinc.UnOp(expr.UnOp, self.evaluate(expr.vars[0])), model)
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

	def VarConstruct(self, argopt1):
		return VarConstruct(self, argopt1)


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
from minizinc import *
from predicate import *
from annotation import *
model = Model()

N = 8
S = model.Set(1,N)


q = model.Array(1,N,S)

model.Constraint( alldifferent(q) )
model.Constraint( alldifferent([q[i]+i for i in S]) )
model.Constraint( alldifferent([q[i]-i for i in S]) )

model.satisfy( int_search(q, first_fail, indomain_min, complete) )
while (model.next()):
	for i in S:
		for j in S:
			if (q[j]).get_value() == i:
				print 'Q',
			else:
				print '*',
			if j == N:
				print '\n',
	print '\n',
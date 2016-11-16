from minizinc import *

m = Model()

R = m.Variable(-100.0,100000.0)
P = m.Variable(-100.0,100000.0)
I = m.Variable(0.0,10.0)


B1 = m.Variable(-100.0,100000.0)
B2 = m.Variable(-100.0,100000.0)
B3 = m.Variable(-100.0,100000.0)
B4 = m.Variable(-100.0,100000.0)


m.Constraint( B1 == P * (1.0 + I) - R,
	   B2 == B1 * (1.0 + I) - R,
	   B3 == B2 * (1.0 + I) - R,
	   B4 == B3 * (1.0 + I) - R,
	 )

m.satisfy(data = (I == 0.04, P == 1000.0, R == 260.0) );
m.next();
print 'Borrowing',P,' at',I.get_value()*100.0,r'% interest, and repaying',R
print ' per quarter for 1 year leaves,',B4, ' owing'
print ' '
print ' '


m.satisfy(data = (I == 0.04, P == 1000.0, B4 == 0.0) );
m.next();
print 'Borrowing',P,' at',I.get_value()*100.0,r'% interest, and repaying',R
print ' per quarter for 1 year leaves,',B4, ' owing'
print ' '
print ' '


m.satisfy(data = (I == 0.04, R == 250.0, B4 == 0.0) );
m.next();
print 'Borrowing',P,' at',I.get_value()*100.0,r'% interest, and repaying',R
print ' per quarter for 1 year leaves,',B4, ' owing'
print ' '
print ' '
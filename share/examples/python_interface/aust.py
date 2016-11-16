from minizinc import *
import sys

m = Model()

N = 3

wa,nsw,nt,v,sa,t,q = [m.Variable(1,N) for i in range(7)]

m.Constraint( wa != nt, wa != sa, nt != sa, nt != q, sa != q, sa != nsw, sa != v, q  != nsw, nsw != v)

m.satisfy()
m.next()


#if sys.version >= '3':
#print('wa =',wa,'\t nt =', nt, '\t sa =', sa)
#print('q =', q, '\t nsw =', nsw, '\t v =', v)
#print('t =', t)
#else:
print 'wa =', wa, '\t nt =', nt, '\t sa =', sa
print 'q =', q, '\t nsw =', nsw, '\t v =', v
print 't =', t, '\n'
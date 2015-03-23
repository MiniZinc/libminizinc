# A political problem

# Suppose that you are a politician trying to win an election. Your district has three
# different types of areas - urban, suburban, and rural. These areas have, respectively,
# 100,000, 200,000, and 50,000 registered voters. Although not all the registered voters
# actually go to the polls, you decide that to govern effectively, you would like at least
# half the registered voters in each of the three regions to vote for you. You are honorable
# and would never consider supporting policies in which you do not believe. You realize,
# however, that certain issues may be more effective in winning votes in certain places.
# Your primary issues are building more roads, gun control, farm subsidies, and a gasoline tax
# dedicated to improved public transit.
# According to your campaign staff's research, you can estimate how many votes you win or lose
# from each population segment by spending $1,000 on advertising on each issue.
#		POLICY		|	URBAN 	SUBURBAN	RURAL
#		-----------------------------------------
#		build roads	|	   -2 		   5		3
#		gun control	|		8		   2	   -5
# 	 farm subsidies |		0		   0	   10
#	   gasoline tax |	   10		   0	   -2
#
# In this table, each entry indicates the number of thousands of either urban, suburban,
# or rural voters who would be won over by spending $1,000 on advertising in support of
# a particular issue. Negative entries denote votes that would be lost.
# Your task is to figure out the minimum amount of mony that you need to spend in order to win
# 50,000 urban votes, 10,000 suburban votes, and 25,000 rural votes.


from minizinc import *

m = Model()

x1 = m.Variable(0.0,10000.0)
x2 = m.Variable(0.0,10000.0)
x3 = m.Variable(0.0,10000.0)
x4 = m.Variable(0.0,10000.0)

m.Constraint( -2.0*x1 + 8.0*x2 +  0.0*x3 + 10.0*x4 >= 50.0,
               5.0*x1 + 2.0*x2 +  0.0*x3 +  0.0*x4 >= 100.0,
               3.0*x1 - 5.0*x2 + 10.0*x3 -  2.0*x4 >= 25.0,
            )

m._debugprint()
m.mznmodel.set_time_limit(10)

m.minimize(x1+x2+x3+x4)

m.next()

print "Minimize cost = ", x1 + x2 + x3 + x4
print "where x1 = ", x1
print "      x2 = ", x2
print "      x3 = ", x3
print "      x4 = ", x4

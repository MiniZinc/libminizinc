from minizinc import *


m = Model()
n = 20
capacity = 50
Items = m.Set(n)

profits = [4,7,3,9,3,8,3,8,2,7,9,2,6,2,8,6,4,8,4,3]
weights = [6,3,7,3,7,9,3,5,1,6,2,6,1,4,2,7,3,2,5,1]

knapsack = m.VarSet(Items)

m.Constraint(Sum([Bool2Int(In(i, knapsack))*weights[i] for i in Items]) <= capacity)

m.maximize(Sum([Bool2Int(In(i, knapsack))*weights[i] for i in Items]))

m.next()

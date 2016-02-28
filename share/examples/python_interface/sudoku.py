from interface import *

m = Model()

S = 3
N = S * S

PuzzleRange = m.Set(1,N)
SubSquareRange = m.Set(1,S)

start = [ [0, 0, 0, 0, 0, 0, 0, 0, 0],
		  [0, 6, 8, 4, 0, 1, 0, 7, 0],
		  [0, 0, 0, 0, 8, 5, 0, 3, 0],
		  [0, 2, 6, 8, 0, 9, 0, 4, 0],
		  [0, 0, 7, 0, 0, 0, 9, 0, 0],
		  [0, 5, 0, 1, 0, 6, 3, 2, 0],
		  [0, 4, 0, 6, 1, 0, 0, 0, 0],
		  [0, 3, 0, 2, 0, 7, 6, 9, 0],
		  [0, 0, 0, 0, 0, 0, 0, 0, 0] ]
puzzle = m.Array(PuzzleRange,PuzzleRange,PuzzleRange)

for i in PuzzleRange:
	for j in PuzzleRange:
		if start[i-1][j-1] > 0:
			m.Constraint(puzzle[i,j] == start[i-1][j-1])




for i in PuzzleRange:
	m.Constraint(AllDiff([puzzle[i,j] for j in PuzzleRange]))

for j in PuzzleRange:
	m.Constraint(AllDiff([puzzle[i,j] for i in PuzzleRange]))

for a in SubSquareRange:
	for o in SubSquareRange:
		m.Constraint(AllDiff([puzzle[(a-1)*S + a1, (o-1)*S + o1] for a1 in SubSquareRange for o1 in SubSquareRange]))

m.satisfy()
m.next()

for i in PuzzleRange:
	for j in PuzzleRange:
		print puzzle[i,j],' ',
		if (j % S) == 0:
			print ' ',
		if j == N and i != N:
			if (i % S) == 0:
				print '\n\n',
			else:
				print '\n',


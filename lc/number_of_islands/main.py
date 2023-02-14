#!/usr/bin/env python3
import collections

def print_grid(grid: list[list[str]]):
	for i in grid:
		for j in i:
			print('{}'.format(j), end='')
		print('');

class Solution:
	@staticmethod
	def numIslands(grid: list[list[str]]) -> int:
		if not grid:
			return 0

		rows = len(grid)
		cols = len(grid[0])
		visit = set()
		islands = 0

		def bfs(r,c):
			q = collections.deque()
			visit.add((r,c))
			q.append((r,c))

			while q:
				row, col = q.popleft()
				nei = [[1,0], [-1, 0], [0,1], [0, -1]]
				for x, y in nei:
					i = row + x
					# check ranges
					if i < 0 or i >= rows:
						continue
					j = col + y
					if j < 0 or j >= cols:
						continue
					# check if visited
					if (i, j) in visit:
						continue
					if grid[i][j] == '1':
						q.append((i, j))
						visit.add((i,j))

		for r in range(rows):
			for c in range(cols):
				if (r, c) not in visit and grid[r][c] == '1':
					bfs(r, c)
					islands += 1

		return islands



grid = [
['1', '1', '1', '1', '0'],
['1', '1', '0', '1', '0'],
['1', '1', '0', '0', '0'],
['0', '0', '0', '0', '1']
]

res = Solution.numIslands(grid)
print_grid(grid)
print('result: {}'.format(res))
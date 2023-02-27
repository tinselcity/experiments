#!/usr/bin/env python3
dirX = 0
dirY = 1
print('heading: {}, {}'.format(dirX, dirY))
for i in range(4):
	# on move
	oldY = dirY
	dirY = dirX
	dirX = -1*oldY
	print('heading: {}, {}'.format(dirX, dirY))

class Solution:
	@staticmethod
	def isRobotBounded(instructions: str) -> bool:
		dirX = 0
		dirY = 1
		x = 0
		y = 0

		for d in instructions:
			if d == 'G':
				x = x+dirX
				y = y+dirY
			elif d == 'L':
				# on move
				oldY = dirY
				dirY = dirX
				dirX = -1*oldY
		if (x, y) == (0, 0):
			return True
		if (dirX, dirY) != (0, 1):
			return True
		return False
	
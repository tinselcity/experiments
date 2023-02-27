#!/usr/bin/env python3
import collections

class Solution:
	@staticmethod
	def ladderLength(beginWord: str, endWord: str, wordList: list[str]) -> int:
		if endWord not in wordList:
			return 0

		nei = collections.defaultdict(list)
		wordList.append(beginWord)
		for word in wordList:
			for j in range(len(word)):
				pattern = word[:j] + "*" + word[j+1:]
				nei[pattern].append(word)

		visit = set([beginWord])
		q = collections.deque([beginWord])
		res = 1
		while q:
			for i in range(len(q)):
				word = q.popleft()
				print('word: {}'.format(word))
				if word == endWord:
					return res
				for j in range(len(word)):
					pattern = word[:j] + "*" + word[j+1:]
					print('PTTN: ** {} **'.format(pattern))
					for neiWord in nei[pattern]:
						print('PTTN: {}'.format(neiWord))
						if neiWord not in visit:
							visit.add(neiWord)
							q.append(neiWord)
			print('add')
			res += 1
		return res

l_dict = [
'hot',
'dot',
'dog',
'lot',
'log',
'cog'
]

l_res = Solution.ladderLength('hit', 'cog', l_dict)
print(l_res)
#!/usr/bin/env python3

class Solution:
	@staticmethod
	def alienOrder(words: list[str]) -> str:
		adj = { c:set() for w  in words for c in w }
		for i in range(len(words) - 1):
			w1, w2 = words[i], words[i+1]
			minLen = min(len(w1), len(w2))
			if w1[:minLen] == w2[:minLen] and len(w1) > len(w2):
				return ''
			for j in range(minLen):
				if w1[j] != w2[j]:
					adj[w1[j]].add(w2[j])
					break

		visit = {}
		res = []

		def dfs(c):
			if c in visit:
				return visit[c]
			print('DFS -STAMP-: {}'.format(c))
			visit[c] = True
			for nei in adj[c]:
				if dfs(nei):
					return True
			visit[c] = False
			res.append(c)

		for c in adj:
			print('DFS <START>: {}'.format(c))
			if dfs(c):
				return ''

		res.reverse()
		return ''.join(res)


l_dict = [
'wrt',
'wrf',
'er',
'ett',
'rftt'
]
l_res = Solution.alienOrder(l_dict)
print(l_res)
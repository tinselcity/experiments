#!/usr/bin/env python3

def solveNQueens(n: int) -> list[list[str]]:
    col = set()
    posDiag = set()
    negDiag = set()

    res = []
    board = [['☐'] * n for i in range(n)]

    def dfs(r):
        if r == n:
            copy = [''.join(row) for row in board]
            res.append(copy)
            return
        
        for c in range(n):
            if c in col or (r+c) in posDiag or (r-c) in negDiag:
                continue

            col.add(c)
            posDiag.add(r+c)
            negDiag.add(r-c)
            board[r][c] = '✓'

            dfs(r+1)

            col.remove(c)
            posDiag.remove(r+c)
            negDiag.remove(r-c)
            board[r][c] = '☐'

    dfs(0)
    return res

res = solveNQueens(n=10)
print('\n'.join(res[0]))

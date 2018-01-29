/*

This algorithm is optimal only for (3,3,3) game.

The variable rows, cols and count must not be changed!

*/


#include "NacEngine.h"
#include <time.h>
#include <stdlib.h>


NacEngine::NacEngine(const int & rows, const int & cols, const int & count)
{
	m_nRows = rows;
	m_nCols = cols;
	m_nCount = count;

	init();
}


NacEngine::~NacEngine()
{
}

// initialize
void NacEngine::init()
{
	for (int i = 0; i < m_nRows; i++)
	{
		for (int j = 0; j < m_nCols; j++)
			m_nCellsState[i][j] = Empty;
	}

	m_nTotalCount = 0;
}

// human's play
NacEngine::PlayResult NacEngine::PlayHuman(const int & row, const int & col)
{
	PlayResult result = Waiting;

	m_nTotalCount++;					// count steps.
	m_nCellsState[row][col] = Human;	// the cell (row, col) is occupied by human

	if (DecideWin(Human))				// check if human wins
		result = HumanWin;

	if (result == Waiting && !Playable())	// if the winner is still undefined and game is not able to continue, this game is a draw. 
		result = Draw;

	return result;
}

// return number of corner opposite to 'cornerNo'
// cell's number is aligned starting from 1.
// that is, ...
//   ----------
//   |  |  |  |
//   | 1| 2| 3|
//   ----------
//   |  |  |  |
//   | 4| 5| 6|
//   ----------
//   |  |  |  |
//   | 7| 8| 9|
//   ----------
int NacEngine::GetOppositeCorner(const int cornerNo)
{
	return 10 - cornerNo;
}

// computer's play
NacEngine::PlayResult NacEngine::PlayCom(int & row, int & col)
{
	PlayResult result = Waiting;
	m_nTotalCount++;

	// Initialize
	// The early moves are performed manually, because in the early of the game an optimal scenario is not able to be looked for 
	// due to lack of decision conditions.
	if (m_nTotalCount == 1)	// if the computer plays first, it marks the cell of random corner.
	{
		int cornerNo = GenRandCornerNum();
		row = (cornerNo - 1) / m_nCols;
		col = (cornerNo - 1) % m_nCols;
		m_nCellsState[row][col] = Com;
		m_nPrevCellNo = cornerNo;
		return Waiting;
	}
	if (m_nTotalCount == 2) // if human plays first
	{
		if (m_nCellsState[1][1] == Empty)	// if human didn't mark the center first, computer marks it.
		{
			row = 1;
			col = 1;
			m_nCellsState[1][1] = Com;
			m_nPrevCellNo = 5;
		}
		else   // if human marked the center first, it marks the cell of random corner.
		{
			int cornerNo = GenRandCornerNum();
			row = (cornerNo - 1) / m_nCols;
			col = (cornerNo - 1) % m_nCols;
			m_nCellsState[row][col] = Com;
			m_nPrevCellNo = cornerNo;
		}

		return Waiting;
	}
	if (m_nTotalCount == 3) // if computer plays first and this is 2nd move
	{
		if (m_nCellsState[1][1] == Human)	// if human marked the center, the opposite corner to previous marked coner is marked.
		{
			int corno = GetOppositeCorner(m_nPrevCellNo);
			row = (corno - 1) / m_nCols;
			col = (corno - 1) % m_nCols;
			m_nCellsState[row][col] = Com;
		}
		else    // if human didn't mark the center ...
		{
			int rr, cc, id;
			for (id = 2; id <= 8; id = id + 2)   // if human marked one of the 2, 4, 6, 8th cells, the center is marked.
			{
				rr = (id - 1) / m_nCols;
				cc = (id - 1) % m_nCols;
				if (m_nCellsState[rr][cc] == Human)
				{
					row = 1;
					col = 1;
					m_nCellsState[1][1] = Com;
					break;
				}
			}
			if (id > 8)				// if human marked nothing of the 2, 4, 6, 8th cells ...
			{                       // if human marked the opposite corner to previous marked corner, 
				                    // one of the other corners is marked. 
									// if not, marked is the opposite corner to the corner marked by human
				int corno = GetOppositeCorner(m_nPrevCellNo);  
				rr = (corno - 1) / m_nCols;
				cc = (corno - 1) % m_nCols;
				int fno = corno > m_nPrevCellNo ? m_nPrevCellNo : corno;
				fno = 4 - fno;
				int rr2, cc2;
				rr2 = (fno - 1) / m_nCols;
				cc2 = (fno - 1) % m_nCols;

				if (m_nCellsState[rr2][cc2] == Empty)
				{
					row = rr2;
					col = cc2;
					m_nCellsState[rr2][cc2] = Com;
				}
				else
				{
					int ffno = GetOppositeCorner(fno);
					rr2 = (ffno - 1) / m_nCols;
					cc2 = (ffno - 1) % m_nCols;
					row = rr2;
					col = cc2;
					m_nCellsState[rr2][cc2] = Com;
				}
			}
		}

		return Waiting;
	}


	// Win : If the player has two in a line, they should complete the line to win the game.
	CellItems * pCellItems = GetCompletableCells(Com);  // check if computer has two in a row.
														// if it has, it marks one of returned list, and will win.
	if (pCellItems->GetCount() > 0)
	{
		PCELL pCell = pCellItems->GetItem(0);
		row = pCell->row;
		col = pCell->col;
		m_nCellsState[row][col] = Com;		// cell (row, col) is occupied by computer.
		delete pCellItems;

		return ComWin;
	}

	// Block : If the opponent has two in a line, the player must play the third point to block the opponent from winning. 
	delete pCellItems;
	pCellItems = GetCompletableCells(Human);	// check if human has two in a row.
												// if he has, computer must mark one of returned list.
												// if the number of cell of the returned list are more than one, human wins.
	if (pCellItems->GetCount() > 0)
	{
		PCELL pCell = pCellItems->GetItem(0);
		row = pCell->row;
		col = pCell->col;
		if (pCellItems->GetCount() > 1)
			result = HumanWin;
		m_nCellsState[row][col] = Com;
		delete pCellItems;

		if (result == Waiting && !Playable())	// if the winner is still undefined and game is not able to continue, this game is a draw. 
			result = Draw;

		return result;
	}

	// Create fork : Create an opportunity where the player has two threats to win.
	delete pCellItems;
	pCellItems = GetPWinableCells(Com);  // check if computer is able to create fork.
								// return value is the list of cells to mark at that time.
								// that is, if it marks the cell of returned list, it will create fork.
								// if is has, it mark the cell, and will be a winner.
	if (pCellItems->GetCount() > 0)
	{
		PCELL pCell = pCellItems->GetItem(0);
		row = pCell->row;
		col = pCell->col;
		delete pCellItems;
		m_nCellsState[row][col] = Com;
		return ComWin;
	}
		

	// The player should create two in a line to force the opponent into defending, 
	// as long as this does not result in the opponent thereby creating a fork.
	// If there is a configuration where the opponent can create a fork, the player should block that fork.
	int nTmp[10][10];
	for (int bi = 0; bi < m_nRows; bi++)
	{
		for (int bj = 0; bj < m_nCols; bj++)
			nTmp[bi][bj] = 0;
	}

	int i, j;
	for (i = 0; i < m_nRows; i++)
	{
		for (j = 0; j < m_nCols; j++)
		{
			if (m_nCellsState[i][j] != Empty)
				continue;

			m_nCellsState[i][j] = Com;   // if this cell is marked by computer next
			
			CellItems * pOpCellItems = GetCompletableCells(Com);  // check if the computer will have two in a row.
			if (pOpCellItems->GetCount() > 0)
			{
				PCELL pTmpCell = pOpCellItems->GetItem(0);
				m_nCellsState[pTmpCell->row][pTmpCell->col] = Human;  // check if defending this by human results in opponent's create fork.
				CellItems * pOpCellItems2 = GetCompletableCells(Human);
				m_nCellsState[pTmpCell->row][pTmpCell->col] = Empty;

				if (pOpCellItems2->GetCount() >= 2)  // if being created, this cell must not be marked next.
					nTmp[i][j] = 2;

				delete pOpCellItems2;
			}
			else           // if the computer will not have two in a row.
			{
				CellItems * pOpCellItems2 = GetPWinableCells(Human); // check if marking this cell results in opponent's create fork.
				if (pOpCellItems->GetCount() > 0)
					nTmp[i][j] = 2;

				delete pOpCellItems2;
			}
			delete pOpCellItems;

			m_nCellsState[i][j] = Empty;
		}
	}
	pCellItems = GetPCompletableCells(Com); // check if computer is able to have two in a row.
	if (pCellItems->GetCount() > 0)			// if present, 
	{										// The cells are distinct that results in opponent's create fork and
											// that result in opponent's two in a row
		int count = pCellItems->GetCount(); 
		for (i = count - 1; i >= 0; i--)
		{
			PCELL pCell = pCellItems->GetItem(i);
			if (nTmp[pCell->row][pCell->col] != 2)
			{
				m_nCellsState[pCell->row][pCell->col] = Com;

				CellItems * pOpCellItems = GetCompletableCells(Com);
				PCELL pTmpCell = pOpCellItems->GetItem(0);
				m_nCellsState[pTmpCell->row][pTmpCell->col] = Human;
				CellItems * pOpCellItems2 = GetCompletableCells(Human);
				m_nCellsState[pTmpCell->row][pTmpCell->col] = Empty;
				m_nCellsState[pCell->row][pCell->col] = Empty;

				int cc = pOpCellItems2->GetCount();
				if (cc > 0)
				{
					if (cc >= 2)
					{
						nTmp[pCell->row][pCell->col] = 2;
						pCellItems->RemoveItem(pCell);
					}
					else if (cc == 1 && nTmp[pCell->row][pCell->col] == 0)
						nTmp[pCell->row][pCell->col] = 1;
					
				}
				delete pOpCellItems2;
				delete pOpCellItems;
			}
		}
	}

	bool bFound = false;
	if (pCellItems->GetCount() > 0)   // Among the cells that computer is able to have two in a row, 
	{                                 // First, mark a cell that didn't result in opponent's two in a row, too, 
									  // and then if so cell is absent, mark a cell that resulted in opponent's two in a row
		int count = pCellItems->GetCount();
		PCELL pCell = NULL;
		for (i = 0; i < count; i++)
		{
			pCell = pCellItems->GetItem(i);
			if (nTmp[pCell->row][pCell->col] == 0)
				break;
		}
		if (i >= count)
		{
			for (i = 0; i < count; i++)
			{
				pCell = pCellItems->GetItem(i);
				if (nTmp[pCell->row][pCell->col] == 1)
					break;
			}
			if (i < count)
			{
				row = pCell->row;
				col = pCell->col;
				m_nCellsState[row][col] = Com;
				bFound = true;
			}
		}
		else
		{
			row = pCell->row;
			col = pCell->col;
			m_nCellsState[row][col] = Com;
			bFound = true;
		}
	}
	
	if (!bFound)		// Duaring above processes if the cell to mark is not looked for, a suitable cell of rest cells is marked.
	{
		for (i = 0; i < m_nRows; i++)
		{
			for (j = 0; j < m_nCols; j++)
			{
				if (m_nCellsState[i][j] == Empty && nTmp[i][j] == 0)
				{
					row = i;
					col = j;
					m_nCellsState[i][j] = Com;
					break;
				}
			}
			if (j < m_nCols)
				break;
		}
		if (j >= m_nRows)
		{
			for (i = 0; i < m_nRows; i++)
			{
				for (j = 0; j < m_nCols; j++)
				{
					if (m_nCellsState[i][j] == Empty && nTmp[i][j] == 1)
					{
						row = i;
						col = j;
						m_nCellsState[i][j] = Com;
						break;
					}
				}
				if (j < m_nCols)
					break;
			}
			if (j >= m_nRows)
			{
				for (i = 0; i < m_nRows; i++)
				{
					for (j = 0; j < m_nCols; j++)
					{
						if (m_nCellsState[i][j] == Empty)
						{
							row = i;
							col = j;
							m_nCellsState[i][j] = Com;
							break;
						}
					}
					if (j < m_nCols)
						break;
				}
			}
		}
	}


	if (DecideWin(Com))
		result = ComWin;

	if (result == Waiting && !Playable())
		result = Draw;

	return result;
}

// check if winner is defined.
bool NacEngine::DecideWin(CellState decideState)
{
	CellState opponent = Human;

	if (decideState == Human)
		opponent = Com;

	// check if the player succeed in placing three of 
	// one's mark in a horizontal, vertical, or diagonal.
	if (Completed(decideState))
		return true;
	

	// if the opponent has no two in a row and the player has a fork, the player will win.
	CellItems * pCellItems = GetCompletableCells(opponent);
	if (pCellItems->GetCount() > 0)  // if the opponent has two in a row, the player may also not win.
	{
		delete pCellItems;
		return false;
	}

	// check if the player has a fork.
	pCellItems = GetCompletableCells(decideState);
	if (pCellItems->GetCount() >= 2)
	{
		delete pCellItems;
		return true;
	}

	delete pCellItems;

	return false;
}

// check if the certain player succeed in placing three of 
// one's mark in a horizontal, vertical, or diagonal.
bool NacEngine::Completed(CellState cell)
{
	int i, j;
	for (i = 0; i < m_nRows; i++)
	{
		for (j = 0; j < m_nCols; j++)
		{
			if (m_nCellsState[i][j] != cell)
				break;
		}

		if (j >= m_nCols)
			return true;
	}

	for (j = 0; j < m_nCols; j++)
	{
		for (i = 0; i < m_nRows; i++)
		{
			if (m_nCellsState[i][j] != cell)
				break;
		}

		if (i >= m_nRows)
			return true;
	}

	for (i = 0, j = 0; i < m_nRows; i++, j++)
	{
		if (m_nCellsState[i][j] != cell)
			break;
	}
	if (i >= m_nRows)
		return true;

	for (i = 0, j = m_nCols - 1; i < m_nRows; i++, j--)
	{
		if (m_nCellsState[i][j] != cell)
			break;
	}
	if (i >= m_nRows)
		return true;

	return false;
}

// check if the player has two in a row.
// return value is the list of cells to mark at that time. 
// that is, if the player marks the cell of returned list, he will win.
CellItems * NacEngine::GetCompletableCells(CellState cellState)
{
	CellState opponent = Human;
	if (cellState == Human)
		opponent = Com;

	CellItems *pRet = new CellItems();
	bool bFoundEmpty;
	int i, j, ii, jj;
	for (i = 0; i < m_nRows; i++)
	{
		bFoundEmpty = false;
		for (j = 0; j < m_nCols; j++)
		{
			if (m_nCellsState[i][j] == opponent)
				break;

			if (m_nCellsState[i][j] == Empty)
			{
				if (bFoundEmpty)
					break;

				jj = j;
				bFoundEmpty = true;
			}
		}

		if (bFoundEmpty && j >= m_nCols)
			pRet->AddItem(i, jj);
	}

	for (j = 0; j < m_nCols; j++)
	{
		bFoundEmpty = false;

		for (i = 0; i < m_nRows; i++)
		{
			if (m_nCellsState[i][j] == opponent)
				break;

			if (m_nCellsState[i][j] == Empty)
			{
				if (bFoundEmpty)
					break;

				ii = i;
				bFoundEmpty = true;
			}
		}

		if (bFoundEmpty && i >= m_nRows)
			pRet->AddItem(ii, j);
	}

	bFoundEmpty = false;
	int tmpFoundRow, tmpFoundCol; 
	for (i = 0, j = 0; i < m_nRows; i++, j++)
	{
		if (m_nCellsState[i][j] == opponent)
			break;

		if (m_nCellsState[i][j] == Empty)
		{
			if (bFoundEmpty)
				break;

			bFoundEmpty = true;
			tmpFoundRow = i;
			tmpFoundCol = j;
		}
	}
	if (bFoundEmpty && i >= m_nRows)
		pRet->AddItem(tmpFoundRow, tmpFoundCol);

	bFoundEmpty = false;
	for (i = 0, j = m_nCols - 1; i < m_nRows; i++, j--)
	{
		if (m_nCellsState[i][j] == opponent)
			break;

		if (m_nCellsState[i][j] == Empty)
		{
			if (bFoundEmpty)
				break;

			bFoundEmpty = true;
			tmpFoundRow = i;
			tmpFoundCol = j;
		}
	}
	if (bFoundEmpty && i >= m_nRows)
		pRet->AddItem(tmpFoundRow, tmpFoundCol);

	return pRet;
}

// check if the player is able to create fork.
// return value is the list of cells to mark at that time.
// that is, if the player mark the cell of returned list, he will create fork.
CellItems * NacEngine::GetPWinableCells(CellState cellState)
{
	CellItems * pRet = new CellItems();
	int i, j;
	for (i = 0; i < m_nRows; i++)
	{
		for (j = 0; j < m_nCols; j++)
		{
			if (m_nCellsState[i][j] != Empty)
				continue;

			m_nCellsState[i][j] = cellState;
			CellItems *pTmpCellItems = GetCompletableCells(cellState);
			m_nCellsState[i][j] = Empty;

			if (pTmpCellItems->GetCount() >= 2)
				pRet->AddItem(i, j);

			delete pTmpCellItems;
		}
	}

	return pRet;
}

// check if the player is able to have two in a row.
// return value is the list of cells to mark at that time.
// that is, if the player mark the cell of returne list, he will have two in a row.
CellItems * NacEngine::GetPCompletableCells(CellState cellState)
{
	CellItems * pRet = new CellItems();
	int i, j;
	for (i = 0; i < m_nRows; i++)
	{
		for (j = 0; j < m_nCols; j++)
		{
			if (m_nCellsState[i][j] != Empty)
				continue;

			m_nCellsState[i][j] = cellState;
			CellItems *pTmpCellItems = GetCompletableCells(cellState);
			m_nCellsState[i][j] = Empty;

			if (pTmpCellItems->GetCount() > 0)
				pRet->AddItem(i, j);

			delete pTmpCellItems;
		}
	}

	return pRet;
}

// check if game is able to continue, that is, check if there is empty cell in board.
bool NacEngine::Playable()
{
	for (int i = 0; i < m_nRows; i++)
	{
		for (int j = 0; j < m_nCols; j++)
		{
			if (m_nCellsState[i][j] == Empty)
				return true;
		}
	}

	return false;
}


int NacEngine::RangedRand(int range_min, int range_max)
{
	srand((unsigned)time(NULL));
	return (int)((double)rand() / (double)(RAND_MAX + 1)) * (range_max - range_min) + range_min;
}

int NacEngine::GenRandCornerNum()
{
	int no, retNo = 1;
	no = RangedRand(0, 4);
	switch (no)
	{
	case 0:
		retNo = 1;
		break;
	case 1:
		retNo = 3;
		break;
	case 2:
		retNo = 7;
		break;
	case 3:
		retNo = 9;
		break;
	default:
		;
	}

	return retNo;
}


CellItems::CellItems()
	: m_pFirstCell(NULL), m_pTailCell(NULL)
{
}

CellItems::~CellItems()
{
	PCELL pCell = m_pFirstCell;
	PCELL pNextCell;
	while (pCell)
	{
		pNextCell = pCell->next;
		free(pCell);
		pCell = pNextCell;
	}
}

PCELL CellItems::GetItem(const int & index)
{
	if (index >= GetCount())
		return NULL;

	PCELL pCell = m_pFirstCell;
	int i;
	for (i = 0 ; i < index; i++, pCell = pCell->next);

	return pCell;
}

int CellItems::GetCount()
{
	PCELL pCell;
	int count = 0;
	for (pCell = m_pFirstCell; pCell; pCell = pCell->next)
		count++;

	return count;
}

void CellItems::AddItem(PCELL pCell)
{
	if (!pCell)
		return;

	for (PCELL pTmp = m_pFirstCell; pTmp; pTmp = pTmp->next)
	{
		if (pTmp->row == pCell->row && pTmp->col == pCell->col)
			return;
	}
	

	pCell->next = NULL;
	pCell->prev = m_pTailCell;
	if (m_pTailCell)
		m_pTailCell->next = pCell;

	m_pTailCell = pCell;

	if (!m_pFirstCell)
		m_pFirstCell = pCell;
}

void CellItems::AddItem(const int & row, const int & col)
{
	PCELL pCell = (PCELL)malloc(sizeof(CELL));

	pCell->row = row;
	pCell->col = col;
	AddItem(pCell);
}

void CellItems::RemoveItem(PCELL pCell)
{
	if (!pCell)
		return;

	for (PCELL pTmp = m_pFirstCell; pTmp; pTmp = pTmp->next)
	{
		if (pTmp == pCell)
		{
			PCELL pPrevCell = pTmp->prev;
			PCELL pNextCell = pTmp->next;
			if (pPrevCell)
				pPrevCell->next = pNextCell;
			if (pNextCell)
				pNextCell->prev = pPrevCell;

			if (m_pTailCell == pCell)
				m_pTailCell = pPrevCell;

			if (m_pFirstCell == pCell)
				m_pFirstCell = pNextCell;

			delete pCell;

			return;
		}
	}
}





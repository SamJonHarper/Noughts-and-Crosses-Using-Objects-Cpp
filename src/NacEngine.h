
#pragma once


// Duration searching, this struct is used to save the result cells searching.
struct CELL{
	int row;
	int col;
	struct CELL * next;
	struct CELL * prev;
};

typedef struct CELL CELL;
typedef struct CELL * PCELL;

// This class manages search result cell's list.
class CellItems {
public:
	CellItems();
	~CellItems();

	PCELL GetItem(const int & index);					// return 'index'th cell item.
	int GetCount();										// return the number of cells.
	void AddItem(PCELL pCell);							// add item in the cells' list.
	void AddItem(const int & row, const int & col);
	void RemoveItem(PCELL pCell);						// remove item from the cells' list.

private:
	PCELL m_pFirstCell;		// the first cell of the list
	PCELL m_pTailCell;		// the tail cell of the list
};


// This class is in charge of Game Engine.
class NacEngine
{
public:
	enum PlayResult {		// now, result
		Waiting = 100,		// it means that the winner is undefined.
		HumanWin,			// it means that you had won.
		ComWin,				// it means that computer had won.
		Draw				// it means that this game had been a draw.
	};
	
	enum CellState {	// In board, each cell's owner
		Empty = 1000,	// Nobody owns this cell.
		Human,			// Human owns this cell.
		Com				// Computer owns this cell.
	};


	// This engine is optimal for (3,3,3) game. 
	// Don't change the rows and cols.
	NacEngine(const int & rows = 3, const int & cols = 3, const int & count = 3);
	~NacEngine();

	void init();	
	PlayResult PlayHuman(const int & row, const int & col);	// Human's play operation
	PlayResult PlayCom(int & row, int & col);				// Computer's play operation

	// in the (m,n,k) game, m_nRows = m, m_nCols = n, m_nCount = k
	// now, these aren't import beause these values are fixed.
	int m_nRows;
	int m_nCols;
	int m_nCount;

private:
	
	bool Playable();	// check if game is able to continue, that is, check if there is empty cell in board.
	bool DecideWin(CellState decideState);	// check if winner is defined.
	bool Completed(CellState cell);			// check if the certain player succeed in placing three of 
											//          one's mark in a horizontal, vertical, or diagonal.

	CellItems * GetCompletableCells(CellState cellState);	// check if the player has two in a row.
								// return value is the list of cells to mark at that time. 
								// that is, if the player marks the cell of returned list, he will win.
	CellItems * GetPWinableCells(CellState cellState);	// check if the player is able to create fork.
								// return value is the list of cells to mark at that time.
								// that is, if the player mark the cell of returned list, he will create fork.
	CellItems * GetPCompletableCells(CellState cellState); // check if the player is able to have two in a row.
								// return value is the list of cells to mark at that time.
								// that is, if the player mark the cell of returne list, he will have two in a row.
	int RangedRand(int range_min, int range_max);	// return random value between 'range_min' and 'range_max'.
	int GenRandCornerNum();		// return random corner number
	int GetOppositeCorner(const int cornerNo);	// return number of corner opposite to 'cornerNo'

	CellState m_nCellsState[10][10];	// the state of cells in board. element's type is "CellState"
								
	int m_nTotalCount;			// the number of total steps
	int m_nPrevCellNo;			// previous marked cell number
};



/*
My sudoku solving algorithm was based off of that provided at www.pagedon.com/sudolu-program/my_programming/
*/
#include <iostream>
#include <fstream>
#include <chrono>
#define boardSize 81
#define sideSize 9

using namespace std;

struct Board{
	int values[81];
	bool isFixedValue[81];
	bool isPossibleValue[81][9];
	int coordinates;
};

void getFileName(char fileName[]){
	cout << "Please give the file location of your sudoku board."<<endl;
	cin.getline(fileName, 255);
	return;
}

bool getBoard(char fileName[], Board &mainBoard){
	ifstream file(fileName);
	if(file.fail())
		return false;
	char temp = '0';
	for (int i = 0; i < boardSize; i++){
		file >> temp;
		mainBoard.values[i] = (int)((int)temp - (int)'0');
		//cout << mainBoard.values[i] << ",";
		if(mainBoard.values[i] > 0){
			mainBoard.isFixedValue[i] = true;
		}
		else{
			mainBoard.isFixedValue[i] = false;
			mainBoard.values[i] = (mainBoard.values[i] / -1);
		}
	}
	file.close();
	return true;
}	

bool anyDuplicates(int board[], int coordinates, int value)
{
	int row = coordinates / sideSize;
	int column = coordinates % sideSize;
	for(int i = row * sideSize; i < ((row*sideSize)+sideSize); i++)
	{
		if(value == board[i])
			return true;
	}
	for(int i = column; i < boardSize; i+=sideSize)
	{
		if(value == board[i])
			return true;
	}
	while(column%3 != 0)
		column--;
	while(row%3 != 0)
		row--;
	for(int r = row; r < (row+3); r++)
	{
		for(int c = column; c < (column+3); c++)
		{
			if(value == board[((r*9)+c)])
				return true;
		}
	}
	return false;
}

bool validateBoard(Board board){
	for(int i = 0; i < boardSize; i++){
		int tempValue = board.values[i];
		board.values[i] = 0;
		if(tempValue != 0 && anyDuplicates(board.values, i, tempValue)){
			cout<<"ERROR: Duplicate value '"
			    << tempValue;
			return false;
		}
		board.values[i] = tempValue;
	}
	return true;
}

void displayBoard(Board board)
{
	ofstream results("results.txt");
	for(int i = 0; i < boardSize; i++){
		results<<board.values[i] <<",";
		if(i%sideSize == 8)
			results<<endl;
	}
	results.close();
}

bool isSolved(Board board)
{
	for(int i = 0; i < boardSize; i++){
		if(board.values[i] == 0)
			return false;
	}
	return true;
}

bool canChange(Board mainBoard, int coordinates, int value)
{
	if(mainBoard.isFixedValue[coordinates])
		return false;
	if(value==0)
		return true;
	if(anyDuplicates(mainBoard.values, coordinates, value))
		return false;
	return true;
}

bool checkPossibles(Board &newBoard)
{
	bool noChanges;
	do{
		noChanges = true;
		for(int i = 0; i < boardSize; i++){
			int possibles = 0;
			int value;
			if(newBoard.isFixedValue[i]){
				for(int num = 0; num < 9; num++)
					newBoard.isPossibleValue[i][num] = false;
				newBoard.isPossibleValue[i][newBoard.values[i] - 1] = true;
			}
			else{
				for(int guess = 1; guess <= 9; guess++){
					if(canChange(newBoard, i, guess)){
						value = guess;
						newBoard.isPossibleValue[i][guess-1] = true;
						possibles++;
					}
					else
						newBoard.isPossibleValue[i][guess-1] = false;
				}
			}
			if(possibles==1){
				newBoard.values[i] = value;
				newBoard.isFixedValue[i] = true;
				noChanges = false;
			}
		}
	}while(noChanges == false);
	if(isSolved(newBoard))
		return true;
	return false;		
}

Board recursiveBrute(Board board, int startPosition)
{
	while(startPosition < boardSize && board.isFixedValue[startPosition] && board.values[startPosition] != 0)
		startPosition++;
	if(startPosition >= boardSize)
		return board;
	for(int guess = 1; guess <= 9; guess++){
		Board tempBoard = board;
		if(board.isPossibleValue[startPosition][guess-1] && canChange(board,startPosition,guess)){
			board.values[startPosition] = guess;
			tempBoard = recursiveBrute(board, startPosition+1);
		}
		if(isSolved(tempBoard))
			return tempBoard;
	}
	board.values[startPosition] = 0;
	return board;
}

float solve(Board &newBoard)
{
	int coordinates = 0;
	bool noChanges;
	//int startTime = clock();
	do
	{
		noChanges = true;
		if(checkPossibles(newBoard))
			return true;
		for(int i = 0; i < boardSize; i += sideSize){
			for(int guess = 1; guess <= 9; guess++){
				int total = 0;
				for(int iRow = i; iRow < (i+9); iRow++){
					if(!newBoard.isFixedValue[iRow])
						total += newBoard.isPossibleValue[iRow][guess-1];
				}
				if(total == 1){
					for(int iRow = i; iRow <(i+9); iRow++){
						if(newBoard.isPossibleValue[iRow][guess-1] && !newBoard.isFixedValue[iRow]){
							newBoard.values[iRow] = guess;
							newBoard.isFixedValue[iRow] = true;
							noChanges = false;
						}
					}
				}
			}
		}
		if(checkPossibles(newBoard))
			return true;

		for(int i = 0; i < sideSize; i++){
			for(int guess = 1; guess <= 9; guess++){
				int total = 0;
				for(int iColumn = i; iColumn < boardSize; iColumn += sideSize){
					if(!newBoard.isFixedValue[iColumn]){
						total += newBoard.isPossibleValue[iColumn][guess-1];
					}
				}
				if(total == 1){
					for(int iColumn = i; iColumn < boardSize; iColumn += sideSize){
						if(newBoard.isPossibleValue[iColumn][guess-1] && !newBoard.isFixedValue[iColumn]){
							newBoard.values[iColumn] = guess;
							newBoard.isFixedValue[iColumn] = true;
							noChanges = false;
						}
					}
				}
			}
		}
		
		if(checkPossibles(newBoard))
			return true;
		for(int r = 0; r < boardSize; r+= sideSize*3){
			for(int c = 0; c < sideSize; c += 3){
				int i = r+c;
				for(int guess = 1; guess <= 9; guess++){
					int total = 0;
					for(int iBlockR = i; iBlockR < (i+sideSize*3); iBlockR += sideSize){
						for(int iBlockC = iBlockR; iBlockC < (iBlockR + 3); iBlockC++){
							if(!newBoard.isFixedValue[iBlockC])
								total += newBoard.isPossibleValue[iBlockC][guess-1];
						}
					}
					if(total == 1){
						for(int iBlockR = i; iBlockR < (i+sideSize*3); iBlockR += sideSize){
							for(int iBlockC = iBlockR; iBlockC < (iBlockR + 3); iBlockC++){
								if(newBoard.isPossibleValue[iBlockC][guess-1] && !newBoard.isFixedValue[iBlockC]){
									newBoard.values[iBlockC] = guess;
									newBoard.isFixedValue[iBlockC] = true;
									noChanges = false;
								}
							}
						}
					}
				}
			}
		}
		if(checkPossibles(newBoard))
			return true;
	}while(noChanges == false);
	if(!isSolved(newBoard))
		newBoard = recursiveBrute(newBoard, 0);
	return 0;
}

int main(int argc, char *argv[]){
	auto start_time = std::chrono::high_resolution_clock::now();
	char fileName[256];
	Board mainBoard;
	getFileName(fileName);
	if(!getBoard(fileName,mainBoard))
		cout <<"Error with filename";
	else if(validateBoard(mainBoard)){
		displayBoard(mainBoard);
	}
	solve(mainBoard);
	displayBoard(mainBoard);
	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	cout << "Solve time took " << std::chrono::duration_cast<std::chrono::milliseconds>(time).count() << "ms"<<endl;
	return 0;
}
















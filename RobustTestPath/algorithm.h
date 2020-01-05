#pragma once

#include "kiss2.h"
#include "data.h"

#include "../cudd/includes/cudd.h"

// ��������� �� ���������, ������ ���� ���������� ���������, � ����� ��� ������� ����������.
struct BddVariables
{
	DdNode ** zx;
	// ����� ������� ���������� � ���������� ���������.
	int inputs;
	int states;
};

// ��������� �� ROBDD �������������� ������� ��������� (Rzi).
struct BddTransitionFunction
{
	DdNode ** ziFunction;
};

// ��� �������� ���������� � ��������� �������������.
struct ProductsString
{
	std::vector<std::string> products;
};

class Algorithm
{
private:
	DdManager * manager_;
	BddVariables bddVariables_;
	BddTransitionFunction bddTransitionFunction_;













private:
	DdNode * nextBddRsi(DdNode * Rsi, std::vector<DdNode *> &RSI);

	// ��������� ������������ ������������������ �� ������ �������� ������, ���������� ��������� (����� ���������).
	std::vector<std::string> getTransitionSequence(const std::vector<DdNode *> &RSI, std::string beginState);

	// �������� ���������� ������������ ������������������, ��� ������� ��� ������� ��������� ���� ��������� ���������.
	// ���������� ������������ �� ������������ ������������������, � ���������� ��������� (������������� ������ �����),
	// � �������� �� ��������� ������ �� ������� ��������� �� ����������. RSI - ������ ������ � �������� �����������
	// (� ������� ����������).
	std::string proof(std::string beginState, const std::vector<std::string> &transitionSequence, const std::vector<DdNode *> &RSI);

public:
	// �������� ����� ���������, ���������� ������������� �������, l - ����������� �� ������ ������������
	// ������������������.
	std::pair<std::vector<std::string>, std::string> transferSequence(DdNode * Rs0, std::string begState, Pla & pla, size_t l);



















public:
	Algorithm();
	~Algorithm();

	void createBddVariables(int inputs, int states);
	void createBddTransitionFunction(Pla pla);

	// �������� bdd ���� �� ��������� ���������� �������������� � ��������� �������.
	DdNode * getBddByProductsString(const ProductsString &products);

	// �� ���������, � ���� ������������� �������, ��������������� kj, ����������� ��������������� Rzi, � ��������� Rsi (����� ���������).
	DdNode * bddByCube(int ** cube);

	// �� bdd ����� ������� ��� ���� ��������������� ������� ���������� � ������� ������� ���� � 1 ������������ �������.
	DdNode * removeInputsVariables(DdNode * f);

	// ��� ��������� ���������.
	ProductsString graphToProductsString(DdNode *graph);

	// ��� ��������� ���������.
	std::string cubeToString(int **cube, size_t size);

	// ��������� ���������� �� ��������� kon � ����� RSI (������ ����������� ��� ��� ����� � ������� �� ���������).
	bool substituteToBdd(DdNode * RSI, DdNode * kon);

	// ������������ ����� ����� ���������.
	ProductsString setInputsAfterStates(std::vector<std::string> products, int inputs, int states);

	// ��������� ���� ������.
	DdNode * and(DdNode * op1, DdNode * op2);

	BddVariables bddVariables();



};	// class Algorithm
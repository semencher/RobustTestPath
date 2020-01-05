#pragma once

#include "kiss2.h"
#include "data.h"

#include "../cudd/includes/cudd.h"

// Указатели на перменные, сперва идут переменные состояний, а потом уже входные переменные.
struct BddVariables
{
	DdNode ** zx;
	// Число входных переменных и переменных состояний.
	int inputs;
	int states;
};

// Указатели на ROBDD представляющие функции переходов (Rzi).
struct BddTransitionFunction
{
	DdNode ** ziFunction;
};

// Для хранения конъюнкций в строковом представлении.
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

	// Получение установочной последовательности по набору хранимых графов, начальному состоянию (часть алгоритма).
	std::vector<std::string> getTransitionSequence(const std::vector<DdNode *> &RSI, std::string beginState);

	// Проверка полученной установочной последовательности, при условии что функции переходов были вычислены правильно.
	// Происходит итерирование по установочной последовательности, и вычисление состояний (моделирование работы схемы),
	// и проверка на получение одного из целевых состояний из начального. RSI - массив графов с входными переменными
	// (в будущем упрощенные).
	std::string proof(std::string beginState, const std::vector<std::string> &transitionSequence, const std::vector<DdNode *> &RSI);

public:
	// Основная часть алгоритма, использует вышеописанные функции, l - ограничение на длинну установочной
	// последовательности.
	std::pair<std::vector<std::string>, std::string> transferSequence(DdNode * Rs0, std::string begState, Pla & pla, size_t l);



















public:
	Algorithm();
	~Algorithm();

	void createBddVariables(int inputs, int states);
	void createBddTransitionFunction(Pla pla);

	// Строится bdd граф по множеству конъюнкций представленных в строковом формате.
	DdNode * getBddByProductsString(const ProductsString &products);

	// По интервалу, в виде динамическоро массива, представляющему kj, перемножает соответствующие Rzi, и формирует Rsi (часть алгоритма).
	DdNode * bddByCube(int ** cube);

	// Из bdd графа удаляет все узлы соответствующие входным переменным и заводит висячие дуги в 1 терминальную вершину.
	DdNode * removeInputsVariables(DdNode * f);

	// Для процедуры упрощения.
	ProductsString graphToProductsString(DdNode *graph);

	// Для процедуры упрощения.
	std::string cubeToString(int **cube, size_t size);

	// Проверяет содержится ли конъюнкци kon в графе RSI (Просто перемножаем два эти графа и смотрим на результат).
	bool substituteToBdd(DdNode * RSI, DdNode * kon);

	// Переставляет входы после состояний.
	ProductsString setInputsAfterStates(std::vector<std::string> products, int inputs, int states);

	// Умножение двух графов.
	DdNode * and(DdNode * op1, DdNode * op2);

	BddVariables bddVariables();



};	// class Algorithm
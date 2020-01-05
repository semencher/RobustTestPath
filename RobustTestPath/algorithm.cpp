#include "algorithm.h"

Algorithm::Algorithm() {
	manager_ = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
}

Algorithm::~Algorithm() {
	Cudd_Quit(manager_);
}

void Algorithm::createBddVariables(int inputs, int states) {
	bddVariables_.inputs = inputs;
	bddVariables_.states = states;
	bddVariables_.zx = new DdNode*[inputs + states];
	size_t size = inputs + states;
	for (size_t i = 0; i < size; ++i) {
		bddVariables_.zx[i] = Cudd_bddIthVar(manager_, i);
		Cudd_Ref(bddVariables_.zx[i]);
	}
}

void Algorithm::createBddTransitionFunction(Pla pla) {
	bddTransitionFunction_.ziFunction = new DdNode*[bddVariables_.states];

	DdNode * treeResult;
	size_t states = bddVariables_.states;
	size_t countElements = pla.elements().size();
	// Цикл для формирования графов для функций переходов.
	for (size_t i = 0; i < states; ++i) {
		treeResult = Cudd_ReadLogicZero(manager_);
		Cudd_Ref(treeResult);
		// Цикл для прохождения по всем конъюнкциям системы.
		for (size_t j = 0; j < countElements; ++j) {
			// Входит ли данная конъюнкция в текущую функцию.
			if (pla.elements()[j].output[i + pla.outputs() - pla.states()] == '1') {
				DdNode * kon = Cudd_ReadOne(manager_);
				Cudd_Ref(kon);

				size_t countStates = bddVariables_.states;
				// Прохождение по конъюнкции по переменным состояний.
				for (size_t u = 0; u < countStates; ++u) {
					if (pla.elements()[j].input[u + bddVariables_.inputs] == '1') {
						DdNode * mult = Cudd_bddAnd(manager_, bddVariables_.zx[u], kon);
						Cudd_Ref(mult);
						Cudd_RecursiveDeref(manager_, kon);
						kon = mult;
					}
					if (pla.elements()[j].input[u + bddVariables_.inputs] == '0') {
						DdNode * notVar = Cudd_Not(bddVariables_.zx[u]);
						Cudd_Ref(notVar);
						DdNode * mult = Cudd_bddAnd(manager_, notVar, kon);
						Cudd_RecursiveDeref(manager_, notVar);
						Cudd_Ref(mult);
						Cudd_RecursiveDeref(manager_, kon);
						kon = mult;
					}
				}
				// Прохождение по этой конъюнкции по входным переменным.
				size_t countInputs = bddVariables_.inputs;
				for (size_t v = 0; v < countInputs; ++v) {
					if (pla.elements()[j].input[v] == '1') {
						DdNode * mult = Cudd_bddAnd(manager_, bddVariables_.zx[v + bddVariables_.states], kon);
						Cudd_Ref(mult);
						Cudd_RecursiveDeref(manager_, kon);
						kon = mult;
					}
					if (pla.elements()[j].input[v] == '0') {
						DdNode * notVar = Cudd_Not(bddVariables_.zx[v + bddVariables_.states]);
						Cudd_Ref(notVar);
						DdNode * mult = Cudd_bddAnd(manager_, notVar, kon);
						Cudd_RecursiveDeref(manager_, notVar);
						Cudd_Ref(mult);
						Cudd_RecursiveDeref(manager_, kon);
						kon = mult;
					}
				}
				DdNode * sum = Cudd_bddOr(manager_, kon, treeResult);
				Cudd_Ref(sum);
				Cudd_RecursiveDeref(manager_, treeResult);
				treeResult = sum;
				Cudd_RecursiveDeref(manager_, kon);
			}
		}
		bddTransitionFunction_.ziFunction[i] = treeResult;
	}
}

DdNode * Algorithm::getBddByProductsString(const ProductsString &products)
{
	DdNode * bdd = Cudd_ReadLogicZero(manager_);
	Cudd_Ref(bdd);
	size_t size = products.products.size();
	// Идем по строковым конъюнкциям.
	for (size_t i = 0; i < size; ++i) {
		DdNode * kon = Cudd_ReadOne(manager_);
		Cudd_Ref(kon);
		size_t sizeKon = products.products[i].size();
		// Идем по строковой конъюнкции.
		for (size_t j = 0; j < sizeKon; ++j) {
			if (products.products[i][j] == '1') {
				DdNode * mult = Cudd_bddAnd(manager_, bddVariables_.zx[j], kon);
				Cudd_Ref(mult);
				Cudd_RecursiveDeref(manager_, kon);
				kon = mult;
			}
			if (products.products[i][j] == '0') {
				DdNode * notVar = Cudd_Not(bddVariables_.zx[j]);
				Cudd_Ref(notVar);
				DdNode * mult = Cudd_bddAnd(manager_, notVar, kon);
				Cudd_RecursiveDeref(manager_, notVar);
				Cudd_Ref(mult);
				Cudd_RecursiveDeref(manager_, kon);
				kon = mult;
			}
		}
		DdNode * sum = Cudd_bddOr(manager_, kon, bdd);
		Cudd_Ref(sum);
		Cudd_RecursiveDeref(manager_, bdd);
		bdd = sum;
		Cudd_RecursiveDeref(manager_, kon);
	}
	return bdd;
}

DdNode * Algorithm::bddByCube(int ** cube)
{
	DdNode * result = Cudd_ReadOne(manager_);
	Cudd_Ref(result);
	for (int i = 0; i < bddVariables_.states; ++i)
	{
		if (cube[0][i] == 1)
		{
			DdNode * newResult = Cudd_bddAnd(manager_, result, bddTransitionFunction_.ziFunction[i]);
			Cudd_Ref(newResult);
			Cudd_RecursiveDeref(manager_, result);
			result = newResult;
		}
		if (cube[0][i] == 0)
		{
			DdNode * inv = Cudd_Not(bddTransitionFunction_.ziFunction[i]);
			Cudd_Ref(inv);
			DdNode * newResult = Cudd_bddAnd(manager_, result, inv);
			Cudd_Ref(newResult);
			Cudd_RecursiveDeref(manager_, inv);
			Cudd_RecursiveDeref(manager_, result);
			result = newResult;
		}
	}
	return result;
}

DdNode * Algorithm::removeInputsVariables(DdNode * f)
{
	int states = bddVariables_.states;
	int inputs = bddVariables_.inputs;
	DdNode * result = Cudd_bddExistAbstract(manager_, f, bddVariables_.zx[inputs + states - 1]);
	Cudd_Ref(result);
	// Переменные удаляются последовательно.
	for (int i = 1; i < inputs; ++i)
	{
		DdNode * restrictNode = Cudd_bddExistAbstract(manager_, result, bddVariables_.zx[inputs + states - i - 1]);

		Cudd_Ref(restrictNode);
		Cudd_RecursiveDeref(manager_, result);
		result = restrictNode;
	}
	return result;
}

ProductsString Algorithm::graphToProductsString(DdNode *graph)
{
	ProductsString products;
	size_t fullStateSize = bddVariables_.inputs + bddVariables_.states;
	int ** cube;
	cube = new int*[1];
	cube[0] = new int[fullStateSize];
	double * value = new double;

	DdGen * gen = Cudd_FirstCube(manager_, graph, cube, value);
	products.products.push_back(cubeToString(cube, fullStateSize));

	while (Cudd_NextCube(gen, cube, value)) {
		products.products.push_back(cubeToString(cube, fullStateSize));
	}
	return products;
}

std::string Algorithm::cubeToString(int **cube, size_t size)
{
	std::string result;
	for (size_t i = 0; i < size; ++i) {
		if (cube[0][i] == 1)
		{
			result = result + '1';
			continue;
		}
		if (cube[0][i] == 0)
		{
			result = result + '0';
			continue;
		}
		result = result + '-';
	}
	return result;
}

bool Algorithm::substituteToBdd(DdNode * RSI, DdNode * kon)
{
	DdNode * result = Cudd_bddAnd(manager_, RSI, kon);
	Cudd_Ref(result);
	DdNode * zero = Cudd_ReadLogicZero(manager_);
	Cudd_Ref(zero);
	DdNode * tmp = Cudd_ReadLogicZero(manager_);
	Cudd_Ref(tmp);
	// Cudd_EquivDC говорит идентичны ли result и zero там где tmp = 0.
	if (Cudd_EquivDC(manager_, result, zero, tmp)) {
		Cudd_RecursiveDeref(manager_, zero);
		Cudd_RecursiveDeref(manager_, tmp);
		return false;
	}
	Cudd_RecursiveDeref(manager_, zero);
	Cudd_RecursiveDeref(manager_, tmp);
	return true;
}

ProductsString Algorithm::setInputsAfterStates(std::vector<std::string> products, int inputs, int states) {
	ProductsString productsResult;
	for (auto iter = products.begin(); iter < products.end(); ++iter) {
		std::string product = iter->substr(inputs, states) + iter->substr(0, inputs);
		productsResult.products.push_back(product);
	}
	return productsResult;
}

DdNode * Algorithm::and(DdNode * op1, DdNode * op2) {
	DdNode * result = Cudd_bddAnd(manager_, op1, op2);
	Cudd_Ref(result);
	return result;
}

BddVariables Algorithm::bddVariables() {
	return bddVariables_;
}











std::pair<std::vector<std::string>, std::string> Algorithm::transferSequence(DdNode * Rs0, std::string begState, Pla & pla, size_t l)
{
	ProductsString beginStateProduct;
	beginStateProduct.products.push_back(begState);
	DdNode * beginStateKon = getBddByProductsString(beginStateProduct);
	std::vector<DdNode *> RSI;

	size_t iter = 0;
	while (!substituteToBdd(Rs0, beginStateKon) && iter <= l)
	{
		DdNode * Rsi = nextBddRsi(Rs0, RSI);
		DdNode * zero = Cudd_ReadLogicZero(manager_);
		Cudd_Ref(zero);
		DdNode * tmp = Cudd_ReadLogicZero(manager_);
		Cudd_Ref(tmp);
		if (Cudd_EquivDC(manager_, Rsi, Cudd_ReadLogicZero(manager_), Cudd_ReadLogicZero(manager_))) {
			Cudd_RecursiveDeref(manager_, zero);
			Cudd_RecursiveDeref(manager_, tmp);
			iter = l;
		}
		Cudd_RecursiveDeref(manager_, zero);
		Cudd_RecursiveDeref(manager_, tmp);
		Cudd_RecursiveDeref(manager_, Rs0);
		Rs0 = Rsi;
		++iter;
	}
	if (iter == l + 1) {
		size_t size = RSI.size();
		for (size_t i = 0; i < size; ++i) {
			Cudd_RecursiveDeref(manager_, RSI[i]);
		}
		RSI.clear();
		Cudd_RecursiveDeref(manager_, Rs0);
		Cudd_RecursiveDeref(manager_, beginStateKon);
		return std::pair<std::vector<std::string>, std::string>(std::vector<std::string>(), std::string());
	}
	else
	{
		std::vector<std::string> result = getTransitionSequence(RSI, begState);
		std::string pr = proof(begState, result, RSI);

		size_t size = RSI.size();
		for (size_t i = 0; i < size; ++i) {
			Cudd_RecursiveDeref(manager_, RSI[i]);
		}
		Cudd_RecursiveDeref(manager_, Rs0);
		Cudd_RecursiveDeref(manager_, beginStateKon);
		return std::pair<std::vector<std::string>, std::string>(result, pr);
	}
}

DdNode * Algorithm::nextBddRsi(DdNode * Rsi, std::vector<DdNode *> &RSI)
{
	DdNode * result = Cudd_ReadLogicZero(manager_);
	Cudd_Ref(result);
	int ** cube;
	cube = new int*[1];
	cube[0] = new int[bddVariables_.inputs + bddVariables_.states];
	double * value = new double;

	// Получаем первый путь kj из Rsi.
	DdGen * gen = Cudd_FirstCube(manager_, Rsi, cube, value);
	// Получаем Rkj.
	DdNode * kon = bddByCube(cube);

	// result это Rsi+1.
	DdNode * newResult = Cudd_bddOr(manager_, result, kon);
	Cudd_Ref(newResult);
	Cudd_RecursiveDeref(manager_, result);
	Cudd_RecursiveDeref(manager_, kon);
	result = newResult;

	// Далее получаем последовательно все пути kj и достраиваем Rsi+1.
	while (Cudd_NextCube(gen, cube, value)) {
		DdNode * kon = bddByCube(cube);

		DdNode * newResult = Cudd_bddOr(manager_, result, kon);
		Cudd_Ref(newResult);
		Cudd_RecursiveDeref(manager_, result);
		Cudd_RecursiveDeref(manager_, kon);
		result = newResult;
	}
	// Добавляем в массив RSI для будущего вычисления установочной последовательности.
	RSI.push_back(result);
	delete[]cube[0];
	delete[]cube;
	delete value;
	// Удаляем узлы соответствующие входным переменным и возвращаем.
	return removeInputsVariables(result);
}

std::vector<std::string> Algorithm::getTransitionSequence(const std::vector<DdNode *> &RSI, std::string beginState)
{
	std::vector<std::string> result;

	int ** cube;
	cube = new int*[1];
	cube[0] = new int[bddVariables_.inputs + bddVariables_.states];
	double * value = new double;

	size_t CountRSI = RSI.size();
	// Идем по графам для получения установочной последовательности в соответствии с алгоритмом.
	for (size_t i = 0; i < CountRSI; ++i) {
		ProductsString products;
		products.products.push_back(beginState);

		DdNode * beginStateKon = getBddByProductsString(products);
		// Получаем продолжение состояния.
		DdNode * prolong = Cudd_bddAnd(manager_, beginStateKon, RSI[CountRSI - i - 1]);
		Cudd_Ref(prolong);

		//Фомрируем входной набор из продолжения (на место - ставим 1).
		Cudd_FirstCube(manager_, prolong, cube, value);
		std::string eps;
		size_t size = bddVariables_.inputs;
		for (size_t j = 0; j < size; ++j) {
			if (cube[0][bddVariables_.states + j] == 1) {
				eps.push_back('1');
				continue;
			}
			if (cube[0][bddVariables_.states + j] == 0) {
				eps.push_back('0');
				continue;
			}
			eps.push_back('1');
		}
		result.push_back(eps);

		// Получаем полное состояние из входного вектора и состояния.
		std::string fullState = beginState + eps;
		ProductsString productFullState;
		productFullState.products.push_back(fullState);
		DdNode * fullStateKon = getBddByProductsString(productFullState);
		beginState.clear();

		// Дальше выводим следующее состояние.
		size_t countTransfFun = bddVariables_.states;
		for (size_t j = 0; j < countTransfFun; ++j) {
			if (substituteToBdd(bddTransitionFunction_.ziFunction[j], fullStateKon)) {
				beginState.push_back('1');
			}
			else {
				beginState.push_back('0');
			}
		}
		Cudd_RecursiveDeref(manager_, beginStateKon);
		Cudd_RecursiveDeref(manager_, prolong);
		Cudd_RecursiveDeref(manager_, fullStateKon);
	}
	delete[]cube[0];
	delete[]cube;
	delete value;
	return result;
}

std::string Algorithm::proof(std::string beginState, const std::vector<std::string> &transitionSequence, const std::vector<DdNode *> &RSI)
{
	size_t size = transitionSequence.size();
	for (size_t i = 0; i < size; ++i) {
		std::string fullState = beginState + transitionSequence[i];
		ProductsString productsString;
		productsString.products.push_back(fullState);
		DdNode * fullStateKon = getBddByProductsString(productsString);
		if (!substituteToBdd(RSI[size - i - 1], fullStateKon)) {
			return std::string();
		}
		beginState.clear();
		size_t statesCount = bddVariables_.states;
		for (size_t j = 0; j < statesCount; ++j) {
			if (substituteToBdd(bddTransitionFunction_.ziFunction[j], fullStateKon)) {
				beginState.push_back('1');
			}
			else {
				beginState.push_back('0');
			}
		}
	}
	return beginState;
}
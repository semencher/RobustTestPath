#include "manager.h"

#include <iostream>

Manager::Manager() {

}

bool Manager::readInputData(std::string testFolder, std::string fileKiss2, std::string filePla) {
	Data data;
	data.setTestFolder(testFolder);
	bool readStatus = data.readKiss2(fileKiss2) && data.readTestFileNames(testFolder + "*") && data.readPla(filePla);
	if (readStatus) {
		data_ = data;
		return true;
	}
	return false;
}

bool Manager::runMainAlgorithm() {
	// Идем по файлам с тестами для путей.
	std::vector<std::string> fileNames = data_.testFileNames();
	int iFileName = 0;
	while (data_.nextTestPaths() != -2) {
		std::cout << "Test file is " + fileNames[iFileName] + "...\n";

		std::vector<TestPath> testPaths = data_.currentTestPaths();

		algorithm_.createBddVariables(data_.pla().inputs() - data_.pla().states(), data_.pla().states());
		algorithm_.createBddTransitionFunction(data_.pla());
		int inputs = data_.pla().inputs() - data_.pla().states();
		int states = data_.pla().states();
		std::vector<ElementK> kiss2Elements = data_.kiss2().elements();

		// Идем по тестам для каждого пути.
		for (auto testPath = testPaths.begin(); testPath < testPaths.end(); ++testPath) {
			std::cout << "Test path is " << testPath->path_ << ", output is " << testPath->output_ << ", input is " << testPath->input_ << "...\n";

			bool beginPathIsState = false;
			bool risingTransition = false;
			bool fallingTransition = false;

			if (testPath->input_ > inputs) {
				beginPathIsState = true;
			}

			// Дальше необходимо построить Rrob.
			ProductsString productsStringRrob;
			productsStringRrob = algorithm_.setInputsAfterStates(testPath->tests_, inputs, states);

			DdNode * Rrob = algorithm_.getBddByProductsString(productsStringRrob);

			if (!beginPathIsState) {
				if (!risingTransition) {
					// Далее делаем первый шаг, а именно выбираем а тесты.
					int xInputVarNum = (testPath->input_ <= inputs) ? states + testPath->input_ - 1 : testPath->input_ - inputs - 1;
					DdNode * Rrob_xi = algorithm_. and (Rrob, algorithm_.bddVariables().zx[xInputVarNum]);
					// Конец первого шага.

					// Построим Rпетли.
					ProductsString productsStringRloop;
					for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
						if (iter->input.state == iter->output.state) {
							productsStringRloop.products.push_back(iter->input.state + iter->input.in);
						}
					}
					DdNode * Rloop = algorithm_.getBddByProductsString(productsStringRloop);

					// Делаем второй шаг алгоритма.
					DdNode * Ra = algorithm_. and (Rrob_xi, Rloop);

					// Делаем третий шаг алгоритма.
					DdNode * notXi = Cudd_Not(algorithm_.bddVariables().zx[xInputVarNum]);
					Cudd_Ref(notXi);
					DdNode * Rrob_notxi = algorithm_. and (Rrob, notXi);

					// Делаем четвертый шаг алгоритма.
					ProductsString productsString = algorithm_.graphToProductsString(Rrob_notxi);
					std::vector<std::string> products = productsString.products;
					for (auto & iter = products.begin(); iter < products.end(); ++iter) {
						for (int i = 0; i < states; ++i) {
							(*iter)[i] = '-';
						}
					}
					productsString.products = products;
					DdNode * Rrob_notxi_input = algorithm_.getBddByProductsString(productsString);

					// Делаем пятый шаг алгоритма.
					std::vector<std::string> productsStringRaStates;
					std::vector<std::string> productsStringRa = algorithm_.graphToProductsString(Ra).products;
					for (auto iter = productsStringRa.begin(); iter < productsStringRa.end(); ++iter) {
						productsStringRaStates.push_back(iter->substr(0, states));
					}

					// Делаем шестой шаг алгоритма.
					ProductsString productsStringRBeforeLoop;
					for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
						if (std::find(productsStringRaStates.begin(), productsStringRaStates.end(), iter->output.state) != productsStringRaStates.end()) {
							productsStringRBeforeLoop.products.push_back(iter->input.state + iter->input.in);
						}
					}
					DdNode * RBeforeLoop = algorithm_.getBddByProductsString(productsStringRBeforeLoop);

					// Делаем седьмой шаг алгоритма.
					DdNode * RBeforeTransition = algorithm_. and (RBeforeLoop, Rrob_notxi_input);

					// Реализуем восьмой шаг алгоритма.
					DdNode * Rs0 = algorithm_.removeInputsVariables(RBeforeTransition);

					// Реализуем девятый шаг алгоритма.
					std::pair<std::vector<std::string>, std::string> result = algorithm_.transferSequence(Rs0,
						data_.kiss2().initialState(),
						data_.pla(),
						100);
					std::vector<std::string> transferSequence = result.first;
					if (result.first.size() != 0) {
						// Реализуем десятый шаг алгоритма.
						std::string state = result.second;
						for (int i = 0; i < inputs; ++i) {
							state = state + "-";
						}
						ProductsString productsString;
						productsString.products.push_back(state);
						DdNode * kon = algorithm_.getBddByProductsString(productsString);
						DdNode * konMultRBeforeTransition = algorithm_. and (RBeforeTransition, kon);
						productsString = algorithm_.graphToProductsString(konMultRBeforeTransition);
						std::string input = "";
						for (int i = 0; i < inputs; ++i) {
							if (productsString.products[0][states + i] != '-') {
								input = input + productsString.products[0][i];
							}
							else {
								input = input + '0';
							}
						}
						transferSequence.push_back(input);

						// Реализауем одинадцатый шаг алгоритма.
						std::string nextState = "";
						for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
							if (iter->input.state == state) {
								for (int i = 0; i < iter->input.in.size(); ++i) {
									if (iter->input.in[i] != '-' && iter->input.in[i] != input[i]) {
										break;
									}
									if (iter->input.in.size() - 1 == i) {
										nextState = iter->output.state;
									}
								}
							}
						}

						// Реализуем двенадцатый шаг алгоритма.
						// input + nextState.

						// Реализуем тринадцатый шаг алгоритма.
						if (input[xInputVarNum] == '0') {
							input[xInputVarNum] = '1';
						}
						else {
							input[xInputVarNum] = '0';
						}
						transferSequence.push_back(input);

						// И так, конец случая когда xi в input для rising transition.
						// transferSequence обнаруживает задержку.
						risingTransition = true;
					}
					// Конец когда входная risingTransition.
				}
				if (!fallingTransition) {
					// Далее делаем первый шаг, а именно выбираем а тесты.
					int xInputVarNum = (testPath->input_ <= inputs) ? states + testPath->input_ - 1 : testPath->input_ - inputs - 1;
					DdNode * notXi = Cudd_Not(algorithm_.bddVariables().zx[xInputVarNum]);
					Cudd_Ref(notXi);
					DdNode * Rrob_notxi = algorithm_.and(Rrob, notXi);
					// Конец первого шага.

					// Построим Rпетли.
					ProductsString productsStringRloop;
					for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
						if (iter->input.state == iter->output.state) {
							productsStringRloop.products.push_back(iter->input.state + iter->input.in);
						}
					}
					DdNode * Rloop = algorithm_.getBddByProductsString(productsStringRloop);

					// Делаем второй шаг алгоритма.
					DdNode * Rb = algorithm_. and (Rrob_notxi, Rloop);
					
					// Делаем третий шаг алгоритма.
					DdNode * Rrob_xi = algorithm_. and (Rrob, algorithm_.bddVariables().zx[xInputVarNum]);

					// Делаем четвертый шаг алгоритма.
					ProductsString productsString = algorithm_.graphToProductsString(Rrob_xi);
					std::vector<std::string> products = productsString.products;
					for (auto & iter = products.begin(); iter < products.end(); ++iter) {
						for (int i = 0; i < states; ++i) {
							(*iter)[i] = '-';
						}
					}
					productsString.products = products;
					DdNode * Rrob_xi_input = algorithm_.getBddByProductsString(productsString);

					// Делаем пятый шаг алгоритма.
					std::vector<std::string> productsStringRbStates;
					std::vector<std::string> productsStringRb = algorithm_.graphToProductsString(Rb).products;
					for (auto iter = productsStringRb.begin(); iter < productsStringRb.end(); ++iter) {
						productsStringRbStates.push_back(iter->substr(0, states));
					}

					// Делаем шестой шаг алгоритма.
					ProductsString productsStringRBeforeLoop;
					for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
						if (std::find(productsStringRbStates.begin(), productsStringRbStates.end(), iter->output.state) != productsStringRbStates.end()) {
							productsStringRBeforeLoop.products.push_back(iter->input.state + iter->input.in);
						}
					}
					DdNode * RBeforeLoop = algorithm_.getBddByProductsString(productsStringRBeforeLoop);

					// Делаем седьмой шаг алгоритма.
					DdNode * RBeforeTransition = algorithm_. and (RBeforeLoop, Rrob_xi_input);

					// Реализуем восьмой шаг алгоритма.
					DdNode * Rs0 = algorithm_.removeInputsVariables(RBeforeTransition);

					// Реализуем девятый шаг алгоритма.
					std::pair<std::vector<std::string>, std::string> result = algorithm_.transferSequence(Rs0,
						data_.kiss2().initialState(),
						data_.pla(),
						100);
					std::vector<std::string> transferSequence = result.first;
					if (result.first.size() != 0) {

						// Реализуем десятый шаг алгоритма.
						std::string state = result.second;
						for (int i = 0; i < states; ++i) {
							state = "-" + state;
						}
						ProductsString productsString;
						productsString.products.push_back(state);
						DdNode * kon = algorithm_.getBddByProductsString(productsString);
						DdNode * konMultRBeforeTransition = algorithm_. and (RBeforeTransition, kon);
						productsString = algorithm_.graphToProductsString(konMultRBeforeTransition);
						std::string input = "";
						for (int i = 0; i < inputs; ++i) {
							if (productsString.products[0][i] != '-') {
								input = input + productsString.products[0][i];
							}
							else {
								input = input + '0';
							}
						}
						transferSequence.push_back(input);

						// Реализауем одинадцатый шаг алгоритма.
						std::string nextState = "";
						for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
							if (iter->input.state == state) {
								for (int i = 0; i < iter->input.in.size(); ++i) {
									if (iter->input.in[i] != '-' && iter->input.in[i] != input[i]) {
										break;
									}
									if (iter->input.in.size() - 1 == i) {
										nextState = iter->output.state;
									}
								}
							}
						}

						// Реализуем двенадцатый шаг алгоритма.
						// input + nextState.

						// Реализуем тринадцатый шаг алгоритма.
						if (input[xInputVarNum] == '0') {
							input[xInputVarNum] = '1';
						}
						else {
							input[xInputVarNum] = '0';
						}
						transferSequence.push_back(input);

						// И так, конец случая когда xi в input для falling transition.
						// transferSequence обнаруживает задержку.
						fallingTransition = true;
					}
				}
				std::cout << "Test path from input x. " << "Rising transition - " << risingTransition << ", falling transition - " <<
					fallingTransition << ". Result - " << (risingTransition && fallingTransition) << "...\n";
			} else {
				// Когда вход среди переменных состояний
				if (!risingTransition) {
					// rising transition
					// Далее делаем первый шаг, а именно выбираем а тесты.
					int zInputVarNum = (testPath->input_ <= inputs) ? states + testPath->input_ - 1 : testPath->input_ - inputs - 1;
					DdNode * Rrob_zi = algorithm_.and(Rrob, algorithm_.bddVariables().zx[zInputVarNum]);
					// Конец первого шага.

					// Строим Rсоседи - Rnb (neighbors).
					ProductsString productsString;
					ProductsString productsStringRnbStates;

					for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
						bool nb = true;
						for (int i = 0; i < iter->input.state.size(); ++i) {
							if (i == zInputVarNum) {
								if ((iter->input.state[i] == '0' && iter->output.state[i] == '1') ||
									(iter->input.state[i] == '1' && iter->output.state[i] == '0')) {
									continue;
								}
								else {
									nb = false;
									break;
								}
							}
							else {
								// При условии что в состояниях kiss2 нет черточек.
								if (iter->input.state[i] != iter->output.state[i]) {
									nb = false;
									break;
								}
							}
							if (nb) {
								productsString.products.push_back(iter->input.state + iter->input.in);
								productsStringRnbStates.products.push_back(iter->input.state);
							}
						 }
					}
					DdNode * Rnb = algorithm_.getBddByProductsString(productsString);

					// Далее делаем второй шаг алгоритма.
					DdNode * Ra = algorithm_.and(Rrob_zi, Rnb);

					// Далее делаем третий шаг алгоритма.
					ProductsString productsStringRa = algorithm_.graphToProductsString(Ra);
					ProductsString productsStringRs0;
					for (auto iter = productsStringRa.products.begin(); iter < productsStringRa.products.end(); ++iter) {
						std::string state = (*iter).substr(0, states);
						if (std::find(productsStringRnbStates.products.begin(), productsStringRnbStates.products.end(), state) !=
							productsStringRnbStates.products.end()) {
							productsStringRs0.products.push_back(state);
						}
					}
					DdNode * Rs0 = algorithm_.getBddByProductsString(productsStringRs0);

					// Далее делаем четвертый шаг алгоритма.
					std::pair<std::vector<std::string>, std::string> result = algorithm_.transferSequence(Rs0,
						data_.kiss2().initialState(),
						data_.pla(),
						100);
					std::vector<std::string> transferSequence = result.first;
					if (result.first.size() != 0) {

						// Далее делаем пятый шаг алгоритма.
						std::string state = result.second;
						for (int i = 0; i < inputs; ++i) {
							state = state + "-";
						}
						ProductsString productsString;
						productsString.products.push_back(state);
						DdNode * kon = algorithm_.getBddByProductsString(productsString);
						DdNode * konMultRBeforeTransition = algorithm_. and (Ra, kon);
						productsString = algorithm_.graphToProductsString(konMultRBeforeTransition);
						std::string input = "";
						for (int i = 0; i < inputs; ++i) {
							if (productsString.products[0][states + i] != '-') {
								input = input + productsString.products[0][i];
							}
							else {
								input = input + '0';
							}
						}
						transferSequence.push_back(input);

						// Далее делаем шестой шаг алгоритма.
						if (input[zInputVarNum] == '0') {
							input[zInputVarNum] = '1';
						}
						else {
							input[zInputVarNum] = '0';
						}
						transferSequence.push_back(input);

						risingTransition = true;
					}
				}
				if (!fallingTransition) {
					// falling transition
					// Далее делаем первый шаг, а именно выбираем а тесты.
					int zInputVarNum = (testPath->input_ <= inputs) ? states + testPath->input_ - 1 : testPath->input_ - inputs - 1;
					DdNode * notZi = Cudd_Not(algorithm_.bddVariables().zx[zInputVarNum]);
					DdNode * Rrob_notzi = algorithm_. and (Rrob, notZi);
					// Конец первого шага.

					// Строим Rсоседи - Rnb (neighbors).
					ProductsString productsString;
					ProductsString productsStringRnbStates;

					for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
						bool nb = true;
						for (int i = 0; i < iter->input.state.size(); ++i) {
							if (i == zInputVarNum) {
								if ((iter->input.state[i] == '0' && iter->output.state[i] == '1') ||
									(iter->input.state[i] == '1' && iter->output.state[i] == '0')) {
									continue;
								}
								else {
									nb = false;
									break;
								}
							}
							else {
								// При условии что в состояниях kiss2 нет черточек.
								if (iter->input.state[i] != iter->output.state[i]) {
									nb = false;
									break;
								}
							}
							if (nb) {
								productsString.products.push_back(iter->input.state + iter->input.in);
								productsStringRnbStates.products.push_back(iter->input.state);
							}
						}
					}
					DdNode * Rnb = algorithm_.getBddByProductsString(productsString);

					// Далее делаем второй шаг алгоритма.
					DdNode * Ra = algorithm_. and (Rrob_notzi, Rnb);

					// Далее делаем третий шаг алгоритма.
					ProductsString productsStringRa = algorithm_.graphToProductsString(Ra);
					ProductsString productsStringRs0;
					for (auto iter = productsStringRa.products.begin(); iter < productsStringRa.products.end(); ++iter) {
						std::string state = (*iter).substr(0, states);
						if (std::find(productsStringRnbStates.products.begin(), productsStringRnbStates.products.end(), state) !=
							productsStringRnbStates.products.end()) {
							productsStringRs0.products.push_back(state);
						}
					}
					DdNode * Rs0 = algorithm_.getBddByProductsString(productsStringRs0);

					// Далее делаем четвертый шаг алгоритма.
					std::pair<std::vector<std::string>, std::string> result = algorithm_.transferSequence(Rs0,
						data_.kiss2().initialState(),
						data_.pla(),
						100);
					std::vector<std::string> transferSequence = result.first;
					if (result.first.size() != 0) {

						// Далее делаем пятый шаг алгоритма.
						std::string state = result.second;
						for (int i = 0; i < inputs; ++i) {
							state = state + "-";
						}
						ProductsString productsString;
						productsString.products.push_back(state);
						DdNode * kon = algorithm_.getBddByProductsString(productsString);
						DdNode * konMultRBeforeTransition = algorithm_. and (Ra, kon);
						productsString = algorithm_.graphToProductsString(konMultRBeforeTransition);
						std::string input = "";
						for (int i = 0; i < inputs; ++i) {
							if (productsString.products[0][states + i] != '-') {
								input = input + productsString.products[0][i];
							}
							else {
								input = input + '0';
							}
						}
						transferSequence.push_back(input);

						// Далее делаем шестой шаг алгоритма.
						if (input[zInputVarNum] == '0') {
							input[zInputVarNum] = '1';
						}
						else {
							input[zInputVarNum] = '0';
						}
						transferSequence.push_back(input);

						fallingTransition = true;
					}
				}
				std::cout << "Test path from input z. " << "Rising transition - " << risingTransition << ", falling transition - " <<
					fallingTransition << ". Result - " << (risingTransition && fallingTransition) << "...\n";
			}
		}
	}
	return true;
}
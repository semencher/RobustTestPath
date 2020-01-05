#include "manager.h"

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
	while (data_.nextTestPaths() != -2) {
		std::vector<TestPath> testPaths = data_.currentTestPaths();

		// Идем по тестам для каждого пути.
		for (auto testPath = testPaths.begin(); testPath < testPaths.end(); ++testPath) {
			// Дальше необходимо построить Rrob.
			algorithm_.createBddVariables(data_.pla().inputs() - data_.pla().states(), data_.pla().states());
			algorithm_.createBddTransitionFunction(data_.pla());

			int inputs = data_.pla().inputs() - data_.pla().states();
			int states = data_.pla().states();
			ProductsString productsStringRrob;
			productsStringRrob = algorithm_.setInputsAfterStates(testPath->tests_, inputs,	states);

			DdNode * Rrob = algorithm_.getBddByProductsString(productsStringRrob);
			
			// Далее делаем первый шаг, а именно выбираем а тесты.
			int xInputVarNum = (testPath->input_ <= inputs) ? states + testPath->input_ - 1 : testPath->input_ - inputs - 1;
			DdNode * Rrob_xi = algorithm_.and(Rrob, algorithm_.bddVariables().zx[xInputVarNum]);
			// Конец первого шага.

			// Построим Rпетли.
			ProductsString productsStringRloop;
			std::vector<ElementK> kiss2Elements = data_.kiss2().elements();
			for (auto iter = kiss2Elements.begin(); iter < kiss2Elements.end(); ++iter) {
				if (iter->input.state == iter->output.state) {
					productsStringRloop.products.push_back(iter->input.state + iter->input.in);
				}
			}
			DdNode * Rloop = algorithm_.getBddByProductsString(productsStringRloop);

			// Делаем второй шаг алгоритма.
			DdNode * Ra = algorithm_.and(Rrob_xi, Rloop);

			// Делаем третий шаг алгоритма.
			DdNode * notXi = Cudd_Not(algorithm_.bddVariables().zx[xInputVarNum]);
			Cudd_Ref(notXi);
			DdNode * Rrob_notxi = algorithm_.and(Rrob, notXi);

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
			DdNode * RBeforeTransition = algorithm_.and(RBeforeLoop, Rrob_notxi_input);

			// Реализуем восьмой шаг алгоритма.
			DdNode * Rs0 = algorithm_.removeInputsVariables(RBeforeTransition);

			// Реализуем девятый шаг алгоритма.
			std::pair<std::vector<std::string>, std::string> result = algorithm_.transferSequence(	Rs0,
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
				DdNode * konMultRBeforeTransition = algorithm_.and(RBeforeTransition, kon);
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

				// И так, конец случая когда xi в input для rising transition.
				// transferSequence обнаруживает задержку.
				// Дальше случай для falling transition...


				





			}
			else {
				// Теста не существует.
			}




		}
	}
	return true;
}
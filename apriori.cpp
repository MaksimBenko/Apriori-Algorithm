#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <functional>

using namespace std;

// Максимальное количество элементов в транзакции;
const unsigned int numOfProducts = 100;

// В процентах;
double CalculateSupport(unsigned int counter, unsigned int numOfTransactionsTotal) {
	return (double)counter / numOfTransactionsTotal * 100.0;
}

// Вывод подсказки;
void ShowUsageAndExit() {
	cout << "Usage: ./apriori <rules filename> <support> <sort type> <max rules on screen>\n";
	cout << "<rules filename>: path to file with rules\n\t<support>: double, (0..100)%\n\t<sort type>: 'u' - upscending, 'd' - downscending, 'n' - no sort\n\t<max rules on screen>: int, -1 - all\n";
	exit(1);
}

class RuleWithSupport {
	public:
		RuleWithSupport() {
			counter = 0;
			rules.clear();
		}
		RuleWithSupport(set<short> rulesIn) {
			rules = rulesIn;
			counter = 0;
		}
		~RuleWithSupport() {
			rules.clear();
		}
		
		bool operator < (const RuleWithSupport &secondRule) const {
			return (counter < secondRule.counter);
		}

		bool operator > (const RuleWithSupport &secondRule) const {
			return (counter > secondRule.counter);
		}

		set<short> rules;	// Набор правил;
		unsigned int counter;	// Количество вхождений в транзакции;
};

// Инициализация вектора едичных правил;
void InitC1Itemset(vector<RuleWithSupport> &c1Itemset) {
	for(int pos = 0; pos < c1Itemset.size(); ++pos) {
		RuleWithSupport currentRuleWithSupport;
		c1Itemset[pos].rules.insert(pos);
	}
}
// Вывод правил на экран
void PrintRules(vector<RuleWithSupport> rules, int maxNumOfRulesIn, int numOfTransactionsTotal) {

	cout << "Result: \nTotal number of rules found: " << rules.size() << endl;
	int iter = 0;
	int maxNumOfRules = maxNumOfRulesIn;
	if(maxNumOfRules < 0) {
		maxNumOfRules = rules.size();
	}
	for(vector<RuleWithSupport>::iterator vectIt = rules.begin(); (vectIt < rules.end()) && (iter < maxNumOfRules); ++vectIt, ++iter) {
		for(set<short>::iterator setIt = vectIt->rules.begin(); setIt != vectIt->rules.end(); ++setIt) {
			cout << *setIt << " ";
		}
		cout << "\t| " << CalculateSupport(vectIt->counter, numOfTransactionsTotal) << "%\n";
	}
}

// Считываение таблицы из файла и заполнение массива с первым набором данных
void FillRulesVectorFromFile(vector<set<short>> &rulesVector, vector<RuleWithSupport> &c1Itemset, char* rulesFileName) {
	ifstream rulesFileStream;
	rulesFileStream.open(rulesFileName);
	
	if (!rulesFileStream.is_open()) {
        	cout << "Unable to find or to open file with rules!\n";
    		exit(1);
	}
	
	unsigned int ruleIndex = 0;
	string tempString;
	while(getline(rulesFileStream,tempString)) {
		set<short> currentRuleSet;
		for(short pos = 0; pos < tempString.length(); ++pos) {
			if(tempString[pos] == '1') {
				currentRuleSet.insert(pos);
				++(c1Itemset[pos].counter);
				//c1Itemset[pos].rules = currentRuleSet;
			//	++c1Itemset[pos];
			}
		}
		rulesVector.push_back(currentRuleSet);
	}
	rulesFileStream.close();

}

// Создаёт вектор с набором новых правил, поддержку которых затем необходимо проверить
void GenerateNewRules(vector<RuleWithSupport> &validRules, vector<RuleWithSupport> &resultRules) {
	vector<RuleWithSupport>::iterator itFirst, itSecond;
	set<short> tmpSet;
	itSecond = validRules.begin() + 1;
	
	// Для создания новых подмножеств, все элементы старых элементов (кроме последнего) должны совпадать. Для одного элемента - это пусое множество. 
	for(itFirst = validRules.begin(); itFirst != validRules.end(); ++itFirst) {
		for(itSecond = itFirst + 1; 
				(itSecond != validRules.end()) && ((*(++(itSecond->rules.rbegin())) == *(++(itFirst->rules.rbegin())) || (itFirst->rules.size() == 1) )); ++itSecond) {
			tmpSet.clear();
			tmpSet = itFirst->rules;
			tmpSet.insert(*itSecond->rules.rbegin());	
			RuleWithSupport currentRule(tmpSet);
			resultRules.push_back(currentRule);
		}
	}
}

// Расчёт поддержки массива правил;
void CalculateSupportForRules(vector<RuleWithSupport> &rulesToCheck, vector<set<short>>& transactionsVector) {
	bool allElementsPresented = true;
	for(vector<RuleWithSupport>::iterator rulesIt = rulesToCheck.begin(); rulesIt != rulesToCheck.end(); ++rulesIt) {
		for(vector<set<short>>::iterator transIt = transactionsVector.begin(); transIt != transactionsVector.end(); ++transIt) {
			allElementsPresented = true;
			
			for(set<short>::iterator elementInRuleToCheckIt = rulesIt->rules.begin(); elementInRuleToCheckIt != rulesIt->rules.end(); 
					++elementInRuleToCheckIt) {				
				if(transIt->find(*elementInRuleToCheckIt) == transIt->end()) {
					allElementsPresented = false;
					break;
				}	
			}
			if(allElementsPresented) {
				++(rulesIt->counter);
			}
		}
	}
}

// Поиск всех правил с поддержкой, не меньше требуемой;
void ProceedItemsets(vector<set<short>> transactions, double support, vector<RuleWithSupport> &validRulesVector, vector<RuleWithSupport> c1Itemset) {
	vector<RuleWithSupport> oldRules, currentValidRules;//, newRules;
	oldRules = c1Itemset;

	vector<RuleWithSupport>::iterator it;

	while(oldRules.size() > 0) {
		currentValidRules.clear();
		for(it = oldRules.begin(); it != oldRules.end(); it++) {
			if(CalculateSupport(it->counter, transactions.size()) >= support) {
				validRulesVector.push_back(*it);
				currentValidRules.push_back(*it);
				
			}
		}
		oldRules.clear();
		GenerateNewRules(currentValidRules, oldRules);	
		CalculateSupportForRules(oldRules, transactions);
	}
	
}

// Сортировка вектора с результатом;
void SortResult(vector<RuleWithSupport>& validResult, char sortType) {
	switch(sortType) {
		case 'u':
			std::sort(validResult.begin(), validResult.end());
			break;
		case 'd':
			std::sort(validResult.rbegin(), validResult.rend());//, SortByCounterDescending);
			break;
	}	
}

int main(int argc, char** argv) {
	vector<set<short>> transactionsVector;			// Вектор транзакций;
	vector<RuleWithSupport> c1Itemset(numOfProducts);	// Вектор с \"единичным\" набором правил (для простоты количество элементов захардкожено, но можно в рассчитывать run-time); 
	vector<RuleWithSupport> validRules;			// Вектор со всеми правилами с поддержкой неменьшей заданной;
	
	int paramCounter = 1;
	
	// Проверка количества аргументов;
	if(argc != 5) {
		ShowUsageAndExit();
	}
	
	// Определение величины поддержки;
	double support = atof(argv[2]);
	if((support < double(0)) || (support > double(100))) {
		cout << "Error! Support must be in range (0..100)%!\n";
		ShowUsageAndExit();
	}

	// Проверка сортировки;
	// Можно добавить проверку на длину слова.
	char sortType = argv[3][0];
	if((sortType != 'u') && (sortType != 'd') && (sortType != 'n')) {
		cout << "Wrong sort type!\n";
		ShowUsageAndExit();
	}

	int maxNumOfRules = atoi(argv[4]);

	cout << "Input data:\n";
	cout << "Support = " << support << '%' << endl;
	cout << "SortType = " << sortType << endl;
	cout << "MaxNumOfRules = " << maxNumOfRules << endl;
	cout << "_________________________________\n\n";

	InitC1Itemset(c1Itemset);						// Инициализация вектора начальных элементов;
	FillRulesVectorFromFile(transactionsVector, c1Itemset, argv[1]);	// Заполнение вектора нчальных элементов;
	ProceedItemsets(transactionsVector, support, validRules, c1Itemset);	// Поиск правил с поддержкой, не меньше требуемой;
	SortResult(validRules, sortType);					// Сортировка результата;
	PrintRules(validRules, maxNumOfRules, transactionsVector.size());	// Вывод результата на экран;

	return 0;
}

/*

0-1 knapsack genetic algorithm

*/

#include <iostream>
#include <queue>
#include <string>
#include <algorithm>
#include <ctime>

using namespace std;

int knapsackVolume;
int subjects;
int populationInitSize;

double percentageOfParentsInTheNewPopulationFromBest;
double percentageOfParentsInTheNewPopulationFromWorst;
double percentageToExtend;

double percentageToFillWithNewChromosomes;
double percentageFromChildrenToMutate;

int iterationLevels;

struct subject {
	int weight;
	int price;
};

subject arr[256];

struct chromosome {
	string genes;
	int overallWeight;
	int overallPrice;

	void calculateOverallWeight() {
	    overallWeight = 0;
		for (int i = 0; i < subjects; ++i)
            overallWeight += (genes[i] == '1') ? arr[i].weight : 0;
	}

	void calculateOverallPrice() {
	    overallPrice = 0;
		for (int i = 0; i < subjects; ++i)
			overallPrice += (genes[i] == '1') ? arr[i].price : 0;
	}

	chromosome(string genes) {
	    this->genes = genes;
		calculateOverallPrice();
		calculateOverallWeight();
	}

	int fitness() {
		return overallPrice;
	}

	bool isValid() {
		calculateOverallWeight();
		if (overallWeight > knapsackVolume) return false;
		calculateOverallPrice();
		return true;
	}

	bool operator < (chromosome& right) {
        return fitness() > right.fitness();
	}

	void print() {
        for (int i = 0; i < subjects; ++i) {
            if (genes[i] == '1') {
                cout << arr[i].weight << " " << arr[i].price << endl;
            }
        }
	}
};

vector<chromosome> population;

void selection() {

	int sz = 0;
	string genes;

    vector<int> indexes;
    for (int i = 0; i < subjects; ++i) indexes.push_back(i);

    while (sz < populationInitSize) {

		string genes(subjects, '0');
		int volume = 0;
		int i;

        random_shuffle(indexes.begin(), indexes.end());

		for (i = 0; i < subjects && volume < knapsackVolume; ++i) {
			int idx = indexes[i];
			genes[idx] = '0' + rand() % 2;
			if (genes[idx] == '1') volume += arr[idx].weight;
        }

		chromosome chr(genes);
		if (chr.isValid()) {
			population.push_back(chr);
			// cout << chr.genes << " " << chr.fitness() << endl;
			++sz;
		}
    }

    cout << "hi\n";

	sort(population.begin(), population.end());
}

void mutateChromosome(chromosome& ch) {

	int numberOfMutatedGenes = 1; // OR rand() % subjects

	if (numberOfMutatedGenes > 1) {
        vector<int> indexes;
        for (int i = 0; i < subjects; ++i)
            indexes.push_back(i);

        int sz = subjects;
        for (int i = 0; i < numberOfMutatedGenes; ++i) {
            int idx = indexes[rand() % sz];
            ch.genes[idx] = '1' + '0' - ch.genes[idx];
            indexes.erase(indexes.begin() + idx);
            sz--;
        }
	} else {
	    int idx = rand() % subjects;
        ch.genes[idx] = '1' + '0' - ch.genes[idx];
	}

}

void initParameters() {

	percentageOfParentsInTheNewPopulationFromBest = 0.65;
	percentageOfParentsInTheNewPopulationFromWorst = 0.05;

	percentageToExtend = 0.0;

	percentageToFillWithNewChromosomes =
        1 - percentageOfParentsInTheNewPopulationFromBest
          - percentageOfParentsInTheNewPopulationFromWorst + percentageToExtend;

	percentageFromChildrenToMutate = 0.20;
	populationInitSize = 700;
	iterationLevels = 12000;

}

vector<chromosome> childrenPopulation;

// Partially crossing-over
void crossChromosomes(chromosome &chr1, chromosome &chr2) {
	int idx = rand() % subjects;

	string resultedGenes;
	resultedGenes = "";
	for (int i = 0; i <= idx; ++i) resultedGenes += chr1.genes[i];
	for (int i = idx + 1; i < subjects; ++i) resultedGenes += chr2.genes[i];

    chromosome g1(resultedGenes);
    if (g1.isValid()) {
        childrenPopulation.push_back(g1);
    }

    resultedGenes = "";
	for (int i = 0; i <= idx; ++i) resultedGenes += chr2.genes[i];
	for (int i = idx + 1; i < subjects; ++i) resultedGenes += chr1.genes[i];

	chromosome g2(resultedGenes);
	if (g2.isValid()) {
        childrenPopulation.push_back(g2);
	}

}

void crossoverSomeParents() {

	int populationCurrSize = population.size();
	int childrenToForm = percentageToFillWithNewChromosomes * populationCurrSize;
	int i1, i2;

	childrenPopulation.clear();
	while (childrenPopulation.size() < childrenToForm) {

		do {
			i1 = rand() % populationCurrSize;
			i2 = rand() % populationCurrSize;

            crossChromosomes(population[i1], population[i2]);
		} while (i1 == i2);

	}

}

void formNewPopulation() {

	vector<chromosome> newPopulation;

	int populationSize = population.size();

	for (int i = 0; i < percentageOfParentsInTheNewPopulationFromBest * populationSize; ++i) {
		newPopulation.push_back(population[i]);
	}

	for (int i = populationSize - 1;
           i >= (1 - percentageOfParentsInTheNewPopulationFromWorst) * populationSize && i >= 0; --i) {
        newPopulation.push_back(population[i]);
	}

	for (int i = 0; i < childrenPopulation.size(); ++i) {
		newPopulation.push_back(childrenPopulation[i]);
	}

	population = newPopulation;
	sort(population.begin(), population.end());

}

void mutateSomeChildren() {

    int mutatedChildren = percentageFromChildrenToMutate * childrenPopulation.size();

    for (int i = 0; i < mutatedChildren; ++i) {
        int idx = rand() % childrenPopulation.size();

        mutateChromosome(childrenPopulation[idx]);
        if (! childrenPopulation[idx].isValid()) {
            childrenPopulation.erase(childrenPopulation.begin() + idx);
        }
    }
}

void read() {

	scanf("%d", &knapsackVolume);
	scanf("%d", &subjects);

	for (int i = 0; i < subjects; ++i) {
		scanf("%d%d", &arr[i].weight, &arr[i].price);
	}

}

void solveGenetically() {

	initParameters();
	selection();

	int bestSolution;
	int i;
	int SOL = 1130;

	for (i = 0; population[0].fitness() < SOL; ++i) {

		crossoverSomeParents();
		mutateSomeChildren();
		formNewPopulation();

		bestSolution = population[0].fitness();
		// cout << "Current best: " << population[0].fitness() << endl;
	}

    cout << "Steps: " << i << endl;

	// bestSolution = population[0].price;
	cout << "Best solution: ";
	cout << bestSolution << endl;
	population[0].print();
}

int main() {

    srand(time(NULL));
	read();
	solveGenetically();

	return 0;
}

/*
5000 24
90 150
130 35
1530 200
500 160
150 60
680 45
270 60
390 40
230 30
520 10
110 70
320 30
240 15
480 10
730 40
420 70
430 75
220 80
70 20
180 12
40 50
300 10
900 1
2000 150
*/

/*
Answer to the long test data 5072 (the right answer is 5119):

Steps: 80294
Best solution: 5072
44 252
667 485
426 477
270 245
632 479
328 371
339 386
344 334
97 91
85 484
481 404
671 419
224 215
362 430

*/

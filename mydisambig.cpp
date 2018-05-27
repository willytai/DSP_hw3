#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include "Ngram.h"
using namespace std;

struct Node
{
	Node(string& k) : _prob(0.0) { _key = k; }
	~Node() {}

	void setProb(double prob) { _prob = prob; }
	double getProb() { return _prob; }
	string Key() { return _key; }

	string _key;
	double _prob;
};

class Graph
{
public:
	Graph(Ngram* m, Vocab& v) { _lm = m; _voc = v; }
	~Graph() {
		for (int i = 0; i < _graph.size(); ++i) {
			for (int j = 0; j < _graph[i].size(); ++j)
				delete _graph[i][j];
		}
	}

	void addNodes(vector<Node*>& GuoNodes) { _graph.push_back(GuoNodes); }
	
	void compute() {
		// if (_graph[0].size() != 1) {
		// 	assert(_graph[1].size() == 1);
		// 	Node*& ref = _graph[1][0];
		// 	double maxprob = -1.0/0.0;
		// 	int max = 0;
		// 	for (int candidate = 0; candidate < _graph[0].size(); ++candidate) {
		// 		double new_prob = getProb(_graph[1][0]->Key().c_str(), _graph[0][candidate]->Key().c_str());
		// 		if (maxprob < new_prob) {
		// 			maxprob = new_prob;
		// 			max = candidate;
		// 		}
		// 	}
		// 	_graph[0][max]->setProb(maxprob);
		// } else _graph[0][0]->setProb(94.87);

		for (int candidate = 0; candidate < _graph[0].size(); ++candidate)
			_graph[0][candidate]->setProb(getProb("<s>", _graph[0][candidate]->Key().c_str()));

		for (int current = 1; current < _graph.size(); ++current) {
			for (int candidate = 0; candidate < _graph[current].size(); ++candidate) {
				double maxprob = -1.0/0.0;
				Node*& w2      = _graph[current][candidate];
				for (int previous = 0; previous < _graph[current-1].size(); ++previous) {
					Node*& w1       = _graph[current-1][previous];
					double pre_prob = _graph[current-1][previous]->getProb();
					double new_prob = getProb(w1->Key().c_str(), w2->Key().c_str()) + pre_prob;

					if (maxprob < new_prob)	maxprob = new_prob;
				}
				w2->setProb(maxprob);
			}
		}
	}

	void trace() {
		for (int i = 0; i < _graph.size(); ++i) {
			int max = 0;
			for (int j = 1; j < _graph[i].size(); ++j) {
				if (_graph[i][max]->getProb() < _graph[i][j]->getProb())
					max = j;
			}
			_result.push_back(max);
		}



		// debug
		// cout << "CHECK" << endl;
		// for (int i = 0; i < _graph.size(); ++i)
		// 	cout << setw(5) << _graph[i].size() << ' ';
		// cout << endl;
		// for (int i = 0; i < _graph.size(); ++i)
		// 	cout << setw(5) << _result[i] << ' ';
		// cout << endl;
	}

	void print() {
		cout << "<s>";
		for (int i = 0; i < _graph.size(); ++i)
			cout << " " << _graph[i][_result[i]]->Key();
		cout << " </s>" << endl;
	}

private:
	vector<vector<Node*> > _graph;
	vector<int>            _result;
	Ngram* 				   _lm;
	Vocab                  _voc;


	double getProb(const char* w1, const char* w2) {
		VocabIndex wd1 = _voc.getIndex(w1);
		VocabIndex wd2 = _voc.getIndex(w2);

		if (wd1 == Vocab_None)
			wd1 = _voc.getIndex(Vocab_Unknown);
		if (wd2 == Vocab_None)
			wd2 = _voc.getIndex(Vocab_Unknown);

		VocabIndex context[] = { wd1, Vocab_None };

		return _lm->wordProb(wd2, context);
	}
};

void ReadMap(string filename, map<string, vector<string> >& ZhuYinMap) {
	ifstream file(filename.c_str());
	string buf;
	while (getline(file, buf)) {
		if (buf == "") break;

		istringstream ss(buf);
		string ZhuYin, Guo;

		ss >> ZhuYin;

		while (ss >> Guo) {
			ZhuYinMap[ZhuYin].push_back(Guo);
		}
	}
}

int main(int argc, char const *argv[]) {
	
	string       test(argv[2]);
	string       ZhuYin(argv[4]);
	const char*  LM(argv[6]);
	int          order(argv[8][0]-'0');

	// comment these out for submission
	cerr << "testing file:   " << test << endl;
	cerr << "map:            " << ZhuYin << endl;
	cerr << "language model: " << LM << endl;
	cerr << "order:          " << order << endl << endl;

	Vocab  voc;
	Ngram* lm = new Ngram(voc, order);

	File lmFile(argv[6], "r");
	lm->read(lmFile);
	lmFile.close();	

	map<string, vector<string> > ZhuYinMap;
	ReadMap(ZhuYin, ZhuYinMap);

	ifstream file(test.c_str());
	string buf;
	while (getline(file, buf)) {
		if (buf == "") break;
		
		Graph GG(lm, voc);

		istringstream ss(buf);
		string Guo;

		while (ss >> Guo) {
			vector<Node*> tmp;
			for (int i = 0; i < ZhuYinMap[Guo].size(); ++i)
				tmp.push_back(new Node(ZhuYinMap[Guo][i]));
			GG.addNodes(tmp);
		}
		GG.compute();
		GG.trace();
		GG.print();
	}

	return 0;
}
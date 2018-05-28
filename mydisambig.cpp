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
	Node(string& k) : _prob(-1.0/0.0), _parent(NULL) { _key = k; }
	~Node() {}

	void setProb(double prob) { _prob = prob; }
	void setParent(Node* p) { _parent = p; }
	double getProb() { return _prob; }
	string Key() { return _key; }

	string _key;
	double _prob;
	Node*  _parent;
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

	void addNodes(vector<Node*>& GuoNodes) { _graph.push_back(GuoNodes); update(); }
	
	void update() {
		if (_graph.size() == 1) {
			for (int candidate = 0; candidate < _graph[0].size(); ++candidate) {
				double log_prob = getProb("<s>", _graph[0][candidate]->Key().c_str());
				_graph[0][candidate]->setProb(log_prob);
			}
			return;
		}

		int previous = _graph.size() - 2;
		int current  = previous + 1;

		for (int candidate = 0; candidate < _graph[current].size(); ++candidate) {
			double max_prob = -1.0/0.0;
			int maxid = -1;
			for (int pre = 0; pre < _graph[previous].size(); ++pre) {
				Node*& origin_node = _graph[previous][pre];
				double origin_prob = _graph[previous][pre]->_prob;
				double new_prob = origin_prob + getProb(origin_node->Key().c_str(), _graph[current][candidate]->Key().c_str());
				if (max_prob < new_prob) {
					max_prob = new_prob;
					maxid    = pre;
				}
			}
			_graph[current][candidate]->setProb(max_prob);
			_graph[current][candidate]->setParent(_graph[previous][maxid]);
		}

	}

	void print() {
		Node* n;
		double max = -1.0/0.0;
		for (int i = 0; i < _graph.back().size(); ++i) {
			if (_graph.back()[i]->getProb() > max) {
				max = _graph.back()[i]->getProb();
				n   = _graph.back()[i];
			}
		}

		vector<string> tmp;
		cout << "<s>";
		while(true) {
			if (!n) break;
			tmp.push_back(n->Key());
			n = n->_parent;
		}
		for (int i = tmp.size()-1; i >= 0; --i)
			cout << " " << tmp[i];
		cout << " </s>" << endl;
	}

private:
	vector<vector<Node*> > _graph;
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

		while (ss >> Guo)
			ZhuYinMap[ZhuYin].push_back(Guo);
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

		GG.print();
	}

	return 0;
}
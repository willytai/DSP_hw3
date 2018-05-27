// #include <iostream>
// #include <string>
// #include <stdio.h>
// #include "Ngram.h"
// using namespace std;

// int main(int argc, char const *argv[]) {
	
// 	string       testfile(argv[2]);
// 	string       ZhuYinMap(argv[4]);
// 	const char*  LM(argv[6]);
// 	int          order(argv[8][0]-'0');

// 	cout << "testing file:   " << testfile << endl;
// 	cout << "map:            " << ZhuYinMap << endl;
// 	cout << "language moedl: " << LM << endl;
// 	cout << "order:          " << order << endl << endl;

// 	Vocab  voc;
// 	Ngram* lm = new Ngram(voc, order);

// 	File lmFile(argv[6], "r");
// 	lm->read(lmFile);
// 	lmFile.close();	

// 	// debug
// 	VocabIndex test1 = voc.getIndex("¨â");
//     VocabIndex test2 = voc.getIndex("¾Û");
//     VocabIndex test3 = voc.getIndex("¤@");
    
//     if(test1== Vocab_None) cout<<"none1"<<endl;
//     if(test2== Vocab_None) cout<<"none2"<<endl;
//     if(test3== Vocab_None) cout<<"none3"<<endl;
//     // cout<<getBiProb("¶}" , "徜" , voc ,  &lm)<<endl;
//     // cout<<getBiProb("¶}", "£¥" , voc ,  &lm)<<endl;


// 	return 0;
// }
#include <stdio.h>
#include "Ngram.h"

int main(int argc, char *argv[])
{

    int ngram_order = 3;
    Vocab voc;
    Ngram lm( voc, ngram_order );

    {
        const char lm_filename[] = "./corpus.lm";
        File lmFile( lm_filename, "r" );
        lm.read(lmFile);
        lmFile.close();
    }

    VocabIndex wid = voc.getIndex("囧");
    if(wid == Vocab_None) {
        printf("No word with wid = %d\n", wid);
        printf("where Vocab_None is %d\n", Vocab_None);
    }

    wid = voc.getIndex("患者");
    VocabIndex context[] = {voc.getIndex("癮") , voc.getIndex("毒"), Vocab_None};
    printf("log Prob(患者|毒-癮) = %f\n", lm.wordProb(wid, context));
}
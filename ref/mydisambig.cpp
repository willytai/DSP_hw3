#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include<sstream>
#include <map>
#include <limits>
#include <algorithm>
#include "Ngram.h"
using namespace std;
// mydisambig -text testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > result2/$$i.txt 
static Vocab voc;
double getBiProb(const char*, const char*,  Ngram*);
//void readmap(const string& ,map<string, vector<string> >&  );
void readmap(char* input,map<string, vector<string> >&  zhuyinmap ){
    ifstream infile(input);
    string line;
    while (getline(infile, line)) {
        istringstream ss(line);
        string zhu, guo;
        ss >> zhu;
        while (ss >> guo) { zhuyinmap[zhu].push_back(guo); }
    }
   // zhuyinmap["<s>"].push_back("<s>");
    //zhuyinmap["<\s>"] .push_back("<\s>"); 
}
static void viterbiinit(vector<string>&  , Ngram*);
static void viterbiiter(vector<string>&  , Ngram*);
static vector<string> viterbacktrack() ;
//static Vocab voc;
int main(int argc, char *argv[])
{
    
   // Vocab voc;
   
    Ngram* lm = new Ngram( voc, atoi(argv[8]) );
    //cout<<argv[8]<<endl;

    {
        File lmFile( argv[6], "r" );
        //File lmFile( "bigram.lm", "r" );
        
        lm->read(lmFile);
        lmFile.close();
    }
    /*
    //debugging///////////////////////////////
    //cout<<"testing"<<endl;
    VocabIndex test1 = voc.getIndex("¨â");
    VocabIndex test2 = voc.getIndex("¾Û");
    VocabIndex test3 = voc.getIndex("¤H");
    
    if(test1== Vocab_None) cout<<"none1"<<endl;
    if(test2== Vocab_None) cout<<"none2"<<endl;
    if(test3== Vocab_None) cout<<"none3"<<endl;
    cout<<getBiProb("¶}" , "徜" , voc ,  &lm)<<endl;
    cout<<getBiProb("¶}", "£¥" , voc ,  &lm)<<endl;
    ////////////////////////////////////////////////////////////
    */

    map<string, vector<string> > zhuyinmap;    
    readmap(argv[4] , zhuyinmap);
     zhuyinmap["<s>"].push_back("<s>");
    ifstream testfile(argv[2]);
    string eachline;
    map<int ,vector<string> > testmap;
    int lineid = 0 ;
    while (getline(testfile , eachline)){
        //cout<<"line "<<lineid<<endl;
        bool firstword = false;
        istringstream is(eachline);
        //vector <string> testline;
        string eachword;
        while(is>> eachword){
            //cout<<eachword<<" ";
            if (firstword ==false){
                firstword = true;
                //viterbi initialize in zhuyin map
                //if(zhuyinmap.find(eachword) != zhuyinmap.end() ){
                    vector<string> zhuoptions =  zhuyinmap[eachword];
                    viterbiinit(zhuoptions  , lm);
                //}
               
            }
            else{
                //iterate viterbi
                //if(zhuyinmap.find(eachword) != zhuyinmap.end() ){
                    vector<string>& zhuoptions =  zhuyinmap[eachword];
                    viterbiiter(zhuoptions  , lm);
                //}
            }
            //testline.push_back(rawword);
        }
    
        
        vector<string> rightseq= viterbacktrack();
        
        cout<<"<s>";
        for(int  i = rightseq.size() -1 ; i >= 0  ; i--){
            cout<<" "<<rightseq[i];
        }
        cout<<" </s>"<<endl;
        firstword = false;
        ++lineid;
    }
    
    return 0 ;
}




//language model


double getBiProb(const char *w1,const char *w2, Ngram *lm)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    
    if(wid1 == Vocab_None)  //OOV
    {   //cout<<"none1"<<endl;
            wid1 = voc.getIndex(Vocab_Unknown);
    }
    if(wid2 == Vocab_None)  //OOV
    {  // cout<<"none2"<<endl;
        wid2 = voc.getIndex(Vocab_Unknown);
    }
    VocabIndex context[] = { wid1, Vocab_None };
    return lm->wordProb( wid2, context);
}
/*static double getWordProb(VocabIndex wid1, VocabIndex wid2 )
{
  VocabIndex context[] = { wid1, Vocab_None };
  return LangModel->wordProb(wid2, context);
}*/

//viterbi function///////////////////////////////////////////////////////////

static map<string , double> onewdmap;
static map<string , double >  twowdmap;
static vector<map<string, string> > psi;
static void viterbiinit(vector<string>& zhuoptions ,  Ngram *lm){
    psi.clear();
    onewdmap.clear();
    twowdmap.clear();
    
    for(int i = 0 ; i <  zhuoptions.size(); i ++){
       // cout<<"zhuopts: "<<zhuoptions[i]<<" ";
        onewdmap[zhuoptions[i]] = getBiProb("<s>" , zhuoptions[i].c_str()  ,  lm);
        //onewdmap[zhuoptions[i]] = 0.0;
    }
   // cout<<endl;
   //cout<<"zhuoptsize: "<<zhuoptions.size()<<endl;
 }

static void viterbiiter(vector<string>& zhuopts ,  Ngram *lm){
    //twowdmap.push_back(onewdmap);
  // cout<<"in"<<endl;
    
    twowdmap.clear();
    psi.push_back(std::map<string, string>());
    for(int i = 0 ; i < zhuopts.size() ; i++ ){
        double maxprob = -1.0/0.0;
        string premaxprobwd = zhuopts[0];
        //premaxprobwd = zhuopts[0];
        //cout<<"zhuoptsize: "<<zhuopts.size()<<endl;
        for(map<string , double>::iterator it = onewdmap.begin() ; it != onewdmap.end() ; it++){
            double firstprob = it->second;
            LogP biprob = getBiProb(it->first.c_str() , zhuopts[i].c_str() ,  lm);
           
            //testing
            //cout<<"preword: "<<it->first.c_str()<<"word: "<<zhuopts[i].c_str() << " biprob: "<< biprob<<endl;
            
            
            double wdprob = firstprob + biprob;
            if(wdprob == -1 * std::numeric_limits<float>::infinity()) continue;
            if(wdprob > maxprob){maxprob = wdprob; premaxprobwd = it->first; 
               // cout<<"prewordmax: "<<it->first<<endl;
            }
        }
        //cout<<"maxprob: "<<maxprob<<endl;
        
        //cout<<"preword: "<<premaxprobwd<<" word: "<<zhuopts[i] << " prob: "<< maxprob<<endl;
        if(premaxprobwd != ""){
            twowdmap[zhuopts[i]] = maxprob;
            psi.back()[zhuopts[i]] = premaxprobwd;
        }
        else {
            premaxprobwd = onewdmap.begin()->first;
            twowdmap[zhuopts[i]] = maxprob;
            psi.back()[zhuopts[i]] = premaxprobwd;
            //cout<<"key: "<<zhuopts[i]<<endl;
            //cout<<"what's inside: "<<premaxprobwd<<endl;
            abort();
        }//>..<
    }
    /*onewdmap.clear();
    for(map<string , double>::iterator it = twowdmap.begin() ; it!= twowdmap.end() ; it++){
        onewdmap[it->first] = it->second;
    }*/
    swap(onewdmap , twowdmap);  
}

vector<string> viterbacktrack(){
     double maxprob = -1.0/0.0;
     vector<string> rightseq;
     string maxwd;
      for(map<string , double>::iterator it = onewdmap.begin() ; it != onewdmap.end() ; it++){
       // cout<<it->second<<endl;
        //maxwd = 
        if(it->second > maxprob){
              maxprob = it-> second ; maxwd = it->first; 
          }
       // else {maxwd = onewdmap.begin()->first;}  
      }
      rightseq.push_back(maxwd);
    //  for(int i = 0 ; i < psi.size() ; i++){
   //    cout<<"psisize: "<<psi.size()<<endl;}
      for(int  i = psi.size()-1 ;  i>= 0  ; i--){
            
            rightseq.push_back(psi[i][maxwd]);
            //cout<<"psi"<<i<<" "<<psi[i][maxwd]<<endl;
            maxwd = psi[i][maxwd];    
        }
       /* 
       while (!psi.empty()) {
           maxwd = psi.back()[maxwd];
            psi.pop_back();
            rightseq.push_back(maxwd);
       } */
       // cout<<"rightseqsize: "<< rightseq.size()<<endl;
        /*for(int i = 0 ; i< rightseq.size() ; i++){
            cout<<rightseq[i]<<endl;
        }*/
    return rightseq;
}






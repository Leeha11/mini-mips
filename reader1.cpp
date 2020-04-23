#include<bits/stdc++.h>
using namespace std;

int clock_cycle=0;

int j;
fstream new_file;
string str;
vector <string> instr;
string word;
vector<string> tokens1;

// MIPS INSTRUCTIONS
class Instructions {
  public:
  map<string,int> labels;
  int pc=0;
  int memory[1024];
  int s[9]={0};
  int t[9]={0};
  int v[2]={0};
  int a[4]={0};
  int k[2]={0};
  int address=268500992;
  int index=0;

  string if_id;
  int id_ex[3];
  string reg_id_ex[3]; 
  int ex_mem;
  string ex_mem_str="";
  int mem_wb;
  string mem_wb_str="";
  int id_ex_arr[5],ex_mem_arr[5],mem_wb_arr[5]; 
  string stall;
  int stall_count=0;
  string nop[4]={""};

    int* assign_pointer(char c) {
      switch(c) {
        case 's':
        return s;
        case 't':
        return t;
        case 'v':
        return v;
        case 'a':
        return a;
        case 'k':
        return k;
        }
        return NULL;
      }

    int zero(string str) {
      if((str.compare("$zero")==0) || (str.compare("$0")==0))
        return 0;
      int ind=((int)str[2]-48);
        return assign_pointer(str[1])[ind];
    }

    void word(string str1) {
        int n;
        stringstream ss;
        ss << str1;
        ss >> n;
        memory[index] = n;
        index++;
    }

    void write_back() {
      if(mem_wb_arr[4]==1) {
        int ind=((int)mem_wb_str[2]-48);
        assign_pointer(mem_wb_str[1])[ind]=mem_wb;
      }
    }

    void mem() {
      if(ex_mem_arr[2]==1) 
        mem_wb=memory[ex_mem];
      else if(ex_mem_arr[3]==1) {
        int ind=((int)ex_mem_str[2]-48);
        memory[ex_mem]=assign_pointer(ex_mem_str[1])[ind];
      }
      else 
        mem_wb=ex_mem;
      for(int x=0; x<5; x++)
        mem_wb_arr[x]=ex_mem_arr[x];
      mem_wb_str=ex_mem_str;
      if(nop[2].compare("none")==0)
        nop[3]="none";
    }

    void execute() {
      if(id_ex_arr[1]==0)  
        ex_mem=id_ex[1]+id_ex[2];
      if(id_ex_arr[1]==1)                           
        ex_mem=id_ex[1]-id_ex[2]; 
      if(id_ex_arr[1]==2) 
        ex_mem=id_ex[1]<<id_ex[2];
      if(id_ex_arr[1]==3) {
        if(id_ex[1]<id_ex[2])
          ex_mem=1;
        else 
          ex_mem=0;
        }
      if(id_ex_arr[1]==4) 
        ex_mem=(id_ex[1]+id_ex[2]*4-address)/4;
      if(id_ex_arr[1]==5)
        ex_mem=id_ex[1];
      if(id_ex_arr[1]==6)
        ex_mem=id_ex[1]*4+address;
      for(int x=0; x<5; x++)
        ex_mem_arr[x]=id_ex_arr[x];
      ex_mem_str=reg_id_ex[0];
      if(nop[1].compare("none")==0)
        nop[2]="none";
    }

    void decode() {
      for(int x=0; x<5; x++)
          id_ex_arr[x]=0;
        
      if(if_id!="") {
        vector <string> tokens;
        string word;
        stringstream ss(if_id);
        while(getline(ss,word,' ')) {
            if(word=="" || word[word.length()-1]==':')
              continue;
            else 
              tokens.push_back(word);
        }
        if(tokens.size()!=0) {
            for(int i=1;i<tokens.size();i++) 
                reg_id_ex[i-1]=tokens[i];
            if(tokens[0].compare("j")==0)
                pc=labels[reg_id_ex[0]];
            else {
                id_ex[0]=zero(reg_id_ex[0]);
                if(tokens[0].compare("add")==0 || tokens[0].compare("sub")==0 || tokens[0].compare("slt")==0) {
                  id_ex[1]=zero(reg_id_ex[1]);
                  id_ex[2]=zero(reg_id_ex[2]); 
                  id_ex_arr[4]=1; 
                }     
                if(tokens[0].compare("addi")==0 || tokens[0].compare("sll")==0) {
                    id_ex[1]=zero(reg_id_ex[1]);
                    stringstream ss;
                    if(reg_id_ex[2][1]=='x')
                      ss << hex << reg_id_ex[2];
                    else
                      ss << reg_id_ex[2];
                    ss >> id_ex[2];
                    id_ex_arr[4]=1; 
                }
                if(tokens[0].compare("beq")==0 || tokens[0].compare("bne")==0) {
                  id_ex[1]=zero(reg_id_ex[1]);
                if(reg_id_ex[0].compare("")!=0 && reg_id_ex[0].compare(ex_mem_str)==0) {
                  if(ex_mem_arr[2]==1)
                    stall="stall";
                  else
                    id_ex[0]=ex_mem;
                }
                else if(reg_id_ex[0].compare("")!=0 && reg_id_ex[0].compare(mem_wb_str)==0) {
                    id_ex[0]=mem_wb;
                }
                if(reg_id_ex[1].compare("")!=0 && reg_id_ex[1].compare(ex_mem_str)==0) {
                  if(ex_mem_arr[2]==1)
                    stall="stall";
                  else
                    id_ex[1]=ex_mem;
                }
                else if(reg_id_ex[1].compare("")!=0 && reg_id_ex[1].compare(mem_wb_str)==0) {
                    id_ex[1]=mem_wb;
                }
                    if((tokens[0].compare("beq")==0 && id_ex[0]==id_ex[1]) || (tokens[0].compare("bne")==0 && id_ex[0]!=id_ex[1])) {
                        pc=labels[reg_id_ex[2]];
                        id_ex_arr[0]=1;
                    }
                    for(int i=0; i<3; i++)
                      reg_id_ex[i]="";
                }
                if(tokens[0].compare("lw")==0 || tokens[0].compare("sw")==0) {
                    id_ex_arr[1]=4;
                    int x=0;
                    id_ex[2]=0;
                    while(reg_id_ex[1][x]!='(') { 
                        id_ex[2]=id_ex[2]*10+((int)reg_id_ex[1][x]-48);
                        id_ex[2]=id_ex[2]/4;
                        x++;    }
                    int ind2=((int)reg_id_ex[1][x+3]-48);
                    id_ex[1]=assign_pointer(reg_id_ex[1][x+2])[ind2];
                    reg_id_ex[1]=reg_id_ex[1].substr(x+1,3);
                    if(tokens[0].compare("lw")==0) {
                      id_ex_arr[2]=1;
                      id_ex_arr[4]=1;
                    }
                    else if(tokens[0].compare("sw")==0) 
                      id_ex_arr[3]=1;
                    reg_id_ex[2]="";
                }
                if(tokens[0].compare("li")==0) {
                  stringstream ss;
                  if(reg_id_ex[1][1]=='x')
                    ss << hex << reg_id_ex[1];
                  else
                    ss << reg_id_ex[1];
                  ss >> id_ex[1];
                  id_ex_arr[2]=5;
                  id_ex_arr[4]=1;
                  reg_id_ex[2]="";
                }
                if(tokens[0].compare("lui")==0) {
                  stringstream ss;
                  if(reg_id_ex[1][1]=='x')
                    ss << hex << reg_id_ex[1];
                  else
                    ss << reg_id_ex[1];
                  ss >> id_ex[1];
                  id_ex[2]=16;
                  id_ex_arr[1]=2;
                  id_ex_arr[4]=1;
                  reg_id_ex[2]="";
                }
                if(tokens[0].compare("la")==0) {
                  if(labels.find(reg_id_ex[1]) == labels.end()) {
                    stringstream ss;
                    if(reg_id_ex[1][1]=='x')
                      ss << hex << reg_id_ex[1];
                    else
                      ss << reg_id_ex[1];
                    ss >> id_ex[1];
                    id_ex_arr[1]=5;
                  } 
                  else {
                    id_ex[1]=labels[reg_id_ex[1]];
                    id_ex_arr[1]=6;
                  }
                  id_ex_arr[4]=1;
                  reg_id_ex[2]="";
                }
                if(reg_id_ex[1].compare("")!=0 && reg_id_ex[1].compare(ex_mem_str)==0) {
                  if(ex_mem_arr[2]==1)
                    stall="stall";
                  else
                    id_ex[1]=ex_mem;
                }
                else if(reg_id_ex[1].compare("")!=0 && reg_id_ex[1].compare(mem_wb_str)==0) {
                    id_ex[1]=mem_wb;
                }
                if(reg_id_ex[2].compare("")!=0 && reg_id_ex[2].compare(ex_mem_str)==0) {
                  if(ex_mem_arr[2]==1)
                    stall="stall";
                  else
                    id_ex[2]=ex_mem;
                }
                else if(reg_id_ex[2].compare("")!=0 && reg_id_ex[2].compare(mem_wb_str)==0) {
                    id_ex[2]=mem_wb;
                }
                if(tokens[0].compare("sub")==0)
                  id_ex_arr[1]=1;
                if(tokens[0].compare("sll")==0)
                  id_ex_arr[1]=2;
                if(tokens[0].compare("slt")==0)
                  id_ex_arr[1]=3;        
          }
        }
      }
      if(stall.compare("stall")==0) {
        stall_count++;
        pc--;
        stall="";
      }
      if(nop[0].compare("none")==0)
        nop[1]="none";
    }

    void fetch() {
      if(pc<instr.size()) {
        if_id=instr[pc];
        ++pc;
      }
      else
        nop[0]="none";
    }
};

// DRIVER FUNCTION

int main(int argc, char const *argv[])  {

    Instructions i;
    vector <string> line;
    int line_num=0;
    new_file.open("text.asm",ios::in);
    string str;

    while(getline(new_file,str)) {
        line.push_back(str);
        stringstream ss(str);
        string w;
        while(getline(ss,w,' ')) {
        if(w=="")
        continue;
        else if(w[w.length()-1]==':') {
        i.labels[w.substr(0,w.length()-1)]=line_num;
        }
        }
        ++line_num;
        }

    int m = i.labels["main"];
    for (auto ad = i.labels.begin(); ad != i.labels.end(); ad++) {
        if(ad->second>=m)
        ad->second=ad->second-m;
    }

    for(int line_num=0; line_num<line.size();line_num++) {
        if(line[line_num]=="")
        continue;
        else if(line[line_num].find(".data")!=string::npos) {
        ++line_num;
        while(1) {
        if(line[line_num].find(".text")!=string::npos) {
        line_num--;
        break;
        }

        stringstream s(line[line_num]);
        string word;
        vector<string> tokens1;

        while(getline(s,word,' ')) {
            if(word=="")
                continue;
            else if(word[word.length()-1]==':') 
                i.labels[word.substr(0,word.length()-1)]=i.index;
            else
                tokens1.push_back(word);
        }

        if(tokens1.size()!=0 && tokens1[0].compare(".word") == 0) {
        for(int j =1; j<tokens1.size();j++){
          i.word(tokens1[j]);
                    } 
            }
        line_num++;
        }
        }

        else if(line[line_num].find(".text")!=string::npos) {
        for(int line_num2=m; line_num2<line.size(); line_num2++) {
        instr.push_back(line[line_num2]);
        ++i.pc;
        }
        }
        else if(line[line_num].find(".globl main")!=string::npos) {
        break; }
    }

    i.pc=0;
    while(clock_cycle>=0) {
      int x;
      if(i.nop[3].compare("none")==0)
        break;
      i.write_back();
      i.mem();
      i.execute();
      i.decode();
      i.fetch();
      clock_cycle++;
    }
    cout << clock_cycle << endl;
    cout << i.stall_count << endl;
    for(int y=0; y<11; y++)
      cout << i.memory[y] << endl;

    return 0;
}
#include<iostream>
#include<fstream>
#include<cstring>
using namespace std;

int Miyaolenth(char* c) 
{
  int klen=1;   //密钥长度
  int clen=strlen(c);   //密文的长度 		     	
  while(1)
  {
        double IC[klen]; //重合指数
        double avgIC=0;  //平均重合指数
        for(int i = 0; i < klen; i++)    //统计分组字母个数 
        {		
            int f[26]={ 0 };   //盛放字母个数的数组
            for(int j = 0; i + j * klen < clen; j++)
                f[(int)(c[i + j * klen]-'A')]++;  
            double e=0.0;

            int L = clen / klen;
            L*=(L-1);
            for(int k=0;k<26;k++)        //分组计算重合指数IC 
                if(f[k]!=0)
                    e=e+((double)f[k]*(double)(f[k]-1))/(double)L;
            IC[i]=e;
    }
	for(int i=0;i<klen;i++)
	    avgIC+=IC[i];

        avgIC/=klen;          //求IC的平均值  
        if (avgIC >= 0.065)  break;    //判断退出条件，重合指数的平均值是否大于0.065
        else  klen++;
  }
  cout << klen << endl;
  return klen;
}

int main(){
    double p[26] = {0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.02, 0.061, 0.07, 0.002, 0.008, 0.04, 0.024, 0.067, 0.075, 0.019, 0.001, 0.06, 0.063, 0.091, 0.028, 0.01, 0.023, 0.001, 0.02, 0.001};
	char miwen[5000];
    ifstream infile;

    //infile.open("miwen.txt", ios::in);
    infile.open("miwen.txt", ios::in);
    if(!infile.is_open()){
        cout << "Open file error!" << endl;
        exit(0);
    }
    int i;
    for(i = 0; !infile.eof(); i++){
        char temp;
        infile >> temp;
        if(temp <= 'Z' && temp >= 'A'){
            miwen[i] = temp;
        } else if(temp <= 'z' && temp >= 'a'){
            miwen[i] = temp - ('a' - 'A');
        }
        
    }
    miwen[i] = '\0';
    infile.close();

    int klen = Miyaolenth(miwen);
    int clen = strlen(miwen);


    int key[klen];  //存放密钥 
	for(int i = 0; i < klen; i++)    //统计分组字母个数 
	{	
      double max = 0.0;
      int g = 0;
	  for(int t = 0; t < 26; t++)
	  {
	 	double x = 0.0;    //拟重合指数 	
	    int f[26]={ 0 };   //盛放字母个数的数组
		for(int j = 0; i + j * klen < clen; j++){
            f[(int)(miwen[i + j * klen]-'A')]++;  
        }
        int L = clen / klen;

        for(int k = 0; k < 26; k++)
            x = x + (double)(p[k] * f[(k + t) % 26])/(L*1.0);
        if(x > max )
        {
        	g = t;
            max = x;
		}
	   }
        key[i] = g;
   }
   
    cout<<"Key:"; 
    for(int i=0;i<klen;i++)    //输出密钥字 
            cout<< (char)(key[i] + 'A') << " ";
	cout << endl;
    char mingwen[clen];

    for(int i = 0; i < clen/klen; i++){
        for(int j = 0; j < klen; j++){
            mingwen[i * klen + j] = (char)((miwen[i * klen + j] - 'A' + key[j]) % 26 + 'A');
            cout << mingwen[i * klen + j];
        }
        
    }

}
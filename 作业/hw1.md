### 信息安全技术第一次作业

姓名：李钰

学号：19335112

#### 作业内容

​	编程实现维吉尼亚密码的破译流程，并破译附件txt文件中的一段维吉尼亚密码加密得到的密文。要求给出完整的实现代码，以及运行结果和分析过程。

#### 实现过程

**1. 从文件中读取密文**

创建一个读文件的数据流，按char来读入文件直至结束。

```c++
//从文件中读取密文，只读取字母，若为小写字母也转成大写字母来读
void get_miwen(char miwen[]){
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
}
```



##### 2. 确定密钥长度：重合指数法

**算法思路**：若密钥的长度为$m$，密文字符串$y=y_1y_2\dots y_n$长度为n。则可以将$y$分割为$m$个长度相等的子串，分别为
$$
y_1=y_1y_{m+1}y_{2m+1}\dots \\
y_2=y_2y_{m+2}y_{2m+2}\dots \\
\dots \\
y_m=y_my_{2m}y_{3m}\dots
$$
每个字符串均为在明文的基础上偏移$k_i$个得到的字母，所以每个子串都是标准的英文文本串。由定理可知，对于每一个字串$y_i$都有以下公式成立。
$$
I_c(x)\approx \sum^{25}_{i=0}p^2_i\approx 0.065
$$
据此，我们可以得到**编码的基本思路**：使用贪心算法，对$m$从1到n取值，统计每一个由$m$生成的子串中各个字幕出现的次数$fi$，利用公式
$$
I_c(x)\approx \frac {\sum^{25}_{i=0}f_i(f_i-1)}{n'(n'-1)}
$$
来计算每一个子串的重合指数，之后对所有重合指数求平均值。将该平均值与0.065作比较，若小于则说明当前的$m$取值不合理，若大于则可大概确定密钥的长度为$m$。

代码实现：

```C++
int keyLength(char* miwen){
    int miwen_len = strlen(miwen);

    for(int m = 1; m < miwen_len; m++){
        double Ic[m]; //m个子串每个的重合因子

        for(int i = 0; i < m; i++){//m个子串都满足

            int sub_len = miwen_len / m; //子串的长度
            int f[26] = {0};//用于统计该子串每个字母出现的次数

            for(int j = 0; j < sub_len; j++){   //遍历该子串
                f[miwen[i + m * j] - 'A']++;
            }

            int total = 0;
            for(int j = 0; j < 26; j++){
                total += f[j] * (f[j] - 1);
            }

            Ic[i] = (double)total / (double)(sub_len * (sub_len - 1)) ;
        }

        double avg_Ic = 0.0;
        for(int i = 0; i < m; i++){
            avg_Ic += Ic[i];
        }
        avg_Ic /= m;

        if(avg_Ic >= 0.06){
            return m;
        }
    }
}
```



##### 3. 确定密钥：重合指数测试法

思路：密钥$K={k_1, k_2,\dots ,k_m}$,对应着每个明文子串字符向右的偏移量。对每一个$k_i$进行从0到25的猜测，假设偏移量为$g$，根据如下公式计算$M_g$，取26个$g$中值最大的那一个当作偏移量。
$$
M_g=\sum^{25}_{i=0}\frac{p_if_{g+i}}{n'}
$$

代码实现：

```C++
void get_key(int klen, char* miwen, char* keys){
    //26个字母出现的概率
    double p[26] = {0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.02, 0.061, 0.07, 0.002, 0.008, 0.04, 0.024, 0.067, 0.075, 0.019, 0.001, 0.06, 0.063, 0.091, 0.028, 0.01, 0.023, 0.001, 0.02, 0.001};
    int sub_len = strlen(miwen) / klen;

    for(int i = 0; i < klen; i++){//对每个子串

        int f[26] = {0};//用于统计该子串每个字母出现的次数
        for(int j = 0; j < sub_len; j++){   //遍历该子串
            f[miwen[i + klen * j] - 'A']++;
        }

        double max = 0;
        
        for(int g = 0; g < 26; g++){//试偏移量
            double Mg = 0;
            for(int j = 0; j < 26; j++){
                Mg += ((double)p[j] * f[(j + g) % 26] / sub_len);
            }

            if(Mg > max){
                max = Mg;
                keys[i] = (char)'A' + g;
            }
        }
        
    }

    return ;
}
```

##### 4. 根据密钥恢复明文

注意移动之后是否会超出'Z'

```C++
void decode(char* mingwen, char* miwen, char* keys){
    int key_len = strlen(keys);

    for(int i = 0; i < strlen(miwen); i++){
        int temp =  miwen[i] + (keys[i % key_len] - 'A');
        int over = temp % ('Z' + 1);
        if(over == temp){
            mingwen[i] = (char)over;
        }else{
            mingwen[i] = (char)over + 'A';
        }

    }
}
```

**5. 主函数**

```C++
int main(){
    //输入密文
    char miwen[5000];
    get_miwen(miwen);

    //获取密钥长度
    int keys_len = keyLength(miwen);

    //进一步获得密钥
    char keys[keys_len];
    get_key(keys_len, miwen, keys);

    //根据密钥解密
    int len = strlen(miwen);
    char mingwen[len];
    decode(mingwen, miwen, keys);
    
    cout << "Keys:" ;
    for(int i = 0 ; i < keys_len; i++){
        cout << keys[i] ;
    }
    cout << endl;
    cout << "Mingwen is:" << endl;

    for(int i = 0; i < len; i++){
        cout << mingwen[i] ;
    }
    cout << endl;
}
```

结果

```C++
Keys:JANET
Mingwen is:
LHEIXGXAUQTPAAGFBLGXJXGIKERSISKSOUFBNQRVUHAVKYHLTXNSLIGWREAMFGWQFBVNXKKQEGKAMCIXJMTVUXSTLVJUGIFYMCZKMDGMKFMFRAQRZXPPFVXWGNWIEIERITKEDLUHGBFTJHMJPBCLQEOAYNALBVOJQYPLYGACIBJIEPTMFNBVWBUAKUPNZKCDPLKJXJYHNKGCKIRUCEJMEZENARRQGOPLKVKTVGAWGCFCUVJSJELIOEJQRFUTYERAXGTAXSXHJDEIFIVTOLANQRGOWYERMGACLLDSAAFQNMBHGNEQZHXIVJDPNE
```


#include<iostream>
#include<math.h>
#include<fstream>
#include <string.h>
using namespace std;

/*
姓名：梁文诗
学号：2022150198
*/

//IP置换矩阵
int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7 };
// 逆IP置换矩阵
int IPR[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25 };

// 扩展矩阵
int EXTEND[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1 };

//密钥第一次置换矩阵
int PC_1[56] = {
    57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };

//密钥第二次置换矩阵
int PC_2[48] = {
    14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 };

//密钥偏移表
const int OFFSET[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

// 8个S盒
int SBOX[8][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};

// P盒
int PBOX[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25 };

//将S盒中的数据转化为2进制字符串
char SBOX2Bits[16][5] = {
    "0000",
    "0001",
    "0010",
    "0011",
    "0100",
    "0101",
    "0110",
    "0111",
    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101",
    "1110",
    "1111" };


//bytes转为bits字符串
void ByteTobits(const char* in, int len, char* out)
{
  
    for (int i = 0; i < len * 8; i++)
    {
        if (in[i / 8] & (0x01 << (7 - i % 8)))
        {
            out[i] = '1';
        }
        else
        {
            out[i] = '0';
        }
    }
}
// bits字符串转为bytes
void bitsToByte(char* in, char* out) {
    int index = 0;
    int sum = 0;
    for (int i = 0; i < strlen(in); i++)
    {
        if (in[i] == '1')
        {
            sum += pow(2, 7 - i % 8);
        }
        if ((i + 1) % 8 == 0)
        {
            out[index] = sum;
            index++;
            sum = 0;
        }
    }
}
//bit字符串转为十进制数
int bitsTod(char* str) {
    int num = 0;
    int j = 0;
    for (int i = strlen(str) - 1; i >= 0; i--)
    {
        if (str[i] == '1')
        {
            num += pow(2, j);
        }
        j++;
    }
    return num;
}

//密钥第一次置换
/*
姓名：梁文诗
学号：2022150198
*/
void  keyReplaceByPC_1(char* in, char* out)
{
    int x = 0;
    for (int i = 0; i < 56; i++)
    {
        x = PC_1[i];
        out[i] = in[x - 1];
    }
}
//密钥第二次置换
/*
姓名：梁文诗
学号：2022150198
*/
void keyReplaceByPC_2(char in[][57], char out[][49])
{
    int x = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 48; j++)
        {
            x = PC_2[j];
            out[i][j] = in[i][x - 1];
        }
    }
}
//得到16组子密钥
void get16Key(char* key, char* key56, char leftKey[][29], char rightKey[][29],char childKey56[][57],char childKey48[][49])
{

    //第一次置换
    keyReplaceByPC_1(key, key56);
    /*
    姓名：梁文诗
    学号：2022150198
    */
    memcpy(leftKey[0], key56, 28);
    memcpy(rightKey[0], key56 + 28, 28);
    int offset = 0;
    for (int i = 0; i < 16; i++)
    {
        offset = OFFSET[i];
        memcpy(leftKey[i + 1], leftKey[i] + offset, 28 - offset);
        memcpy(leftKey[i + 1] + 28 - offset, leftKey[i], offset);
        memcpy(rightKey[i + 1], rightKey[i] + offset, 28 - offset);
        memcpy(rightKey[i + 1] + 28 - offset, rightKey[i], offset);
    }

    //拼接成56bit密钥
     /*
    姓名：梁文诗
    学号：2022150198
    */
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            childKey56[i][j] = leftKey[i + 1][j];
        }
        for (int j = 28; j < 56; j++)
        {
            childKey56[i][j] = leftKey[i + 1][j - 28];
        }
    }

    //第二次置换
    keyReplaceByPC_2(childKey56, childKey48);

}


//明文置换
/*
姓名：梁文诗
学号：2022150198
*/
void replaceByIP(char* in, char* out)
{
    int x = 0;
    for (int i = 0; i < 64; i++)
    {
        x = IP[i];
        out[i] = in[x - 1];
    }
}
//明文逆置换
void replaceByIPR(char* in, char* out)
{
    int x = 0;
    for (int i = 0; i < 64; i++)
    {
        x = IPR[i];
        out[i] = in[x - 1];
    }
}

//轮函数
char* function(char* text, char* key)
{
    char result[33] = { 0 };
    //拓展操作
    char text48[48] = { 0 };
    int x = 0;
    /*
    姓名：梁文诗
    学号：2022150198
    */
    for (int i = 0; i < 48; i++)
    {
        x = EXTEND[i];
        text48[i] = text[x - 1];
    }

    //与子密钥异或
    for (int i = 0; i < 48; i++)
    {
        if (text48[i] == key[i]) text48[i] = '0';
        else text48[i] = '1';
    }
    /*
    姓名：梁文诗
    学号：2022150198
    */
    //压缩运算
    char text32[32] = { 0 };
    for (int i = 0; i < 8; i++)
    {
        char xbits[3] = { 0 };
        char ybits[5] = { 0 };
        int  y = 0;
        xbits[0] = text48[i * 6];
        xbits[1] = text48[i * 6 + 5];
        for (int j = 0; j < 4; j++)
        {
            ybits[j] = text48[i * 6 + j + 1];
        }
        
        x = bitsTod(xbits); //2进制转换为10进制行数
        y = bitsTod(ybits); //2进制转换为10进制列数
        
        for (int j = 0; j < 4; j++)
        {
            text32[i * 4 + j] = SBOX2Bits[SBOX[i][x][y]][j];//S盒取数转换为2进制
        }
    }
    /*
    姓名：梁文诗
    学号：2022150198
    */
    //置换运算P
    for (int i = 0; i < 32; i++)
    {
        x = PBOX[i];
        result[i] = text32[x - 1];
    }
    return result;
}


void DES(const char* key, char* in, char* out, int mode)
{
    char keybits[65] = { 0 };
    char key56bits[57] = { 0 };
    char inbits[65] = { 0 };
    char leftKey[17][29] = { {0} };
    char rightKey[17][29] = { {0} };
    char childKey56[17][57] = { {0} };
    char childKey48[17][49] = { {0} };
    char inIP[65] = { 0 };
    char Left[17][33] = { {0} };
    char Right[17][33] = { {0} };


    //生成密钥
    ByteTobits(key, 8, keybits);
    ByteTobits(in, 8, inbits);
    
    //16组子密钥产生函数
    get16Key(keybits, key56bits, leftKey, rightKey, childKey56, childKey48);
 
    //明文IP置换
    replaceByIP(inbits, inIP);
    /*
    姓名：梁文诗
    学号：2022150198
    */
    for (int i = 0; i < 32; i++)
    {
        Left[0][i] = inIP[i];
        Right[0][i] = inIP[i + 32];
    }
    //调用16次轮函数
    for (int i = 1; i <= 16; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            Left[i][j] = Right[i - 1][j];
        }
        char* result;
        //区分加解密
        if (mode == 1)
        {
            //调用轮函数
            result = function(Right[i - 1], childKey48[i - 1]);
        }
        else
        {
            //调用轮函数
            result = function(Right[i - 1], childKey48[16 - i]);
        }
        //异或操作
        for (int j = 0; j < 32; j++)
        {
            if (Left[i - 1][j] == result[j])
            {
                Right[i][j] = '0';
            }
            else
            {
                Right[i][j] = '1';
            }
        }
    
    }
    /*
   姓名：梁文诗
   学号：2022150198
   */
    char result1[65] = { 0 };
    //左右互换
    for (int i = 0; i < 32; i++)
    {
        result1[i] = Right[16][i];
    }
    for (int i = 32, j = 0; i < 64; i++, j++)
    {
        result1[i] = Left[16][j];
    }
    char result2[65] = { 0 };
 
    //逆置换
    replaceByIPR(result1, result2);
    //字节输出
    bitsToByte(result2, out);

}

void my3DES(char* key, char* in, char* out, int mode)
{
    char key1[9] = { 0 };
    char key2[9] = { 0 };
    char key3[9] = { 0 };
    /*
    姓名：梁文诗
    学号：2022150198
    */
    //输入16byte的密钥分为三个密钥
    for (int i = 0; i < 8; i++)
    {
        key1[i] = key[i];
        key2[i] = key[i + 8];
        key3[i] = key[i];
    }
    char bittext[65] = { 0 };
    char bytetext[9] = { 0 };
    char out64[65] = { 0 };
    char out8[9] = { 0 };
    //3DES加密 加密 解密 加密
    if (mode == 1)
    {
        /*
        姓名：梁文诗
        学号：2022150198
        */
        DES(key1, in, out, 1);
        for (int i = 0; i < 8; i++)
        {
            bytetext[i] = out[i];
        }
        DES(key2, bytetext, out, 2);
        for (int i = 0; i < 8; i++)
        {
            bytetext[i] = out[i];
        }
        DES(key3, bytetext, out, 1);
    }
    //3DES解密 解密 加密 解密
    else
    {
        /*
        姓名：梁文诗
        学号：2022150198
        */
        DES(key1, in, out, 2);
        for (int i = 0; i < 8; i++)
        {
            bytetext[i] = out[i];
        }
        DES(key2, bytetext, out, 1);
        for (int i = 0; i < 8; i++)
        {
            bytetext[i] = out[i];
        }
        DES(key3, bytetext, out, 2);
    }
}



int main()
{
    string fname, enc_fname, dec_fname, key;
    cout << "请输入要加密的文件名：";
    cin >> fname;
    char fnamebuf[20];
    strcpy(fnamebuf, fname.c_str());
    FILE* filein, * fileout, * filein2, * fileout2;
    //检查文件存不存在
    if ((filein = fopen(fnamebuf, "rb")) == NULL)
    {
        cout << "文件不存在" << endl;
        system("pause");
        return 0;
    }

    int blockNum = 0;//记录有多少个分组
    cout << "请输入加密后存储的文件名：";
    cin >> enc_fname;
    cout << "准备加密，请输入密钥：";
    cin >> key;
    char enfnamebuf[20];
    strcpy(enfnamebuf, enc_fname.c_str());
    char keybuf[20];
    strcpy(keybuf, key.c_str());
    fileout = fopen(enfnamebuf, "wb");
    int count = 0;
    int lastCount = 0;  //记录最后一个分组多少个字节
    char data[9] = { 0 };
    char result[65] = { 0 };
    // 加密过程
    /*
    姓名：梁文诗
    学号：2022150198
    */
    while (1)
    {
        memset(data, 0, 8);
        if ((count = fread(data, 1, 8, filein)) == 0)   //记录读取到的有效字节数
        {
            break;
        }
        my3DES(keybuf, data, result, 1);
        
        fwrite(result, 8, 1, fileout);
        blockNum++;     //记录有多少个分组
        lastCount = count;//记录最后一个分组多少个字节
    }
    fclose(filein);
    fclose(fileout);

    cout << "加密完成" << endl;
    cout << "请输入解密后存储的文件名：";
    cin >> dec_fname;
    char defnamebuf[20];
    strcpy(defnamebuf, dec_fname.c_str());

    filein2 = fopen(enfnamebuf, "rb");
    fileout2 = fopen(defnamebuf, "wb");

    cout << "准备解密，请输入密钥：";
    cin >> key;
    char keybuf2[20];
    strcpy(keybuf2, key.c_str());
    char data2[65] = { 0 };
    char result2[9] = { 0 };
    int x = 0;
    //解密过程
    /*
    姓名：梁文诗
    学号：2022150198
    */
    while (1)
    {
        x++;
        memset(data2, 0, 64);
        if ((count = fread(data2, 1, 8, filein2)) == 0)
        {
            break;
        }
        my3DES(keybuf2, data2, result2, 2);
        //去除自动补充的部分
        if (x == blockNum)
        {
            fwrite(result2, lastCount, 1, fileout2);
        }
        else
        {
            fwrite(result2, 8, 1, fileout2);
        }
    }
    fclose(filein2);
    fclose(fileout2);
    cout << "解密完成" << endl;
    system("pause");
}
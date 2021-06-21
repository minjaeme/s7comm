/*
@todo(21 06 18)
- 

*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <string> 
#include <cstring>
#include <random>
using namespace std;

// util
int makeRandom(int s); // 자리수
vector<string> split(string s, string divid);
void printPDU(vector<int> v);
void printPDUforTest(vector<int> v);

// make parameter
int makeFunction (string function);
int makeSize (string size);
int makeLength (string length);
int makeLength (string length, string base);
int makeDBNumber (string db);
int makeArea (string area);
int makeAddress (string addr);
int makeAddress (string addr, string base);

int main() {
    int itemCount = 0;
    string function;
    vector<string> inp;
    int tmpValue = 0;

    vector<int> pdu;
    
    // step0. inp 파일 넣기
    cin >> itemCount >> function;
    // step0. getline 입력 받기
    cin.ignore();
    for (int i=0; i<itemCount; i++) {
        string assignPLC;
        getline(cin, assignPLC);
        inp.push_back(assignPLC);
    }

    // info
    cout << "---inp data info---\n";
    for (int i=0; i<itemCount; i++) {
        cout << inp[i] << '\n';
    }
    cout << "---end---\n";
    // test end

    // step0-1 PDU 제외한 헤더 구현 TPKT.
    // version, reserved
    pdu.push_back(0x03);
    pdu.push_back(0x00);
    /* size */
    tmpValue = 0xff;
    pdu.push_back(tmpValue / 0x100);
    pdu.push_back(tmpValue % 0x100);

    // step0-2. COTP
    pdu.push_back(0x02);
    pdu.push_back(0xf0);
    pdu.push_back(0x80);


    // step1. initPDU Id, POSCTR, reserved
    
    pdu.push_back(0x32);
    pdu.push_back(0x01);
    pdu.push_back(0x00);
    pdu.push_back(0x00);

    // step1-2. reference, random
    // pdu.push_back(makeRandom(2));
    // pdu.push_back(makeRandom(2));
    pdu.push_back(0xff);
    pdu.push_back(0xff);

    // step2. add parameter length
    tmpValue = 2 + itemCount * 12;
    pdu.push_back(tmpValue / 0x100);
    pdu.push_back(tmpValue % 0x100);

    // step2-2. data length; 없으니 0
    pdu.push_back(0);
    pdu.push_back(0);

    // step3. parameter(1) offset0: function
    pdu.push_back(makeFunction("r"));
    
    // step3-2. parameter(2) offset1: count
    pdu.push_back(itemCount);

    // step4. make parameter
    for (int i=0; i<itemCount; i++) {
        /*
        DB1-W:R 3 1
        M-B:R 2 1
        */
        // cur[1] offset (func[0] 기준, Byte로 바꿔야 한다.)
        // cur[2] len (func[0] 기준, Byte로 바꿔야 한다.)
        vector<string> cur = split(inp[i], " ");
        // comd[0] = DB1 or M
        // comd[1] = W:R or B:R
        vector<string> comd = split(cur[0], "-");
        // func[0] = W or B : size
        // func[1] = W or R 
        vector<string> func = split(comd[1], ":");
        
        cout << comd[0] << " " << func[0] << " " << func[1] << " " << cur[1] << " " << cur[2] << endl;
        cout << comd[0] << " " << "B" << " " << func[1]  << " " << makeAddress(cur[1], func[0]) << " " << makeAddress(cur[2], func[0]) << endl;

        /*
        DB1-W:R 3 1 offset3 len1
        M-B:R 2 1

        DB1-W:R 3 1
        DB1.DB W 6, 2 // word 시작 add6, len4
        */
        
        int index = 0;
        // step 4-2. spec && length
        pdu.push_back(0x12);
        pdu.push_back(0x0a);

        // step 4-3. syntax
        pdu.push_back(0x10);
        
        // step 4-4. size
        pdu.push_back(makeSize(func[0])); // ok

        // step 4-5. len
        tmpValue = makeAddress(cur[1], func[0]); 
        pdu.push_back(tmpValue / 0x100);
        pdu.push_back(tmpValue % 0x100);

        // step 4-6. db ok
        if (comd[0].find("DB") != std::string::npos) {
            // 만약 db다
            tmpValue = makeDBNumber(comd[0]);
            pdu.push_back(tmpValue / 0x100);
            pdu.push_back(tmpValue % 0x100);
        } else {
            // 아니라면
            pdu.push_back(0x00);
            pdu.push_back(0x00);
        }

        // step 4-7. area
        pdu.push_back(makeArea(func[0]));
        
        // step 4-8. addr
        tmpValue = makeAddress(cur[2], func[0]);
        pdu.push_back(tmpValue >> 0x10);
        pdu.push_back((tmpValue >> 0x8) % 0x100);
        pdu.push_back(tmpValue % 0x100);
    }

    // 마지막에 step 0-1 사이즈를 update
    tmpValue = pdu.size();
    pdu[2] = tmpValue / 0x100;
    pdu[3] = tmpValue % 0x100;
    
    printPDUforTest(pdu);
    return 0;
}

int makeFunction (string function) {
    int res = 0;
    // 현재는 read만 = 04
    if (!function.compare("r")) {
        res = 4;
    }
    return res;
}

int makeSize (string size) {
    /*
    0x01 - BIT
    0x02 - BYTE
    0x03 - CHAR
    0x04 - WORD
    0x05 - INT
    0x06 - DWORD    
    */
    int res = 0;
    if (!size.compare("X")){
        res = 0x01;
    } else if (!size.compare("B")){
        res = 0x02;
    } else if (!size.compare("W")){
        res = 0x04;
    } else if (!size.compare("D")){
        res = 0x06;
    }

    return res;
}

int makeLength (string length) {
    int res = 0;
    if (!length.compare("X")){
        res = 0x1;
    } else if (!length.compare("B")){
        res = 0x1;
    } else if (!length.compare("W")){
        res = 0x2;
    } else if (!length.compare("D")){
        res = 0x4;
    }
    return res;
}

int makeLength (string length, string base) {
    int res = 0;
    /* 
    모두 Byte기준으로 바꾼 뒤에 정리
     */
    int initLength = 0; // Byte기준 Length
    if (!base.compare("X")) {
        // BIT
        res = makeLength(length) * 8;
    } else if (!base.compare("B")) {
        // Byte
        // 바꿀필요없음.
        res = makeLength(length);
    } else if (!base.compare("W")) {
        // Word
        res = makeLength(length) / 2;
    } else if (!base.compare("D")) {
        // Double Word
        res = makeLength(length) / 4;
    }
    return res;
}


int makeDBNumber (string db) {
    int res = 0;

    string dbNumber = "";
    for (int i=2; i<db.size(); i++) {
        // DB1 
        // DB100
        if (48 > db[i] || db[i] > 57) {
            break;
        }
        dbNumber += db[i];
    }
    res = stoi(dbNumber);
    return res;
}

int makeArea (string area) {
    int res = 0;
    if (!area.compare("P")){
        res = 0x80;
    } else if (!area.compare("I")){
        res = 0x81;
    } else if (!area.compare("Q")){
        res = 0x82;
    } else if (!area.compare("M")){
        res = 0x83;
    } else if (area.find("DB") != std::string::npos){
        res = 0x84;
    } 
    return res;
}


int makeAddress (string addr) {
    int res = 0;
    if (addr.find(".") == std::string::npos) {
        // bit가 없다면
        res = 8 * stoi(addr);
    } else {
        // 있다면
        vector<string> cur = split(addr, ".");
        res = 8 * stoi(cur[0]) + stoi(cur[1]);
    }
    return res;
}

int makeAddress (string addr, string base) {
    int res = 0;
    /* 
    모두 Byte기준으로 바꾼 뒤에 정리
     */
    int initLength = 0; // Byte기준 Length
    if (!base.compare("X")) {
        // BIT
        res = stoi(addr);
    } else if (!base.compare("B")) {
        // Byte
        // 바꿀필요없음.
        res = stoi(addr) << 3;
    } else if (!base.compare("W")) {
        // Word
        res = stoi(addr) * 2 << 3;
    } else if (!base.compare("D")) {
        // Double Word
        res = stoi(addr) * 4 << 3;
    }
    return res;
}



vector<string> split(string s, string divid) {
	vector<string> v;
	char* c = strtok((char*)s.c_str(), divid.c_str());
	while (c) {
		v.push_back(c);
		c = strtok(NULL, divid.c_str());
	}
	return v;
}

int makeRandom(int s) {
    // 시드값을 얻기 위한 random_device 생성.
    std::random_device rd;

    // random_device 를 통해 난수 생성 엔진을 초기화 한다.
    std::mt19937 gen(rd());

    // 0 부터 10^s-1 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
    std::uniform_int_distribution<int> dis(0, pow(10, s)-1);
    int res = dis(gen);

    return res;
}

void printPDU(vector<int> v) {
    for (int i : v) { 
        cout << setfill('0') << setw(2) << hex << i << " ";
    }
}


void printPDUforTest(vector<int> v) {
    for (int i = 19; i < v.size(); i++) {
        cout << setfill('0') << setw(2) << hex << v[i] << " ";
        if (i%12 == 6) {
            cout << '\n';
        }
    }
    
}
*beta version*   
**테스트** 진행중입니다

## 명령어
```
for linux
g++ make-pdu-arr.cpp && cat inp | ./a.out

for windows(cmd)
g++ make-pdu-arr.cpp && type inp | ".\a.exe"
```

## input 파일 형식
```
6 r
I X 4.0
M B 2
M D 3
DB1.DB X 0.0
DB7.DB W 6
DB10.DB D 6
```
[예시]  
개수 함수(r or w, 현재는 r만 지원)  
명령어1  
명령어2  
명령어3  
...  
명령어6  


메모리식별자 길이표시문자 바이트.비트

I: 입력 메모리  
Q: 출력 메모리  
M: 비트 메모리  
DB: 데이터 블록  

X: bit  
B: byte  
W: word  
D: Dword  

## output 예제
```
---inp data info---
I X 4.0
M B 2
M D 3
DB1.DB X 0.0
DB7.DB W 6
DB10.DB D 6
---end---
32 01 00 00 ff ff 00 4a 00 00 04 06 12 0a 10 01 00 01 00 00 81 00 00 20 12 0a 10 02 00 01 00 00 83 00 00 10 12 0a 10 06 00 04 00 00 83 00 00 18 12 0a 10 01 00 01 00 01 84 00 00 00 12 0a 10 04 00 02 00 07 84 00 00 30 12 0a 10 06 00 04 00 0a 84 00 00 30 
```

# s7 pdu 구성
오프셋이 

```
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|                             MW100                             |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|              MB100            |              MB101            |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+


한칸이 2바이트
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|                             MD100                             |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|              MW100            |              MW102            |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|     MB100     |      MB101    |     MB102     |     MB103     |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 6 | 4 | 2 | 0 | 6 | 4 | 2 | 0 | 6 | 4 | 2 | 0 | 6 | 4 | 2 | 0 |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

MD100은 MB100~MB103
MD101은 MB101~MB104
```

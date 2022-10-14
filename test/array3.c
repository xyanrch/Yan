int test(int(*a)[5])
 {
    int i;
    int j;
    for(i =0;i<2;i++)
    for(j =0;j<5;j++)
        print(a[i][j]);
//print(a[9][8]);

return 99;
}
int main()
{
    int a[2][5];
        int i;
    int j;
    for(i =0;i<2;i++)
    for(j =0;j<5;j++)
        a[i][j] = i*10+j;
   print(test(a));

    return 0;
}
/*
Expected output:
0
1
2
3
4
10
11
12
13
14
99
*/
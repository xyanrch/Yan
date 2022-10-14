int main()
{
    int a[10];
    int i;
    for (i = 0; i < 10; i++)
    {
        a[i] = i + 1;
        if (i > 5)
            print(a[i - 1]);
        print(a[i]);
    }
    int aa;
    aa = 9;
    print(a[0]);

    return 0;
}
/*
Expected output:
1
2
3
4
5
6
6
7
7
8
8
9
9
10
1
*/
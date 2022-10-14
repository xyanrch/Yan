static int g_static;
int g_var;
int add(int v)
{
    static int c;
    c = c+2;
    return c;
}
int main()
{

    for(int i=0;i<5;i++)
    {
        print(add(2));
    }
    return 0;
}
/*
Expected output:
2
4
6
8
10
*/
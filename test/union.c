int main()
{
    union test{
        int x;
        int y;
    };
    union test p;
    p.x = 8;
    print(p.x);
    print(p.y);
    return 0;
}
/*
Expected output:
8
8
*/
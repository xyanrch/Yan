int dump(int x)
{
    print(x);
    int p;
    p = x+12;
    print(p);
    return 1;
}
int dump2(int x, int y,int z)
{
    int xx;
    int yy;
    xx =x+x;
    yy = y+y;
    print(x);
    print(y);
    print(xx+yy);
    print(z);
    return  0;
}


int main(void)
{
    int x ;
    x =12;
    int y;
    y =66;
    dump(x);
    dump2(33,44,88);
    return 0;
}
/*
Expected output:
12
24
33
44
154
88
*/
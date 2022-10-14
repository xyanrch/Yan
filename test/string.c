int test(char* s)
{
    printstr(s);
    return 0;

}
int main()
{
    char *s;
    s = "hello world\n";

    {
        char * p;
        p = "again\n";
        test(p);
    }
    printstr(s);
    return 0;
}
/*
Expected output:
again
hello world
*/
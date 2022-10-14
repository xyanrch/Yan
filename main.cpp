#include<cstring>

#include<stdio.h>
#include "src/lexer.h"
#include "src/AST.h"
#include "src/gen_x86.h"
#include "src/parser.h"

//namespace Yan{

static void help()
{
    std::string help="Usgae: Yan [option] file ...\n"
                     "  -h       Display help info\n"
                     "  -S       Compile only,Do not assemble and link\n"
                     "  -o       Complie and link,Generate excutable file\n";
    std::cout<<help;
}
static void compile(std::vector<std::string>&files)
{
    Yan::lexer pp(files.front());

    std::string out_asmeble_file = "a.s";
    
    Yan::parser pars(pp);
    auto pp1= pars.parseProgram();
    Yan::gen gencode(pars.getScope(),out_asmeble_file);
    pp1->accept(&gencode);
    
    // if(Yan::log_level  == Yan::L_DEBUG)
    // {
    //     std::ofstream f(files.front()+".symbol");

    //     pars.getScope()->dumpSymbol(f);
    //     f.close();
    // }
   // gencode.genProgram(pp1);
        
}
static void compile_link(std::vector<std::string>&files)
{
    compile(files);
    const char* cmd = "cc  a.s";
    if(system(cmd)!=0)
    {
        Yan::ExitWithError("Link error");
    }
}
static void parseArgs(int argc, char* argv[])
{
    if(argc<2)
    {
        help();
    }
    bool S_arg = false;
    bool h_arg =false;
    bool o_arg = false;
    bool error = false;
    std::vector<std::string> inputFiles;
    for(auto i=1; i<argc;i++)
    {
        if(strcmp(argv[i],"-S")==0)
        {
            S_arg =true;   
        }
        else if (strcmp(argv[i],"-h")==0)
        {
            h_arg =true;
        }
        else if(strcmp(argv[i],"-o")==0)
        {
            o_arg = true;
        }
        else if(argv[i][0]!='-')
        {
            inputFiles.emplace_back(argv[i]);
        }
        else
        {
            error =true;
        }        
        
    }

    if(error || h_arg || inputFiles.empty())
    {
        help();
        return;
    }
    
    if(S_arg)
    {
        compile(inputFiles);
    }
    else if(o_arg||!inputFiles.empty())
    {
        compile_link(inputFiles);
    }
    
}
int main(int argc,char*argv[])
{
    parseArgs(argc,argv);
 

    return 0;
}


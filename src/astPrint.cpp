#include "astPrinter.h"
namespace Yan
{
   astPrinter::astPrinter(std::ostream& out):out_(out)
{
}

astPrinter::~astPrinter()
{
}
void astPrinter::visit(Declaration* node)
{

}
void astPrinter::visit(Enumerator * node)
{
   
}

}
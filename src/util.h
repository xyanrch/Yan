#ifndef _UTIL_H_
#define  _UTIL_H_
namespace Yan
{
// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
extern int align_to(int n, int align) ;
};
#endif
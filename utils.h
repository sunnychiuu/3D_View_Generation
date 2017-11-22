#ifndef __UTILS__
#define __UTILS__

#include <string>
using namespace std;

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif
 
string removeExtension(string const& filename) {
    string::const_reverse_iterator pivot
            = find(filename.rbegin(), filename.rend(), '.');
    return pivot == filename.rend()
            ? filename
            : string(filename.begin(), pivot.base() - 1);
}

string basename(string const& path) {
    return string(find_if(path.rbegin(), path.rend(), 
                [](char c) {return c == '/';}).base(), 
            path.end());
}


#endif

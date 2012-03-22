#include <iostream>
#include <iomanip>
#include "stat.h"

using namespace std;

int main() {
    double var_x, var_y;
    size_t freedom_x, freedom_y;
    cin >> var_x >> var_y >> freedom_x >> freedom_y;
    cout << "p_obs = " << common_variance(var_x, var_y, freedom_x, freedom_y) << endl;
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:

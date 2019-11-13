// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>

int main() {
    constexpr float pi = 3.14159265359;

    std::stringstream ss;

    // Sanity check default print.
    ss << pi;
    assert(ss.str() == "3.14159");
    ss.str("");

    // Change precision format to scientific,
    // and restore default afterwards.
    std::ios ss_state(nullptr);
    ss_state.copyfmt(ss);
    ss << std::setprecision(2);
    ss << std::scientific;
    ss << pi;
    assert(ss.str() == "3.14e+00");
    ss.str("");
    ss.copyfmt(ss_state);

    // Check that cout state was restored.
    ss << pi;
    assert(ss.str() == "3.14159");
    ss.str("");
}

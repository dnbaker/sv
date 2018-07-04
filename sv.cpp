#include "sv.h"
using namespace sprs;

int main() {
    SparseVector<float, u32> sv(1 << 16, 10);
    for(u32 i(0); i < 10; ++i) {
        sv.emplace_back(10 - i, float(i * i + i));
    }
    for(const auto &el: sv) {
        std::fprintf(stderr, "ind %u has %f\n", el.first.get(), el.second.get());
    }
    sv.sort();
    for(const auto &el: sv) {
        std::fprintf(stderr, "ind %u has %f\n", el.first.get(), el.second.get());
    }
}

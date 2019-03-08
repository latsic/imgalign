#ifndef IMGALIGN_EDGEBLACKLISTH
#define IMGALIGN_EDGEBLACKLISTH

#include "CommonTypes.h"
#include "../precomp.hpp"

namespace imgalign {

class EdgeBlackList {

  public:

    void addIndex(int imageIndex1, int imageIndex2);
    bool inside(int imageIndex1, int imageIndex2);
    void clear();

  private:
    std::vector<std::pair<int, int>> blackList;
};

}

#endif
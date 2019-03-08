#ifndef IMGALIGN_IMAGEINDEXBLACKLISTH
#define IMGALIGN_IMAGEINDEXBLACKLISTH

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "MultiStitcher.h"
#include "../precomp.hpp"

namespace imgalign {

class ImageIndexBlackList {

  public:

    void addIndex(int imageIndex);
    bool inside(int imageIndex);
    void clear();

  private:

    std::vector<int> blackList;
};

}

#endif
#include <bits/stdc++.h>
#include "ns3/core-module.h"
#include "ns3/ai-module.h"
#include "ns3/network-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"

namespace ns3 {

typedef struct {
    int posX;
    int posY;
    int decimalX;
    int decimalY;
} MCSFeature;

typedef struct {
    int64_t mcsPredicted;
} MCSPredicted;


}

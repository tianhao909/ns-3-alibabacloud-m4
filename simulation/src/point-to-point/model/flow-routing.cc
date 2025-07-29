#include "flow-routing.h"
#include <iostream>

// Global variable definitions
std::unordered_map<FlowKey, uint32_t, FlowKeyHash> global_flow_to_path_map;
bool global_use_custom_routing = false;

// Direct access functions for frontend-backend communication
extern "C" {
    void SetGlobalFlowMapDirect(const FlowKey* keys, const uint32_t* values, size_t size) {
        global_flow_to_path_map.clear();
        for (size_t i = 0; i < size; i++) {
            global_flow_to_path_map[keys[i]] = values[i];
        }
    }

    void SetGlobalCustomRoutingDirect(bool enable) {
        global_use_custom_routing = enable;
    }


}
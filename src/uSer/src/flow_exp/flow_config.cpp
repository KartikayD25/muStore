#pragma once
#include <vector>
#include <string>

struct FlowConfig {
    enum class Mode {
        ONE_TO_ONE,      // Each client thread maps to specific server thread
        MANY_TO_ONE,     // Multiple client threads to single server thread
        ONE_TO_MANY      // Single client thread to multiple server threads
    };

    Mode mode;
    std::vector<int> client_cores;
    std::vector<int> server_cores;
    size_t request_count;
    size_t payload_size;
};
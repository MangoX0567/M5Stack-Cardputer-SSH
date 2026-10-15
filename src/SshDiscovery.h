#pragma once

#include <stddef.h>
#include <stdint.h>

#include "AppConfig.h"
#include "OperationCallbacks.h"

namespace app {

constexpr size_t kMaxSshDiscoveryResults = 8;

struct SshServiceInfo {
    char name[kHostDisplayNameCapacity]{};
    char target[kSshHostCapacity]{};
    uint16_t port = kDefaultSshPort;
};

class SshDiscoveryList {
public:
    void reset();
    bool add(const char* name, const char* target, uint16_t port);
    const SshServiceInfo* results() const { return results_; }
    size_t count() const { return count_; }

private:
    SshServiceInfo results_[kMaxSshDiscoveryResults]{};
    size_t count_ = 0;
};

class SshDiscovery {
public:
    int discover(SshServiceInfo* results, size_t capacity, const OperationCallbacks& callbacks);

private:
    bool ensureMdns();
#ifdef ARDUINO
    bool mdnsStarted_ = false;
#endif
};

}  // namespace app

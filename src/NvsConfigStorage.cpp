#include "NvsConfigStorage.h"

#include <Preferences.h>
#include <nvs.h>

namespace app {

namespace {

constexpr char kNamespace[] = "pi-console";

// A namespace that has never been written can't be opened read-only. On a
// fresh device that just means nothing is saved yet, not a storage failure.
bool namespaceMissing() {
    nvs_handle_t handle = 0;
    const esp_err_t result = nvs_open(kNamespace, NVS_READONLY, &handle);
    if (result == ESP_OK) nvs_close(handle);
    return result == ESP_ERR_NVS_NOT_FOUND;
}

}  // namespace

BlobStatus NvsConfigStorage::length(const char* key, size_t& result) {
    result = 0;
    if (key == nullptr || key[0] == '\0') {
        return BlobStatus::Error;
    }

    Preferences preferences;
    if (!preferences.begin(kNamespace, true)) {
        return namespaceMissing() ? BlobStatus::Missing : BlobStatus::Error;
    }
    if (!preferences.isKey(key)) {
        preferences.end();
        return BlobStatus::Missing;
    }
    result = preferences.getBytesLength(key);
    preferences.end();
    return result == 0 ? BlobStatus::Error : BlobStatus::Ok;
}

bool NvsConfigStorage::read(const char* key, void* destination, size_t lengthValue) {
    if (key == nullptr || key[0] == '\0' || destination == nullptr || lengthValue == 0) {
        return false;
    }
    Preferences preferences;
    if (!preferences.begin(kNamespace, true)) {
        return false;
    }
    const size_t readLength = preferences.getBytes(key, destination, lengthValue);
    preferences.end();
    return readLength == lengthValue;
}

bool NvsConfigStorage::write(const char* key, const void* source, size_t lengthValue) {
    if (key == nullptr || key[0] == '\0' || source == nullptr || lengthValue == 0) {
        return false;
    }
    Preferences preferences;
    if (!preferences.begin(kNamespace, false)) {
        return false;
    }
    const size_t written = preferences.putBytes(key, source, lengthValue);
    preferences.end();
    return written == lengthValue;
}

bool NvsConfigStorage::remove(const char* key) {
    if (key == nullptr || key[0] == '\0') {
        return false;
    }
    Preferences preferences;
    if (!preferences.begin(kNamespace, false)) {
        return false;
    }
    if (!preferences.isKey(key)) {
        preferences.end();
        return true;
    }
    const bool removed = preferences.remove(key);
    preferences.end();
    return removed;
}

}  // namespace app

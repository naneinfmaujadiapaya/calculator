#include "history.h"

void History::add(const std::string& expression, double result) {
    items.push_back({expression, result});
    if (items.size() > kMaxEntries) {
        items.erase(items.begin());
    }
}

void History::clear() {
    items.clear();
}

const std::vector<HistoryEntry>& History::entries() const {
    return items;
}

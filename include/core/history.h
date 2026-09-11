#pragma once
#include <string>
#include <vector>

struct HistoryEntry {
    std::string expression;
    double result;
};

class History {
public:
    void add(const std::string& expression, double result);
    void clear();
    const std::vector<HistoryEntry>& entries() const;

private:
    std::vector<HistoryEntry> items;
    static constexpr size_t kMaxEntries = 50;
};

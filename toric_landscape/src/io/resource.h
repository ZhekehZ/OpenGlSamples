#pragma once

#include <filesystem>
#include <string>
#include <utility>

struct Resource {
    Resource(std::filesystem::path path = "assets") : path(std::move(path)) {}

    template <typename ... Strings>
    [[nodiscard]] std::filesystem::path get(Strings const & ... ss) const {
        auto result = path;
        (... , (result = result.append(ss)));
        return result;
    }

    template <typename ... Strings>
    [[nodiscard]] std::string get_s(Strings const & ... ss) const {
        return get(ss...).string();
    }

    [[maybe_unused]] [[nodiscard]] auto iterate() const {
        return std::filesystem::directory_iterator(path);
    }

    std::filesystem::path path;
};

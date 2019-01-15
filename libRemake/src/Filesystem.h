#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <optional>
#include <Types/string_view.h>

namespace g2 {
std::optional<fs::path> find_file_recursive( string_view path, string_view name );
} // namespace g2

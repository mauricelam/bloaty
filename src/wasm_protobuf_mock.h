#ifndef WASM_PROTOBUF_MOCK_H_
#define WASM_PROTOBUF_MOCK_H_

#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

/**
 * This is a minimal mock of the protobuf-generated classes and Arena.
 * It is used to remove the protobuf dependency from the WebAssembly build.
 */

namespace google {
namespace protobuf {
class Arena {
 public:
  Arena() {}
  ~Arena() {
    for (auto& alloc : allocations_) {
      alloc.deleter(alloc.ptr);
    }
  }

  template <typename T>
  T* CreateArray(size_t n) {
    T* ptr = new T[n];
    allocations_.push_back({ptr, [](void* p) { delete[] static_cast<T*>(p); }});
    return ptr;
  }

  template <typename T>
  static T* CreateArray(Arena* arena, size_t n) {
    return arena->CreateArray<T>(n);
  }

 private:
  struct Allocation {
    void* ptr;
    void (*deleter)(void*);
  };
  std::vector<Allocation> allocations_;
};

namespace io {
// Not implemented, but needed for compilation if not guarded
class IstreamInputStream;
}
class TextFormat;
}  // namespace protobuf
}  // namespace google

namespace bloaty {

class Regex {
 public:
  void set_pattern(const std::string& p) { pattern_ = p; }
  const std::string& pattern() const { return pattern_; }
  void set_replacement(const std::string& r) { replacement_ = r; }
  const std::string& replacement() const { return replacement_; }
  bool has_pattern() const { return !pattern_.empty(); }
  bool has_replacement() const { return !replacement_.empty(); }
 private:
  std::string pattern_;
  std::string replacement_;
};

class CustomDataSource {
 public:
  void set_name(const std::string& n) { name_ = n; }
  const std::string& name() const { return name_; }
  void set_base_data_source(const std::string& b) { base_data_source_ = b; }
  const std::string& base_data_source() const { return base_data_source_; }

  Regex* add_rewrite() {
    rewrites_.emplace_back();
    return &rewrites_.back();
  }
  const std::vector<Regex>& rewrite() const { return rewrites_; }
  int rewrite_size() const { return rewrites_.size(); }

 private:
  std::string name_;
  std::string base_data_source_;
  std::vector<Regex> rewrites_;
};

class Options {
 public:
  enum Demangle {
    DEMANGLE_SHORT = 0,
    DEMANGLE_FULL = 1,
    DEMANGLE_NONE = 2
  };
  enum SortBy {
    SORTBY_BOTH = 0,
    SORTBY_VMSIZE = 1,
    SORTBY_FILESIZE = 2
  };

  void add_filename(const std::string& f) { filenames_.push_back(f); }
  const std::vector<std::string>& filename() const { return filenames_; }
  int filename_size() const { return filenames_.size(); }

  void add_base_filename(const std::string& f) { base_filenames_.push_back(f); }
  const std::vector<std::string>& base_filename() const { return base_filenames_; }
  int base_filename_size() const { return base_filenames_.size(); }

  void add_debug_filename(const std::string& f) { debug_filenames_.push_back(f); }
  const std::vector<std::string>& debug_filename() const { return debug_filenames_; }
  int debug_filename_size() const { return debug_filenames_.size(); }

  void add_source_map(const std::string& s) { source_maps_.push_back(s); }
  const std::vector<std::string>& source_map() const { return source_maps_; }

  void add_dsym_path(const std::string& p) { dsym_paths_.push_back(p); }
  const std::vector<std::string>& dsym_path() const { return dsym_paths_; }

  void add_data_source(const std::string& d) { data_sources_.push_back(d); }
  const std::vector<std::string>& data_source() const { return data_sources_; }
  int data_source_size() const { return data_sources_.size(); }

  void set_max_rows_per_level(int64_t m) { max_rows_per_level_ = m; has_max_rows_per_level_ = true; }
  int64_t max_rows_per_level() const { return max_rows_per_level_; }
  bool has_max_rows_per_level() const { return has_max_rows_per_level_; }

  void set_demangle(Demangle d) { demangle_ = d; }
  Demangle demangle() const { return demangle_; }

  void set_sort_by(SortBy s) { sort_by_ = s; has_sort_by_ = true; }
  SortBy sort_by() const { return sort_by_; }
  bool has_sort_by() const { return has_sort_by_; }

  void set_verbose_level(int32_t v) { verbose_level_ = v; }
  int32_t verbose_level() const { return verbose_level_; }

  void set_debug_vmaddr(uint64_t v) { debug_vmaddr_ = v; has_debug_vmaddr_ = true; }
  uint64_t debug_vmaddr() const { return debug_vmaddr_; }
  bool has_debug_vmaddr() const { return has_debug_vmaddr_; }

  void set_debug_fileoff(uint64_t f) { debug_fileoff_ = f; has_debug_fileoff_ = true; }
  uint64_t debug_fileoff() const { return debug_fileoff_; }
  bool has_debug_fileoff() const { return has_debug_fileoff_; }

  CustomDataSource* add_custom_data_source() {
    custom_data_sources_.emplace_back();
    return &custom_data_sources_.back();
  }
  const std::vector<CustomDataSource>& custom_data_source() const { return custom_data_sources_; }

  void set_disassemble_function(const std::string& f) { disassemble_function_ = f; }
  const std::string& disassemble_function() const { return disassemble_function_; }
  bool has_disassemble_function() const { return !disassemble_function_.empty(); }
  std::string* mutable_disassemble_function() { return &disassemble_function_; }

  void set_source_filter(const std::string& f) { source_filter_ = f; }
  const std::string& source_filter() const { return source_filter_; }
  bool has_source_filter() const { return !source_filter_.empty(); }

  void set_dump_raw_map(bool d) { dump_raw_map_ = d; }
  bool dump_raw_map() const { return dump_raw_map_; }

  Options() : max_rows_per_level_(20), has_max_rows_per_level_(false),
              demangle_(DEMANGLE_SHORT), sort_by_(SORTBY_BOTH), has_sort_by_(false),
              verbose_level_(0), debug_vmaddr_(0), has_debug_vmaddr_(false),
              debug_fileoff_(0), has_debug_fileoff_(false), dump_raw_map_(false) {}

 private:
  std::vector<std::string> filenames_;
  std::vector<std::string> base_filenames_;
  std::vector<std::string> debug_filenames_;
  std::vector<std::string> source_maps_;
  std::vector<std::string> dsym_paths_;
  std::vector<std::string> data_sources_;
  int64_t max_rows_per_level_;
  bool has_max_rows_per_level_;
  Demangle demangle_;
  SortBy sort_by_;
  bool has_sort_by_;
  int32_t verbose_level_;
  uint64_t debug_vmaddr_;
  bool has_debug_vmaddr_;
  uint64_t debug_fileoff_;
  bool has_debug_fileoff_;
  std::vector<CustomDataSource> custom_data_sources_;
  std::string disassemble_function_;
  std::string source_filter_;
  bool dump_raw_map_;
};

} // namespace bloaty

#endif

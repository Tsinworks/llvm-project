#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"

namespace metal {
static constexpr const char metallib_magic[4]{'M', 'T', 'L', 'B'};

LLVM_PACKED(struct version {
  union {
    struct {
      uint16_t major : 8;
      uint16_t rev : 4;   // lo
      uint16_t minor : 4; // hi
    };
    uint16_t packed = 0;
  };
});

LLVM_PACKED(struct metallib_version {
  // version of the container (NOTE: per-program Metal/AIR version info is
  // extra)
  version ver_lib;
  version ver_dwarf;
  // 2 (macOS), 3 (iOS), 5 (macOS since 10.16/11.0)
  uint32_t unkown_version;
  // 0
  uint32_t zero;
});
static_assert(sizeof(metallib_version) == 12, "invalid version header length");

LLVM_PACKED(struct metallib_header_control {
  uint64_t programs_offset{};
  uint64_t programs_length{};
  uint64_t reflection_offset{};
  uint64_t reflection_length{};
  uint64_t debug_offset{};
  uint64_t debug_length{};
  uint64_t bitcode_offset{};
  uint64_t bitcode_length{};
  uint32_t program_count{};
});
static_assert(sizeof(metallib_header_control) == 68,
              "invalid program info length");

LLVM_PACKED(struct metallib_header {
  const char magic[4]; // == metallib_magic
  const metallib_version version;
  const uint64_t file_length;
  const metallib_header_control header_control;
});
static_assert(sizeof(metallib_header) == 4 + sizeof(metallib_version) +
                                             sizeof(uint64_t) +
                                             sizeof(metallib_header_control),
              "invalid metallib header size");

struct LibraryMeta {
  uint32_t length; // including length itself

  // NOTE: tag types are always 32-bit
  // NOTE: tag types are always followed by a uint16_t that specifies the length
  // of the tag data
#define make_tag_type(a, b, c, d)                                              \
  ((uint32_t(d) << 24u) | (uint32_t(c) << 16u) | (uint32_t(b) << 8u) |         \
   uint32_t(a))
  enum Tag : uint32_t {
    // used in initial header section
    NAME = make_tag_type('N', 'A', 'M', 'E'),
    TYPE = make_tag_type('T', 'Y', 'P', 'E'),
    HASH = make_tag_type('H', 'A', 'S', 'H'),
    MD_SIZE = make_tag_type('M', 'D', 'S', 'Z'),
    OFFSET = make_tag_type('O', 'F', 'F', 'T'),
    VERSION = make_tag_type('V', 'E', 'R', 'S'),
    // used in reflection section
    CNST = make_tag_type('C', 'N', 'S', 'T'),
    VATT = make_tag_type('V', 'A', 'T', 'T'),
    VATY = make_tag_type('V', 'A', 'T', 'Y'),
    RETR = make_tag_type('R', 'E', 'T', 'R'),
    ARGR = make_tag_type('A', 'R', 'G', 'R'),
    // used in debug section
    DEBI = make_tag_type('D', 'E', 'B', 'I'),
    // TODO/TBD
    LAYR = make_tag_type('L', 'A', 'Y', 'R'),
    TESS = make_tag_type('T', 'E', 'S', 'S'),
    SOFF = make_tag_type('S', 'O', 'F', 'F'),
    // generic end tag
    END = make_tag_type('E', 'N', 'D', 'T'),
  };
#undef make_tag_type

  enum class FunctionType : uint8_t {
    VERTEX = 0,
    FRAGMENT = 1,
    KERNEL = 2,
    // TODO: tessellation?
    NONE = 255
  };

  struct VersionInfo {
    uint32_t major : 16;
    uint32_t minor : 8;
    uint32_t rev : 8;
  };

  struct Offset {
    // NOTE: these are all relative offsets -> add to metallib_header_control
    // offsets to get absolute offsets
    uint64_t reflection_offset;
    uint64_t debug_offset;
    uint64_t bitcode_offset;
  };

  struct Sha256 {
    uint8_t data[32]{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
  };

  struct FunctionEntry {
    uint32_t length;
    llvm::StringRef name; // NOTE: limited to 65536 - 1 ('\0')
    FunctionType type{FunctionType::NONE};
    Sha256 sha256_hash;
    Offset offset{0, 0, 0};
    uint64_t bitcode_size{0}; // always 8 bytes
    VersionInfo api_version{0, 0, 0};
    VersionInfo language_version{0, 0, 0};
    uint8_t tess_info{0};
    uint64_t soffset{0};
  };

  llvm::SmallVector<FunctionEntry> entries;
}; // LibraryMeta

class Library {
public:
  Library(const uint8_t *data, size_t length);
  ~Library();

private:
  LibraryMeta meta;
};
} // namespace metal
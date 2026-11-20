#include "FreeTypeFontFixture.hpp"
#include "prx/libSceFreeTypeOl/libSceFreeTypeOl.hpp"
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {

void Require(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "%s\n", message);
    std::exit(1);
}

void* Load(const char* path) {
#ifdef _WIN32
    auto module = LoadLibraryA(path);
#else
    auto module = dlopen(path, RTLD_NOW | RTLD_LOCAL);
#endif
    Require(module != nullptr, path);
    return module;
}

template<typename Function>
Function Resolve(void* module, const char* nid) {
#ifdef _WIN32
    auto symbol = GetProcAddress(static_cast<HMODULE>(module), nid);
#else
    auto symbol = dlsym(module, nid);
#endif
    Require(symbol != nullptr, nid);
    return reinterpret_cast<Function>(symbol);
}

}

int main(int argc, char** argv) {
    Require(argc == 3, "expected FreeType and Ol module paths");
    auto* ft = Load(argv[1]);
    auto* ol = Load(argv[2]);
    auto init = Resolve<decltype(&FT_Init_FreeType_nid_postfix)>(ft, "GNsiLzm4SH8");
    auto openFace = Resolve<decltype(&FT_New_Memory_Face_nid_postfix)>(ft, "40Wa2neSLM0");
    auto closeFace = Resolve<decltype(&FT_Done_Face_nid_postfix)>(ft, "OgFczHko0w8");
    auto finish = Resolve<decltype(&FT_Done_FreeType_nid_postfix)>(ft, "dH02CpEEcYk");
    auto removeMap = Resolve<decltype(&sceFreeTypeOlCMapDone_nid_postfix)>(ol, "h0n4Fb9j2R8");

    const auto font = SquareGlyphFont('A', 700, true);
    FreeType::GuestLibrary* library = nullptr;
    FreeType::GuestFaceRec* face = nullptr;
    Require(init(&library) == 0, "initialize FreeType");
    Require(openFace(library, font.data(), font.size(), 0, &face) == 0, "open two-map font");
    Require(face->num_charmaps == 2 && face->charmap, "select initial map");
    auto* survivor = face->charmaps[face->charmaps[0] == face->charmap ? 1 : 0];
    removeMap(face->charmap);
    Require(face->num_charmaps == 1 && !face->charmap && face->charmaps[0] == survivor, "preserve surviving map");
    removeMap(survivor);
    Require(face->num_charmaps == 0 && !face->charmaps, "release last map");
    Require(closeFace(face) == 0 && finish(library) == 0, "close face and library");
}

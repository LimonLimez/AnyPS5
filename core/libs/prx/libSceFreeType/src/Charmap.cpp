#include "prx/libSceFreeType/include/Api.hpp"
#include "prx/libSceFreeType/include/GuestObjects.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"

#include FT_TRUETYPE_TABLES_H

#if defined(ANYPS5_FREETYPE_BUNDLED_CMAP)
#include "prx/libc/include/General.hpp"

#include <freetype/internal/ftobjs.h>
#include <cerrno>
#endif

#include <cstddef>
#include <cstdint>
#include <stdexcept>

using namespace FreeType;

namespace {

FT_CharMap HostCharmap(const GuestCharMapRec* charmap) {
    if (!charmap || !charmap->face) return nullptr;
    const GuestFace* owner = FaceOf(static_cast<const GuestFaceRec*>(charmap->face));
    if (owner && owner->host) {
        for (std::size_t index = 0; index < owner->charmapPointers.size(); ++index) {
            if (owner->charmapPointers[index] == charmap) return owner->host->charmaps[index];
        }
    }
    throw std::runtime_error("FreeType: a charmap outside the guest face's read-only array is not implemented");
}

}

extern "C" {

int APS5_VABI FT_Get_Charmap_Index_nid_postfix(GuestCharMapRec* charmap) {
    return FT_Get_Charmap_Index(HostCharmap(charmap));
}

#if defined(ANYPS5_FREETYPE_BUNDLED_CMAP)
void APS5_VABI FreeTypeDoneCharmap_nid_no_patch(GuestCharMapRec* charmap) {
    if (!charmap) return;
    GuestCharMapRec record{};
    const int savedErrno = errno;
    const bool copied = GuestCopyIn_nid_no_patch(&record, charmap, sizeof(record));
    errno = savedErrno;
    if (!copied) return;
    auto* owner = RegisteredGuestFace_nid_no_patch(static_cast<GuestFaceRec*>(record.face));
    if (!owner || !owner->host || owner->host->num_charmaps <= 0) return;
    const auto count = static_cast<std::size_t>(owner->host->num_charmaps);
    if (owner->charmapPointers.size() != count) return;
    for (std::size_t index = 0; index < owner->charmapPointers.size(); ++index) {
        if (owner->charmapPointers[index] != charmap) continue;
        FT_CMap_Done(FT_CMAP(owner->host->charmaps[index]));
        if (static_cast<std::size_t>(owner->host->num_charmaps) != count) SyncCharmaps_nid_no_patch(*owner);
        return;
    }
}
#endif

std::uint64_t APS5_VABI FT_Get_CMap_Language_ID_nid_postfix(GuestCharMapRec* charmap) {
    return FT_Get_CMap_Language_ID(HostCharmap(charmap));
}

std::int64_t APS5_VABI FT_Get_CMap_Format_nid_postfix(GuestCharMapRec* charmap) {
    return FT_Get_CMap_Format(HostCharmap(charmap));
}

}

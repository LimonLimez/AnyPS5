# libSceFreeTypeOl

## add Functional

- [sceFreeTypeOlCMapDone](../../../core/libs/prx/libSceFreeTypeOl/Export.cpp) (libSceFreeTypeOl) - Bundled builds remove host-backed charmaps from their owning face. System FreeType builds still throw because the private FreeType function is unavailable. Independent guest-created charmaps are unsupported. The `void` signature follows upstream FreeType and is unverified on console.

## change FT_CMap_New

- [`FT_Attach_Stream` and `FT_CMap_New`](../../../core/libs/prx/libSceFreeTypeOl/Export.cpp) (`ZqvTX8u1bEc`, `wLqQQWyOdpc`): direct native calls bypass the guest face and charmap layouts, stream and callback ABI, and lifecycle ownership. These exports still require a dedicated guest bridge.

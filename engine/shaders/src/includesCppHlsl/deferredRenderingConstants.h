#ifndef __INCLUDE_GUARD_deferredRenderingConstants_h__
#define __INCLUDE_GUARD_deferredRenderingConstants_h__



// Surface properties G-buffer meaning (R8G8B8A8_UNORM):
#define DEFERRED_SURFACE_PROPERTIES_METALLICITY_CHANNEL 0
#define DEFERRED_SURFACE_PROPERTIES_ROUGHNESS_CHANNEL 1
#define DEFERRED_SURFACE_PROPERTIES_AMBIENT_OCCLUSION_CHANNEL 2
#define DEFERRED_SURFACE_PROPERTIES_FLAG_BIT_MASK_CHANNEL 3

// Surface properties alpha channel flag interpretation:
#define DEFERRED_SURFACE_FLAG_RECEIVE_SHADOWS (1u << 0)
#define DEFERRED_SURFACE_FLAG_LIT (2u << 0)
#define DEFERRED_SURFACE_FLAG_BIT_MASK_MAX 255u



#endif // __INCLUDE_GUARD_deferredRenderingConstants_h__
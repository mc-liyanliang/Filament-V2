/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <filament/MaterialInstance.h>

#include "details/Material.h"

namespace filament {

using namespace math;
using namespace backend;

// ------------------------------------------------------------------------------------------------

// This is the untyped/sized version of the setParameter: we end up here for e.g. vec4<int> and
// vec4<float>. This must not be inlined (this is the whole point).
template<size_t Size>
UTILS_NOINLINE
void FMaterialInstance::setParameterUntypedImpl(const char* name, const void* value) noexcept {
    ssize_t offset = mMaterial->getUniformInterfaceBlock().getUniformOffset(name, 0);
    if (UTILS_LIKELY(offset >= 0)) {
        mUniforms.setUniformUntyped<Size>(size_t(offset), value);  // handles specialization for mat3f
    }
}

// ------------------------------------------------------------------------------------------------

// this converts typed calls into the untyped-sized call.
template<typename T>
UTILS_ALWAYS_INLINE
inline void FMaterialInstance::setParameterImpl(const char* name, T const& value) noexcept {
    static_assert(!std::is_same_v<T, math::mat3f>);
    setParameterUntypedImpl<sizeof(T)>(name, &value);
}

// specialization for mat3f
template<>
inline void FMaterialInstance::setParameterImpl(const char* name, mat3f const& value) noexcept {
    ssize_t offset = mMaterial->getUniformInterfaceBlock().getUniformOffset(name, 0);
    if (UTILS_LIKELY(offset >= 0)) {
        mUniforms.setUniform(size_t(offset), value);
    }
}

// ------------------------------------------------------------------------------------------------

template<size_t Size>
UTILS_NOINLINE
void FMaterialInstance::setParameterUntypedImpl(const char* name, const void* value, size_t count) noexcept {
    ssize_t offset = mMaterial->getUniformInterfaceBlock().getUniformOffset(name, 0);
    if (UTILS_LIKELY(offset >= 0)) {
        mUniforms.setUniformArrayUntyped<Size>(size_t(offset), value, count);
    }
}

template<typename T>
UTILS_ALWAYS_INLINE
inline void FMaterialInstance::setParameterImpl(const char* name, const T* value, size_t count) noexcept {
    static_assert(!std::is_same_v<T, math::mat3f>);
    setParameterUntypedImpl<sizeof(T)>(name, value, count);
}

// ------------------------------------------------------------------------------------------------

template <typename T, typename>
void MaterialInstance::setParameter(const char* name, T const& value) noexcept {
    upcast(this)->setParameterImpl(name, value);
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, bool const& v) noexcept {
    // this kills tail-call optimization
    MaterialInstance::setParameter(name, (uint32_t)v);
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, bool2 const& v) noexcept {
    // this kills tail-call optimization
    MaterialInstance::setParameter(name, uint2(v));
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, bool3 const& v) noexcept {
    // this kills tail-call optimization
    MaterialInstance::setParameter(name, uint3(v));
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, bool4 const& v) noexcept {
    // this kills tail-call optimization
    MaterialInstance::setParameter(name, uint4(v));
}

// explicit template instantiation of our supported types
template UTILS_PUBLIC void MaterialInstance::setParameter<float>   (const char* name, float const&    v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int32_t> (const char* name, int32_t const&  v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint32_t>(const char* name, uint32_t const& v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int2>    (const char* name, int2 const&     v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int3>    (const char* name, int3 const&     v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int4>    (const char* name, int4 const&     v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint2>   (const char* name, uint2 const&    v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint3>   (const char* name, uint3 const&    v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint4>   (const char* name, uint4 const&    v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<float2>  (const char* name, float2 const&   v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<float3>  (const char* name, float3 const&   v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<float4>  (const char* name, float4 const&   v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<mat3f>   (const char* name, mat3f const&    v) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<mat4f>   (const char* name, mat4f const&    v) noexcept;

// ------------------------------------------------------------------------------------------------

template <typename T, typename>
void MaterialInstance::setParameter(const char* name, const T* value, size_t count) noexcept {
    upcast(this)->setParameterImpl(name, value, count);
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, const bool* v, size_t c) noexcept {
    auto* p = new uint32_t[c];
    std::copy_n(v, c, p);
    MaterialInstance::setParameter(name, p, c);
    delete [] p;
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, const bool2* v, size_t c) noexcept {
    auto* p = new uint2[c];
    std::copy_n(v, c, p);
    MaterialInstance::setParameter(name, p, c);
    delete [] p;
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, const bool3* v, size_t c) noexcept {
    auto* p = new uint3[c];
    std::copy_n(v, c, p);
    MaterialInstance::setParameter(name, p, c);
    delete [] p;
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter(const char* name, const bool4* v, size_t c) noexcept {
    auto* p = new uint4[c];
    std::copy_n(v, c, p);
    MaterialInstance::setParameter(name, p, c);
    delete [] p;
}

template<>
UTILS_PUBLIC void MaterialInstance::setParameter<mat3f>(const char* name, const mat3f* v, size_t c) noexcept {
    // pretend each mat3 is an array of 3 float3
    MaterialInstance::setParameter(name, reinterpret_cast<math::float3 const*>(v), c * 3);
}

template UTILS_PUBLIC void MaterialInstance::setParameter<float>   (const char* name, const float    *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int32_t> (const char* name, const int32_t  *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint32_t>(const char* name, const uint32_t *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int2>    (const char* name, const int2     *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int3>    (const char* name, const int3     *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<int4>    (const char* name, const int4     *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint2>   (const char* name, const uint2    *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint3>   (const char* name, const uint3    *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<uint4>   (const char* name, const uint4    *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<float2>  (const char* name, const float2   *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<float3>  (const char* name, const float3   *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<float4>  (const char* name, const float4   *v, size_t c) noexcept;
template UTILS_PUBLIC void MaterialInstance::setParameter<mat4f>   (const char* name, const mat4f    *v, size_t c) noexcept;

// ------------------------------------------------------------------------------------------------

Material const* MaterialInstance::getMaterial() const noexcept {
    return upcast(this)->getMaterial();
}

const char* MaterialInstance::getName() const noexcept {
    return upcast(this)->getName();
}

void MaterialInstance::setParameter(const char* name, Texture const* texture,
        TextureSampler const& sampler) noexcept {
    return upcast(this)->setParameterImpl(name, texture, sampler);
}

void MaterialInstance::setParameter(const char* name, RgbType type, float3 color) noexcept {
    upcast(this)->setParameterImpl<float3>(name, Color::toLinear(type, color));
}

void MaterialInstance::setParameter(const char* name, RgbaType type, float4 color) noexcept {
    upcast(this)->setParameterImpl<float4>(name, Color::toLinear(type, color));
}

void MaterialInstance::setScissor(uint32_t left, uint32_t bottom, uint32_t width,
        uint32_t height) noexcept {
    upcast(this)->setScissor(left, bottom, width, height);
}

void MaterialInstance::unsetScissor() noexcept {
    upcast(this)->unsetScissor();
}

void MaterialInstance::setPolygonOffset(float scale, float constant) noexcept {
    upcast(this)->setPolygonOffset(scale, constant);
}

void MaterialInstance::setMaskThreshold(float threshold) noexcept {
    upcast(this)->setMaskThreshold(threshold);
}

void MaterialInstance::setSpecularAntiAliasingVariance(float variance) noexcept {
    upcast(this)->setSpecularAntiAliasingVariance(variance);
}

void MaterialInstance::setSpecularAntiAliasingThreshold(float threshold) noexcept {
    upcast(this)->setSpecularAntiAliasingThreshold(threshold);
}

void MaterialInstance::setDoubleSided(bool doubleSided) noexcept {
    upcast(this)->setDoubleSided(doubleSided);
}

void MaterialInstance::setTransparencyMode(TransparencyMode mode) noexcept {
    upcast(this)->setTransparencyMode(mode);
}

void MaterialInstance::setCullingMode(CullingMode culling) noexcept {
    upcast(this)->setCullingMode(culling);
}

void MaterialInstance::setColorWrite(bool enable) noexcept {
    upcast(this)->setColorWrite(enable);
}

void MaterialInstance::setDepthWrite(bool enable) noexcept {
    upcast(this)->setDepthWrite(enable);
}

void MaterialInstance::setDepthCulling(bool enable) noexcept {
    upcast(this)->setDepthCulling(enable);
}

MaterialInstance* MaterialInstance::duplicate(MaterialInstance const* other, const char* name) noexcept {
    return FMaterialInstance::duplicate(upcast(other), name);
}


} // namespace filament

// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_COMMON_GRAPHICS_TEXTURE_H_
#define FLUTTER_COMMON_GRAPHICS_TEXTURE_H_

#include <map>

#include "flutter/fml/macros.h"
#include "flutter/fml/synchronization/waitable_event.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSamplingOptions.h"

class GrDirectContext;

namespace flutter {

class ContextListener {
 public:
  ContextListener();
  ~ContextListener();

  // Called from raster thread.
  virtual void OnGrContextCreated() = 0;

  // Called from raster thread.
  virtual void OnGrContextDestroyed() = 0;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(ContextListener);
};

class Texture : public ContextListener {
 public:
  explicit Texture(int64_t id);  // Called from UI or raster thread.
  virtual ~Texture();            // Called from raster thread.

  // Called from raster thread.
  virtual void Paint(SkCanvas& canvas,
                     const SkRect& bounds,
                     bool freeze,
                     GrDirectContext* context,
                     const SkSamplingOptions& sampling,
                     const SkPaint* paint = nullptr) = 0;

  // Called on raster thread.
  virtual void MarkNewFrameAvailable() = 0;

  // Called on raster thread.
  virtual void OnTextureUnregistered() = 0;

  int64_t Id() { return id_; }

 private:
  int64_t id_;
  FML_DISALLOW_COPY_AND_ASSIGN(Texture);
};

class TextureRegistry {
 public:
  TextureRegistry();

  // Called from raster thread.
  void RegisterTexture(const std::shared_ptr<Texture>& texture);

  // Called from raster thread.
  void RegisterContextListener(uintptr_t id,
                               std::weak_ptr<ContextListener> image);

  // Called from raster thread.
  void UnregisterTexture(int64_t id);

  // Called from the raster thread.
  void UnregisterContextListener(uintptr_t id);

  // Called from raster thread.
  std::shared_ptr<Texture> GetTexture(int64_t id);

  // Called from raster thread.
  void OnGrContextCreated();

  // Called from raster thread.
  void OnGrContextDestroyed();

 private:
  std::map<int64_t, std::shared_ptr<Texture>> mapping_;
  std::map<uintptr_t, std::weak_ptr<ContextListener>> images_;

  FML_DISALLOW_COPY_AND_ASSIGN(TextureRegistry);
};

}  // namespace flutter

#endif  // FLUTTER_COMMON_GRAPHICS_TEXTURE_H_

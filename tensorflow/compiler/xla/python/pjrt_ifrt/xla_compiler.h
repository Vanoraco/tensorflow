/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_XLA_PYTHON_PJRT_IFRT_XLA_COMPILER_H_
#define TENSORFLOW_COMPILER_XLA_PYTHON_PJRT_IFRT_XLA_COMPILER_H_

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "llvm/Support/ExtensibleRTTI.h"
#include "mlir/IR/BuiltinOps.h"  // from @llvm-project
#include "tensorflow/compiler/xla/python/ifrt/compiler.h"
#include "tensorflow/compiler/xla/python/ifrt/host_callback.h"

namespace xla {
namespace ifrt {

struct XlaProgram : llvm::RTTIExtends<XlaProgram, Program> {
  XlaProgram() = default;
  explicit XlaProgram(mlir::ModuleOp mlir_module)
      : mlir_module(std::move(mlir_module)) {}

  mlir::ModuleOp mlir_module;

  static char ID;  // NOLINT
};

// Wraps compilation options for an XLA computation.
//
// TODO(hyeontaek): Move this class out of pjrt_ifrt.
//
// TODO(hyeontaek): Move `loaded_host_callbacks` to a (new) `LoadOptions`
// because compilation (without loading) should not take them.
struct XlaCompileOptions
    : llvm::RTTIExtends<XlaCompileOptions, CompileOptions> {
  XlaCompileOptions() = default;
  explicit XlaCompileOptions(xla::CompileOptions compile_options,
                             std::vector<tsl::RCReference<LoadedHostCallback>>
                                 loaded_host_callbacks = {})
      : compile_options(std::move(compile_options)),
        loaded_host_callbacks(std::move(loaded_host_callbacks)) {}

  xla::CompileOptions compile_options;
  std::vector<tsl::RCReference<LoadedHostCallback>> loaded_host_callbacks;

  // CompileOptions implementation.

  ~XlaCompileOptions() override = default;

  static char ID;  // NOLINT
};

// Wraps deserialization options for an XLA computation.
//
// TODO(hyeontaek): Move this class out of pjrt_ifrt.
//
// TODO(hyeontaek): Move `loaded_host_callbacks` to a (new) `LoadOptions`
// because deserialization (without loading) should not take them.
struct XlaDeserializeOptions
    : llvm::RTTIExtends<XlaDeserializeOptions, DeserializeOptions> {
  XlaDeserializeOptions() = default;
  explicit XlaDeserializeOptions(
      std::optional<xla::CompileOptions> compile_options,
      std::vector<tsl::RCReference<LoadedHostCallback>> loaded_host_callbacks =
          {})
      : compile_options(std::move(compile_options)),
        loaded_host_callbacks(std::move(loaded_host_callbacks)) {}

  // `compile_options` may be unspecified if deserialization does not override
  // it.
  std::optional<xla::CompileOptions> compile_options;
  std::vector<tsl::RCReference<LoadedHostCallback>> loaded_host_callbacks;

  // DeserializeOptions implementation.

  ~XlaDeserializeOptions() override = default;

  static char ID;  // NOLINT
};

// Gets `xla::ifrt::XlaCompileOptions` from `xla::ifrt::CompileOptions`.
StatusOr<std::unique_ptr<XlaCompileOptions>> GetXlaCompileOptions(
    std::unique_ptr<CompileOptions> options);

// Gets `xla::ifrt::XlaDeserializeOptions` from `xla::ifrt::DeserializeOptions`.
StatusOr<std::unique_ptr<XlaDeserializeOptions>> GetXlaDeserializeOptions(
    std::unique_ptr<DeserializeOptions> options);

}  // namespace ifrt
}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_PYTHON_PJRT_IFRT_XLA_COMPILER_H_

#ifndef INCLUDE_ENJAM_DEPS_H_
#define INCLUDE_ENJAM_DEPS_H_

#include <enjam/application.h>
#include <njctr/njctr.h>

namespace Enjam {

template<class T>
using IFactory = njctr::IFactory<T>;

using Injector = njctr::Injector<
    IFactory<Simulation()>,
    njctr::SharedResolver<Renderer>,
    njctr::SharedResolver<Input>,
    njctr::SharedResolver<renderer::RendererBackend>,
    njctr::SharedResolver<Scene>,
    njctr::SharedResolver<Camera>>;
}

#endif //INCLUDE_ENJAM_DEPS_H_

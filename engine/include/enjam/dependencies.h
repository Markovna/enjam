#ifndef INCLUDE_ENJAM_DEPS_H_
#define INCLUDE_ENJAM_DEPS_H_

#include <enjam/application.h>
#include <enjector/enjector.h>
#include <enjector/factories.h>

namespace Enjam {

using Dependencies = ::enjector::Dependencies<
    ::enjector::IFactory<Simulation>,
    Renderer,
    Input,
    renderer::RendererBackend,
    Scene,
    Camera>;

}

#endif //INCLUDE_ENJAM_DEPS_H_

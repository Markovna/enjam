#ifndef INCLUDE_ENJAM_DEPS_H_
#define INCLUDE_ENJAM_DEPS_H_

#include <njctr/njctr.h>

namespace Enjam {

class Simulation;
class Renderer;
class Input;
class RendererBackend;
class Scene;
class Camera;

using Injector = njctr::Injector<
    njctr::IFactory<Simulation()>,
    njctr::SharedResolver<Renderer>,
    njctr::SharedResolver<Input>,
    njctr::SharedResolver<RendererBackend>,
    njctr::SharedResolver<Scene>,
    njctr::SharedResolver<Camera>>;
}

#endif //INCLUDE_ENJAM_DEPS_H_

#include <Corrade/PluginManager/AbstractManager.h>
#include <Magnum/GL/OpenGL.h>
#include <Magnum/Platform/GLContext.h>
#include <dt/df/plugin/plugin.hpp>
#include <imnodes.h>
#include "nodes/color_cvt.hpp"
#include "nodes/mat_copy.hpp"
#include "nodes/mat_display.hpp"
#include "nodes/mat_output.hpp"
#include "nodes/video_capture.hpp"
#include "slots/mat.hpp"

namespace dt::df::plugin
{

template <typename TNode>
void registerNode(IGraphManager &graph, const std::string &group)
{
    graph.registerNodeFactory(
        TNode::kKey,
        group + std::string{TNode::kName},
        [](IGraphManager &graph) { return std::make_shared<TNode>(graph); },
        [](IGraphManager &graph, const nlohmann::json &json) { return std::make_shared<TNode>(graph, json); });
}
template <typename TSlot>
void registerSlot(IGraphManager &graph)
{
    graph.registerSlotFactory(
        TSlot::kKey,
        [](IGraphManager &graph, SlotType type, const SlotName &name, SlotId local, SlotFieldVisibility vis) {
            return std::make_shared<TSlot>(TSlot::kKey, graph, type, name, local, vis);
        },
        [](const nlohmann::json &json) { return std::make_shared<TSlot>(json); });
}
class OpenCvPlugin final : public Plugin
{

  public:
    explicit OpenCvPlugin(Corrade::PluginManager::AbstractManager &manager, const std::string &plugin)
        : Plugin{manager, plugin}
    {}

    void setup(Magnum::GL::Context &gl_ctx, ImGuiContext *imgui_ctx, imnodes::Context *imnodes_ctx)
    {
        Magnum::Platform::GLContext::makeCurrent(&gl_ctx);
        ImGui::SetCurrentContext(imgui_ctx);
        imnodes::SetCurrentContext(imnodes_ctx);
    }
    void registerNodeFactories(IGraphManager &graph)
    {
        registerNode<opencv::VideoCaptureNode>(graph, "opencv/");
        registerNode<opencv::MatDisplayNode>(graph, "opencv/");
        registerNode<opencv::MatCopyNode>(graph, "opencv/");
        registerNode<opencv::MatOutputNode>(graph, "opencv/");
        registerNode<opencv::ColorCvtNode>(graph, "opencv/");
    }
    void registerSlotFactories(IGraphManager &graph)
    {
        registerSlot<opencv::MatSlot>(graph);
    }
};
} // namespace dt::df::plugin

CORRADE_PLUGIN_REGISTER(OpenCvPlugin, dt::df::plugin::OpenCvPlugin, "de.mathisloge.dt.dataflow/1.0")

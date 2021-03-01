#include <Corrade/PluginManager/AbstractManager.h>
#include <dt/df/plugin/plugin.hpp>
#include <imnodes.h>

namespace dt::df::plugin
{

class MyPlugin final : public Plugin
{

  public:
    explicit MyPlugin(Corrade::PluginManager::AbstractManager &manager, const std::string &plugin)
        : Plugin{manager, plugin}
    {}

    void setup(ImGuiContext *imgui_ctx, imnodes::Context *imnodes_ctx)
    {
        ImGui::SetCurrentContext(imgui_ctx);
        imnodes::SetCurrentContext(imnodes_ctx);
    }
    void registerNodeFactories(IGraphManager &graph)
    {}
    void registerSlotFactories(IGraphManager &graph)
    {}
};
} // namespace dt::df::plugin

CORRADE_PLUGIN_REGISTER(MyPlugin, dt::df::plugin::MyPlugin, "de.mathisloge.dt.dataflow/1.0")

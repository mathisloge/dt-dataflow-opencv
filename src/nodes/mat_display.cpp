#include "mat_display.hpp"
#include <Corrade/Containers/ArrayView.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/ImageView.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Range.h>
#include <Magnum/PixelFormat.h>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <opencv2/imgproc.hpp>
#include "../slots/mat.hpp"

namespace dt::df::plugin::opencv
{
MatDisplayNode::MatDisplayNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, CreateInputs(graph_manager), Slots{}}
    , mat_{nullptr}
{
    initSlots();
}
MatDisplayNode::MatDisplayNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
    , mat_{nullptr}
{
    initSlots();
}

void MatDisplayNode::initSlots()
{
    std::static_pointer_cast<MatSlot>(inputByLocalId(0))->subscribe([this](const BaseSlot *slot) {
        auto in_mat = static_cast<const MatSlot *>(slot)->value();
        std::unique_lock<std::mutex> l{tex_mtx_};
        mat_ = in_mat;
        mat_changed_ = true;
    });
}

void MatDisplayNode::renderCustomContent()
{
    using namespace Magnum;
    ImGui::Text("Current Mat:");
    ImGui::Value("W", mat_ ? mat_->size().width : 0);
    ImGui::Value("H", mat_ ? mat_->size().height : 0);
    if (mat_changed_ && mat_)
    {

        std::unique_lock<std::mutex> l{tex_mtx_};
        mat_changed_ = false;
        cv::Mat img_mat;
        cv::cvtColor(*mat_, img_mat, cv::COLOR_BGR2RGB);
        Corrade::Containers::ArrayView arr_view(img_mat.data, img_mat.total() * img_mat.elemSize());
        ImageView2D image(PixelFormat::RGB8Unorm, {img_mat.size().width, img_mat.size().height}, arr_view);

        tex_.setMagnificationFilter(GL::SamplerFilter::Linear)
            .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
            .setStorage(1, GL::TextureFormat::RGBA8, image.size())
            .setSubImage(0, {}, image)
            .generateMipmap();
        size_ = ImVec2{static_cast<float>(image.size().x()), static_cast<float>(image.size().y())};
    }
    ImGui::Image(static_cast<ImTextureID>(&tex_), size_ / 3.f);
}

Slots MatDisplayNode::CreateInputs(IGraphManager &graph_manager)
{
    try
    {
        const auto &mat_slot_fac = graph_manager.getSlotFactory("OpenCvMatSlot");
        return Slots{mat_slot_fac(graph_manager, SlotType::input, "mat", 0, SlotFieldVisibility::never)};
    }
    catch (...)
    {}
    return Slots{};
}
} // namespace dt::df::plugin::opencv

#include "pch.h"

#include "graph/channel_graph_dump.h"

#include "quest3d/api.h"
#include "util/text_convert.h"

using namespace quest3d::api;

namespace
{
std::string get_channel_value(A3d_Channel* channel)
{
    OLECHAR* guid_olechar;
    StringFromCLSID(channel->GetChannelType().guid, &guid_olechar);
    std::wstring wstring = std::wstring(guid_olechar);
    std::string guidstr = util::utf8_encode(wstring);
    auto guid(channel->GetChannelType().guid);

    if(guid == STRING_GUID) {
        auto channel_string = StringChannel_GetString((Aco_StringChannel*)channel);
        if(channel_string) {
            return channel_string;
        }
        return "(nullptr)";
    }
    if(guid == FLOAT_CHANNEL_GUID) {
        std::string result_string;
        result_string += std::to_string(Aco_FloatChannel_GetFloat(channel));
        result_string += "\nDefault Value: ";
        result_string += std::to_string(Aco_FloatChannel_GetDefaultFloat(channel));
        return result_string;
    }
    if(strstr(guidstr.c_str(), "F26BB40B-B196")) {
        auto channel_string = StringOperator_GetString(channel);
        if(channel_string) {
            return channel_string;
        }
        return "(nullptr)";
    }

    return "";
}
} // namespace

namespace graph
{
void write_channel_graph(A3d_ChannelGroup* group, UGraphviz::Graph* graph)
{
    auto& registry = graph->GetRegistry();

    for(int i {}; i < 50000; ++i) {
        A3d_Channel* channel = ChannelGroup_GetChannel(group, i);
        if(channel) {
            GUID channel_guid(channel->GetChannelType().guid);
            auto node = registry.RegisterNode(std::to_string(Channel_GetChannelIDIndexNr(channel)));

            std::string node_label;
            node_label += Channel_GetChannelName(channel);
            node_label += "\\n";
            node_label += channel->GetChannelType().name;
            std::string channel_value = get_channel_value(channel);
            // Backslashes need to be escaped.
            util::replace_all(channel_value, "\\", "\\\\");
            // Graphviz can't cope with newlines either, so they need to be replaced with the literal "\n".
            util::replace_all(channel_value, "\r\n", "\\n");
            util::replace_all(channel_value, "\n", "\\n");
            // Escape quotes as well.
            util::replace_all(channel_value, "\"", "\\\"");
            if(!channel_value.empty()) {
                node_label += "\\nChannel value: ";
                node_label += channel_value;
            }
            registry.RegisterNodeAttr(node, UGraphviz::Attrs_label, node_label);
            registry.RegisterNodeAttr(node, UGraphviz::Attrs_shape, "box");
            if(Channel_GetChannelIDIndexNr(channel) == 0) {
                registry.RegisterNodeAttr(node, UGraphviz::Attrs_color, "green");
            }
            graph->AddNode(node);
        }
    }

    for(int channelnum {}; channelnum < 50000; ++channelnum) {
        A3d_Channel* channel = ChannelGroup_GetChannel(group, channelnum);

        if(channel) {
            const int children = Channel_GetChildCount(channel);

            for(int childnum {}; childnum < children; ++childnum) {
                A3d_Channel* child(Channel_GetChild(channel, childnum));

                if(child) {
                    if(registry.IsRegisteredNode(std::to_string(Channel_GetChannelIDIndexNr(child)))
                        && registry.IsRegisteredNode(std::to_string(Channel_GetChannelIDIndexNr(channel)))) {
                        auto edge = registry.RegisterEdge(registry.GetNodeIndex(std::to_string(Channel_GetChannelIDIndexNr(channel))),
                            registry.GetNodeIndex(std::to_string(Channel_GetChannelIDIndexNr(child))));
                        graph->AddEdge(edge);
                    }
                }
            }
        }
    }
}
} // namespace graph

#include "pch.h"

#include "graph/channel_graph_dump.h"

#include "quest3d/api.h"
#include "util/text_convert.h"

using namespace quest3d::api;

namespace graph
{

namespace
{

std::string get_channel_value(A3d_Channel* channel)
{
    OLECHAR* guid_OLECHAR;
    StringFromCLSID(channel->GetChannelType().guid, &guid_OLECHAR);
    std::wstring wstring = std::wstring(guid_OLECHAR);
    std::string guidstr = util::Utf8Encode(wstring);
    auto guid(channel->GetChannelType().guid);

    if(guid == STRING_GUID) {
        auto channelString = StringChannel_GetString((Aco_StringChannel*)channel);
        if(channelString) {
            return channelString;
        }
        return "(nullptr)";
    }
    if(guid == FLOAT_CHANNEL_GUID) {
        std::string resultString;
        resultString += std::to_string(Aco_FloatChannel_GetFloat(channel));
        resultString += "\nDefault Value: ";
        resultString += std::to_string(Aco_FloatChannel_GetDefaultFloat(channel));
        return resultString;
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

void write_channel_graph(A3d_ChannelGroup* group, UGraphviz::Graph* graph)
{
    auto& registry = graph->GetRegistry();

    for(int i {}; i < 50000; ++i) {
        A3d_Channel* channel = ChannelGroup_GetChannel(group, i);
        if(channel) {
            GUID channelGuid(channel->GetChannelType().guid);
            auto node = registry.RegisterNode(std::to_string(Channel_GetChannelIDIndexNr(channel)));

            std::string nodeLabel;
            nodeLabel += Channel_GetChannelName(channel);
            nodeLabel += "\\n";
            nodeLabel += channel->GetChannelType().name;
            std::string channelValue = get_channel_value(channel);
            // Backslashes need to be escaped.
            util::ReplaceAll(channelValue, "\\", "\\\\");
            // Graphviz can't cope with newlines either, so they need to be replaced with the literal "\n".
            util::ReplaceAll(channelValue, "\r\n", "\\n");
            util::ReplaceAll(channelValue, "\n", "\\n");
            // Escape quotes as well.
            util::ReplaceAll(channelValue, "\"", "\\\"");
            if(!channelValue.empty()) {
                nodeLabel += "\\nChannel value: ";
                nodeLabel += channelValue;
            }
            registry.RegisterNodeAttr(node, UGraphviz::Attrs_label, nodeLabel);
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

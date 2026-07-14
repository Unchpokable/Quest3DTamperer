#pragma once

#include "pch.h"

namespace graph
{
// Populates `graph` with one node per channel in `group` (labeled with the
// channel's name, type and, where known, its current value) and one edge per
// parent/child relationship, so it can be dumped as a DOT digraph.
void write_channel_graph(A3d_ChannelGroup* group, UGraphviz::Graph* graph);
} // namespace graph

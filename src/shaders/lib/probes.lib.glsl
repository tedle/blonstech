////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

// Globals
layout(std430) buffer probe_buffer
{
    Probe _probes[];
};

layout(std430) buffer probe_network_buffer
{
    ProbeSearchCell _probe_network[];
};

// Enums copied from blons::LightSector
#define PROBE_SEARCH_CELL_INVALID_ID -1
#define PROBE_SEARCH_CELL_FACE_123 0
#define PROBE_SEARCH_CELL_FACE_023 1
#define PROBE_SEARCH_CELL_FACE_013 2
#define PROBE_SEARCH_CELL_FACE_012 3
#define PROBE_SEARCH_CELL_EDGE_12 0
#define PROBE_SEARCH_CELL_EDGE_02 1
#define PROBE_SEARCH_CELL_EDGE_01 2
#define PROBE_SEARCH_CELL_FACE 3

Probe FindProbe(int probe_id)
{
    return _probes[probe_id];
}

int CountProbes()
{
    return _probes.length();
}

ProbeSearchCell FindProbeSearchCell(int cell_id)
{
    return _probe_network[cell_id];
}

int CountProbeSearchCells()
{
    return _probe_network.length();
}

bool IsOuterProbeSearchCell(const ProbeSearchCell cell)
{
    return cell.probe_vertices[3] == PROBE_SEARCH_CELL_INVALID_ID;
}

ProbeWeight[4] FindProbeWeights(vec3 world_pos)
{
    ProbeWeight weights[4];
    const float kMinBarycentricMargin = -0.01f;
    const float kMinPlaneDistance = abs(kMinBarycentricMargin) / 2.0f;
    const int kMaxSteps = 1024;
    int cell_id = 0;
    for (int i = 0; i < kMaxSteps; i++)
    {
        const ProbeSearchCell cell = FindProbeSearchCell(cell_id);
        vec4 barycentric_coords;
        // Barycentric coordinates are calculated by different means for outer and inner cells
        if (IsOuterProbeSearchCell(cell))
        {
            // Get all the probes we need in a single global memory read
            Probe fetched_probes[3] = {
                FindProbe(cell.probe_vertices[0]),
                FindProbe(cell.probe_vertices[1]),
                FindProbe(cell.probe_vertices[2])
            };
            // Collect the positions of the 3 probes in this outer cell
            vec3 pos[3] = vec3[3](
                vec3(fetched_probes[0].pos[0], fetched_probes[0].pos[1], fetched_probes[0].pos[2]),
                vec3(fetched_probes[1].pos[0], fetched_probes[1].pos[1], fetched_probes[1].pos[2]),
                vec3(fetched_probes[2].pos[0], fetched_probes[2].pos[1], fetched_probes[2].pos[2])
            );
            // Collect each vertex normal, pre-scaled during bake to have a distance from the hull plane equal to 1
            vec3 normal[3] = vec3[3](
                vec3(cell.barycentric_converter[0][0], cell.barycentric_converter[0][1], cell.barycentric_converter[0][2]),
                vec3(cell.barycentric_converter[1][0], cell.barycentric_converter[1][1], cell.barycentric_converter[1][2]),
                vec3(cell.barycentric_converter[2][0], cell.barycentric_converter[2][1], cell.barycentric_converter[2][2])
            );
            vec3 plane_normal = normalize(cross(pos[1] - pos[0], pos[2] - pos[0]));
            vec3 ray = world_pos - pos[0];
            float distance_to_plane = dot(plane_normal, ray);
            if (distance_to_plane < kMinPlaneDistance)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_FACE];
                continue;
            }
            // Extrude the hull face to contain our world position
            vec3 extruded_face[3] = vec3[3](
                vec3(pos[0] + (normal[0] * distance_to_plane)),
                vec3(pos[1] + (normal[1] * distance_to_plane)),
                vec3(pos[2] + (normal[2] * distance_to_plane))
            );
            // Then calculate the barycentric coordinates from the extruded triangle
            // There are only 3 probes for an outer cell, the last weight will always be 0
            barycentric_coords = vec4(TriangleBarycentric(extruded_face, plane_normal, world_pos), 0.0f);
            if (barycentric_coords.x < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_EDGE_12];
                continue;
            }
            if (barycentric_coords.y < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_EDGE_02];
                continue;
            }
            if (barycentric_coords.z < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_EDGE_01];
                continue;
            }
        }
        else
        {
            Probe fetched_probe = FindProbe(cell.probe_vertices[0]);
            vec3 fetched_pos = vec3(fetched_probe.pos[0], fetched_probe.pos[1], fetched_probe.pos[2]);
            vec3 barycentric_pos = vec3(world_pos - fetched_pos);
            mat3 converter = mat3(
                cell.barycentric_converter[0][0], cell.barycentric_converter[0][1], cell.barycentric_converter[0][2],
                cell.barycentric_converter[1][0], cell.barycentric_converter[1][1], cell.barycentric_converter[1][2],
                cell.barycentric_converter[2][0], cell.barycentric_converter[2][1], cell.barycentric_converter[2][2]
            );
            barycentric_coords.xyz = converter * barycentric_pos;
            barycentric_coords = vec4(1.0f - barycentric_coords.x - barycentric_coords.y - barycentric_coords.z, barycentric_coords.xyz);
            if (barycentric_coords.x < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_FACE_123];
                continue;
            }
            if (barycentric_coords.y < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_FACE_023];
                continue;
            }
            if (barycentric_coords.z < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_FACE_013];
                continue;
            }
            if (barycentric_coords.w < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[PROBE_SEARCH_CELL_FACE_012];
                continue;
            }
        }
        weights[0].id = cell.probe_vertices[0];
        weights[0].weight = max(barycentric_coords.x, 0.0f);
        weights[1].id = cell.probe_vertices[1];
        weights[1].weight = max(barycentric_coords.y, 0.0f);
        weights[2].id = cell.probe_vertices[2];
        weights[2].weight = max(barycentric_coords.z, 0.0f);
        weights[3].id = IsOuterProbeSearchCell(cell) ? 0 : cell.probe_vertices[3];
        weights[3].weight = max(barycentric_coords.w, 0.0f);
        return weights;
    }
    return weights;
}
# The Windows 10/11 SDK still ships d3d9.h, but not the D3DX9 utility headers
# (d3dx9.h, d3dx9math.h, ...) that the Quest3D SDK headers need for
# D3DXVECTOR3/D3DXMATRIX. We pull every DirectX 9 header from the vendored
# June 2010 DirectX SDK instead of mixing it with the Windows SDK's copies,
# to avoid the well-known header conflicts between the two.
#
# Nothing here is linked: no D3DX9 *function* is ever called by this project
# (D3DXVECTOR3/D3DXMATRIX construction and operators are inlined from
# d3dx9math.inl), so the DirectX-SDK-June2010/Lib import libraries are
# intentionally unused.

add_library(dxsdk INTERFACE)
target_include_directories(dxsdk INTERFACE "${CMAKE_SOURCE_DIR}/DirectX-SDK-June2010/Include")

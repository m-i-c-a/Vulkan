#include "Renderable.hpp"

#include <iostream>

void Renderable::render(VkCommandBuffer commandBuffer) const
{
    // bind obj data

    static VkDeviceSize pOffsets = 0;
    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.getBuffer(), 0u, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(commandBuffer, 0u, 1u, m_VertexBuffer.getBufferPointer(), &pOffsets);
    vkCmdDrawIndexed(commandBuffer, indexCount, 1u, 0u, vertexOffset, 0u);
}
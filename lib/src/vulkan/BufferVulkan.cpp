/*
* Viry3D
* Copyright 2014-2018 by Stack - stackos@qq.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "BufferVulkan.h"
#include "DisplayVulkan.h"
#include "graphics/Graphics.h"
#include "memory/Memory.h"

namespace Viry3D
{
	BufferVulkan::BufferVulkan():
		m_size(0),
		m_type(BufferType::None),
		m_buffer(VK_NULL_HANDLE),
		m_memory(VK_NULL_HANDLE)
	{
	}

	BufferVulkan::~BufferVulkan()
	{
		auto display = (DisplayVulkan*) Graphics::GetDisplay();
		auto device = display->GetDevice();

		vkDeviceWaitIdle(device);

		vkFreeMemory(device, m_memory, NULL);
		vkDestroyBuffer(device, m_buffer, NULL);
	}

	void BufferVulkan::CreateInternal(BufferType type, bool dynamic)
	{
		auto display = (DisplayVulkan*) Graphics::GetDisplay();
		auto device = display->GetDevice();
		VkResult err;

		if (m_buffer == VK_NULL_HANDLE)
		{
			VkBufferCreateInfo buf_info;
			Memory::Zero(&buf_info, sizeof(buf_info));
			buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buf_info.pNext = NULL;
			buf_info.size = (uint32_t) m_size;
			buf_info.flags = 0;

			m_type = type;
			switch (type)
			{
				case BufferType::Vertex:
				{
					buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
					break;
				}
				case BufferType::Index:
				{
					buf_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
					break;
				}
				case BufferType::Uniform:
				{
					buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
					break;
				}
				case BufferType::Image:
				{
					buf_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
					break;
				}
				case BufferType::None:
					break;
			}

			err = vkCreateBuffer(device, &buf_info, NULL, &m_buffer);
			assert(!err);
		}

		if (m_memory == VK_NULL_HANDLE)
		{
			VkMemoryRequirements mem_reqs;
			vkGetBufferMemoryRequirements(device, m_buffer, &mem_reqs);

			VkMemoryAllocateInfo mem_alloc = {
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				NULL,
				mem_reqs.size,
				0,
			};
			bool pass = display->CheckMemoryType(
				mem_reqs.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&mem_alloc.memoryTypeIndex);
			assert(pass);

			err = vkAllocateMemory(device, &mem_alloc, NULL, &m_memory);
			assert(!err);

			err = vkBindBufferMemory(device, m_buffer, m_memory, 0);
			assert(!err);
		}
	}

	void BufferVulkan::Fill(void* param, FillFunc fill)
	{
		auto device = ((DisplayVulkan*) Graphics::GetDisplay())->GetDevice();

		void* data;
		VkResult err = vkMapMemory(device, m_memory, 0, (uint32_t) m_size, 0, &data);
		assert(!err);

		ByteBuffer buffer((byte*) data, m_size);
		fill(param, buffer);

		vkUnmapMemory(device, m_memory);
	}

	void BufferVulkan::UpdateRange(int offset, int size, const void* data)
	{
		auto device = ((DisplayVulkan*) Graphics::GetDisplay())->GetDevice();

		void* mapped;
		VkResult err = vkMapMemory(device, m_memory, (uint32_t) offset, (uint32_t) size, 0, &mapped);
		assert(!err);

		Memory::Copy(mapped, data, size);

		vkUnmapMemory(device, m_memory);
	}
}

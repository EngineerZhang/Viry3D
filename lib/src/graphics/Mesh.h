/*
* Viry3D
* Copyright 2014-2017 by Stack - stackos@qq.com
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

#pragma once

#include "Object.h"
#include "Color.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"

namespace Viry3D
{
	class GameObject;

	class Mesh: public Object
	{
	public:
		static Ref<Mesh> Create(bool dynamic = false);

		void Update();
		const Ref<VertexBuffer>& GetVertexBuffer() const { return m_vertex_buffer; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_index_buffer; }
		void GetIndexRange(int submesh_index, int& start, int& count);
		int GetSubmeshCount() const;
		bool IsDynamic() const { return m_dynamic; }

		Vector<Vector3> vertices;
		Vector<Vector2> uv;				//Texture
		Vector<Color> colors;			//UI
		Vector<Vector2> uv2;			//Lightmap
		Vector<Vector3> normals;		//Light
		Vector<Vector4> tangents;		//NormalMap
		Vector<Vector4> bone_weights;
		Vector<Vector4> bone_indices;	//Skinned

		struct Submesh
		{
			int start;
			int count;
		};

		struct BlendShapeVertexDelta
		{
			Vector3 vertex;
			Vector3 normal;
			Vector3 tangent;
		};

		struct BlendShapeFrame
		{
			float weight;
			Vector<BlendShapeVertexDelta> deltas;
		};

		struct BlendShape
		{
			String name;
			Vector<BlendShapeFrame> frames;
			float weight;

			BlendShape():
				weight(0)
			{
			}
		};

		Vector<unsigned short> triangles;
		Vector<Submesh> submeshes;
		Vector<Matrix4x4> bind_poses;
		Vector<BlendShape> blend_shapes;

	private:
		static void FillVertexBuffer(void* param, const ByteBuffer& buffer);
		static void FillIndexBuffer(void* param, const ByteBuffer& buffer);

		Mesh();
		void UpdateVertexBuffer();
		void UpdateIndexBuffer();
		int VertexBufferSize() const;
		int IndexBufferSize() const;

		bool m_dynamic;
		Ref<VertexBuffer> m_vertex_buffer;
		Ref<IndexBuffer> m_index_buffer;
	};
}

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

#include "Component.h"
#include "GameObject.h"
#include "Action.h"
#include "animation/AnimationCurve.h"

namespace Viry3D
{
	enum class TweenerPlayStyle
	{
		Once,
		Loop,
		PingPong,
	};

	class Tweener: public Component
	{
		DECLARE_COM_CLASS_ABSTRACT(Tweener, Component);
	protected:
		Tweener();
		virtual ~Tweener();
		virtual void Update();
		virtual void OnSetValue(float value) = 0;

	public:
		AnimationCurve curve;
		float duration;
		float delay;
		TweenerPlayStyle play_style;
		Action on_finish;

	protected:
		float m_time_start;
		float m_time;
		bool m_reverse;
		bool m_finish;
	};
}

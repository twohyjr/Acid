#pragma once

#include <string>
#include "Maths/Matrix4.hpp"

namespace acid
{
	class FL_EXPORT JointTransformData
	{
	private:
		std::string m_jointNameId;
		Matrix4 m_jointLocalTransform;
	public:
		JointTransformData(const std::string &jointNameId, const Matrix4 &jointLocalTransform);

		~JointTransformData();

		std::string GetJointNameId() const { return m_jointNameId; }

		Matrix4 GetJointLocalTransform() const { return m_jointLocalTransform; }
	};
}

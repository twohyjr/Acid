#include "Post/PostFilter.hpp"

namespace acid {
uint32_t PostFilter::GlobalSwitching = 0;

PostFilter::PostFilter(const Pipeline::Stage &pipelineStage, const std::vector<std::filesystem::path> &shaderStages, const std::vector<Shader::Define> &defines) :
	Subrender(pipelineStage),
	m_pipeline(pipelineStage, shaderStages, {}, defines, PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::None) {
}

const Descriptor *PostFilter::GetAttachment(const std::string &descriptorName, const Descriptor *descriptor) const {
	auto it = m_attachments.find(descriptorName);

	if (it == m_attachments.end()) {
		return descriptor;
	}

	return it->second;
}

const Descriptor *PostFilter::GetAttachment(const std::string &descriptorName, const std::string &rendererAttachment) const {
	auto it = m_attachments.find(descriptorName);

	if (it == m_attachments.end()) {
		return Graphics::Get()->GetAttachment(rendererAttachment);
	}

	return it->second;
}

void PostFilter::SetAttachment(const std::string &descriptorName, const Descriptor *descriptor) {
	auto it = m_attachments.find(descriptorName);

	if (it == m_attachments.end()) {
		m_attachments.emplace(descriptorName, descriptor);
		return;
	}

	it->second = descriptor;
}

bool PostFilter::RemoveAttachment(const std::string &name) {
	auto it = m_attachments.find(name);

	if (it != m_attachments.end()) {
		m_attachments.erase(it);
		return true;
	}

	return false;
}

void PostFilter::PushConditional(const std::string &descriptorName1, const std::string &descriptorName2, const std::string &rendererAttachment1,
	const std::string &rendererAttachment2) {
	// TODO: Clean up this state machine mess, this logic may also be incorrect.
	auto it1 = m_attachments.find(descriptorName1);
	auto it2 = m_attachments.find(descriptorName2);

	if (it1 != m_attachments.end() && it2 != m_attachments.end()) {
		m_descriptorSet.Push(descriptorName1, GetAttachment(descriptorName1, rendererAttachment1));
		m_descriptorSet.Push(descriptorName2, GetAttachment(descriptorName2, rendererAttachment1));
		return;
	}
	if (it1 == m_attachments.end() && it2 != m_attachments.end()) {
		m_descriptorSet.Push(descriptorName1, Graphics::Get()->GetAttachment(GlobalSwitching % 2 == 1 ? rendererAttachment1 : rendererAttachment2));
		m_descriptorSet.Push(descriptorName2, GetAttachment(descriptorName2, rendererAttachment1));
		return;
	}
	if (it1 != m_attachments.end() && it2 == m_attachments.end()) {
		m_descriptorSet.Push(descriptorName1, GetAttachment(descriptorName1, rendererAttachment1));
		m_descriptorSet.Push(descriptorName2, Graphics::Get()->GetAttachment(GlobalSwitching % 2 == 1 ? rendererAttachment1 : rendererAttachment2));
		return;
	}

	if (GlobalSwitching % 2 == 1) {
		m_descriptorSet.Push(descriptorName1, Graphics::Get()->GetAttachment(rendererAttachment1));
		m_descriptorSet.Push(descriptorName2, Graphics::Get()->GetAttachment(rendererAttachment2));
	} else {
		m_descriptorSet.Push(descriptorName1, Graphics::Get()->GetAttachment(rendererAttachment2));
		m_descriptorSet.Push(descriptorName2, Graphics::Get()->GetAttachment(rendererAttachment1));
	}

	GlobalSwitching++;
}
}

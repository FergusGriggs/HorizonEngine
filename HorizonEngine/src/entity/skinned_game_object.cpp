#include "skinned_game_object.h"

#include "../graphics/data/skinned_model.h"

namespace hrzn::entity
{
    SkinnedGameObject::SkinnedGameObject()
    {
        m_type = GameObject::Type::eSkinned;
    }

    SkinnedGameObject::~SkinnedGameObject()
    {
    }

    bool SkinnedGameObject::initialise(std::string label, const std::string& filePath)
    {
        m_label = label;

        m_model = gfx::ResourceManager::it().getSkinnedModelPtr(filePath);

        if (m_model == nullptr)
        {
            return false;
        }

        return true;
    }

    void SkinnedGameObject::update(float deltaTime)
    {
        
        XMMATRIX transformMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_transform.getRotationMatrix() * XMMatrixTranslation(m_transform.getPositionFloat3().x, m_transform.getPositionFloat3().y, m_transform.getPositionFloat3().z);
        if (auto* skinnedModel = dynamic_cast<gfx::SkinnedModel*>(m_model))
        {
            skinnedModel->update(transformMatrix, deltaTime);
        };
    }
}

#pragma once

#include "renderable_game_object.h"

namespace hrzn::entity
{
    class SkinnedGameObject : public RenderableGameObject
    {
    public:
        SkinnedGameObject();
        ~SkinnedGameObject();

        virtual bool initialise(std::string label, const std::string& filePath) override;
        virtual void update(float deltaTime) override;
    };
}

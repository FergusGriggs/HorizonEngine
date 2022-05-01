#pragma once

#include "renderable_game_object.h"

/***********************************************
* 
MARKING SCHEME: System Architecture and code style 
COMMENT INDEX: 0
DESCRIPTION: How skinned objects are represented in the scene, inheriting from 
             RenderableGameObject means that very little additional work was required
             in the base engine code to allow for skinned models to be used

***********************************************/

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

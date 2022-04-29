#include "skinned_model.h"

#include "../data/vertex_types.h"

namespace hrzn::gfx
{
    // Bone Keyframe
    BoneKeyframe::BoneKeyframe() :
        m_time(0.0f),
        m_translation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f),
        m_orientationQuaternion(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    BoneKeyframe::~BoneKeyframe()
    {
    }

    // Bone Animation
    float BoneAnimation::getStartTime() const
    {
        if (m_keyframes.empty())
        {
            return 0.0f;
        }

        return m_keyframes.front().m_time;
    }

    float BoneAnimation::getEndTime() const
    {
        if (m_keyframes.empty())
        {
            return 0.0f;
        }

        return m_keyframes.back().m_time;
    }

    void BoneAnimation::getTransformAtTime(XMFLOAT4X4& M, float time) const
    {
        if (m_keyframes.empty())
        {
            return XMStoreFloat4x4(&M, XMMatrixIdentity());
        }

        // If before the first frame
        if (time <= m_keyframes.front().m_time)
        {
            XMVECTOR scale =       XMLoadFloat3(&(m_keyframes.front().m_scale));
            XMVECTOR zero =        XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
            XMVECTOR orientation = XMLoadFloat4(&(m_keyframes.front().m_orientationQuaternion));
            XMVECTOR translation = XMLoadFloat3(&(m_keyframes.front().m_translation));
            
            XMStoreFloat4x4(&M, XMMatrixAffineTransformation(scale, zero, orientation, translation));
        }
        // Or after the last
        else if (time >= m_keyframes.back().m_time)
        {
            XMVECTOR scale =       XMLoadFloat3(&(m_keyframes.back().m_scale));
            XMVECTOR zero =        XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
            XMVECTOR orientation = XMLoadFloat4(&(m_keyframes.back().m_orientationQuaternion));
            XMVECTOR translation = XMLoadFloat3(&(m_keyframes.back().m_translation));
            
            XMStoreFloat4x4(&M, XMMatrixAffineTransformation(scale, zero, orientation, translation));
        }
        else
        {
            for (UINT i = 0; i < m_keyframes.size() - 1; ++i)
            {
                if (time >= m_keyframes[i].m_time && time <= m_keyframes[i + 1].m_time)
                {
                    float progress = (time - m_keyframes[i].m_time) / (m_keyframes[i + 1].m_time - m_keyframes[i].m_time);

                    XMVECTOR scaleCurrent = XMLoadFloat3(&(m_keyframes[i].m_scale));
                    XMVECTOR scaleScaleNext = XMLoadFloat3(&(m_keyframes[i + 1].m_scale));
                    XMVECTOR lerpedScale = XMVectorLerp(scaleCurrent, scaleScaleNext, progress);

                    XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

                    XMVECTOR orientationCurrent = XMLoadFloat4(&(m_keyframes[i].m_orientationQuaternion));
                    XMVECTOR orientationNext = XMLoadFloat4(&(m_keyframes[i + 1].m_orientationQuaternion));
                    XMVECTOR orientationSlerped = XMQuaternionSlerp(orientationCurrent, orientationNext, progress);

                    XMVECTOR translationCurrent = XMLoadFloat3(&(m_keyframes[i].m_translation));
                    XMVECTOR translationNext = XMLoadFloat3(&(m_keyframes[i + 1].m_translation));
                    XMVECTOR translationLerped = XMVectorLerp(translationCurrent, translationNext, progress);
                    
                    XMStoreFloat4x4(&M, XMMatrixAffineTransformation(lerpedScale, zero, orientationSlerped, translationLerped));
                    break;
                }
            }
        }
    }

    // Skeleton Animation
    float SkeletonAnimation::getStartTime() const
    {
        return m_startTime;
    }

    float SkeletonAnimation::getEndTime() const
    {
        return m_endTime;
    }

    void SkeletonAnimation::calcualteStartAndEndTime()
    {
        // Start time
        float earliestStartTime = FLT_MAX;
        for (int i = 0; i < m_boneAnimations.size(); ++i)
        {
            earliestStartTime = fminf(earliestStartTime, m_boneAnimations[i].getStartTime());
        }
        m_startTime = earliestStartTime;

        // End time
        float latestEndTime = 0.0f;
        for (int i = 0; i < m_boneAnimations.size(); ++i)
        {
            latestEndTime = fmaxf(latestEndTime, m_boneAnimations[i].getEndTime());
        }
        m_endTime = latestEndTime;
    }

    void SkeletonAnimation::getBoneTransformsAtTime(std::vector<XMFLOAT4X4>& boneTransforms, float time) const
    {
        for (int i = 0; i < m_boneAnimations.size(); ++i)
        {
            m_boneAnimations[i].getTransformAtTime(boneTransforms[i], time);
        }
    }

    // Skinned Model
    SkinnedModel::SkinnedModel() :
        Model()
    {
    }

    SkinnedModel::~SkinnedModel()
    {
    }

    bool SkinnedModel::initialise(const std::string& filePath)
    {
        m_filePath = filePath;

        try
        {
            if (!loadModel<gfx::SkinnedVertex>(filePath))
            {
                return false;
            }
        }
        catch (utils::COMException& exception)
        {
            utils::ErrorLogger::log(exception);
            return false;
        }

        return true;
    }

    void SkinnedModel::setSkinningData(std::vector<int>& boneHierarchy, std::vector<XMFLOAT4X4>& boneOffsets, std::map<std::string, SkeletonAnimation>& animations)
    {
        m_boneHierarchy = boneHierarchy;
        m_boneOffsets = boneOffsets;
        m_animations = animations;
    }

    void SkinnedModel::setCurrentAnimation(const std::string& animationName)
    {
        auto animationIter = m_animations.find(animationName);
        if (animationIter != m_animations.end())
        {
            m_currentAnimationName = animationName;
            m_currentAnimation = &(animationIter->second);
        }
    }

    void SkinnedModel::setCurrentAnimationLooping(bool looping)
    {
        m_currentAnimationLooping = looping;
    }

    bool SkinnedModel::isCurrentAnimationLooping() const
    {
        return m_currentAnimationLooping;
    }

    const std::string& SkinnedModel::getCurrentAnimationName() const
    {
        return m_currentAnimationName;
    }

    const SkeletonAnimation* SkinnedModel::getCurrentAnimation() const
    {
        // Can be nullptr (allowed)
        return m_currentAnimation;
    }

    const SkeletonAnimation* SkinnedModel::getAnimation(const std::string& animationName) const
    {
        auto animationIter = m_animations.find(animationName);
        if (animationIter != m_animations.end())
        {
            return &(animationIter->second);
        }

        return nullptr;
    }

    UINT SkinnedModel::getNumBones() const
    {
        return m_boneHierarchy.size();
    }

    void SkinnedModel::update(float deltaTime)
    {
        if (m_currentAnimation != nullptr)
        {
            m_currentAnimationTime += deltaTime;

            if (m_currentAnimationTime > m_currentAnimation->getEndTime())
            {
                m_currentAnimationTime -= m_currentAnimation->getEndTime();
            }

            updateBoneTransforms();
        }
    }

    void SkinnedModel::updateBoneTransforms()
    {
        UINT numBones = m_boneOffsets.size();

        // If we have an animation, use it
        if (m_currentAnimation != nullptr)
        {
            std::vector<XMFLOAT4X4> toParentTransforms(numBones);

            // Get the transforms of all the bones in the current animation at the given time
            m_currentAnimation->getBoneTransformsAtTime(toParentTransforms, m_currentAnimationTime);

            // Traverse the hierarchy and transform all the bones to the root space
            std::vector<XMFLOAT4X4> toRootTransforms(numBones);

            // The root bone has index 0.  The root bone has no parent, so its toRootTransform is just its local bone transform
            toRootTransforms[0] = toParentTransforms[0];

            // Now find the toRootTransform of the children
            for (UINT i = 1; i < numBones; ++i)
            {
                XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

                int parentIndex = m_boneHierarchy[i];
                XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

                XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

                XMStoreFloat4x4(&toRootTransforms[i], toRoot);
            }

            // Premultiply by the bone offset transform to get the final transform.
            for (UINT i = 0; i < numBones; ++i)
            {
                XMMATRIX offset = XMLoadFloat4x4(&(m_boneOffsets[i]));
                XMMATRIX toRoot = XMLoadFloat4x4(&(toRootTransforms[i]));
                XMStoreFloat4x4(&m_boneTransforms[i], XMMatrixMultiply(offset, toRoot));
            }
        }
        else
        {
            // No animation, return to default pose
            for (UINT i = 0; i < numBones; ++i)
            {
                XMStoreFloat4x4(&m_boneTransforms[i], XMMatrixIdentity());
            }
        }
    }

    void SkinnedModel::postSceneBasicParse(const aiScene* scene)
    {
        for (int i = 0; i < scene->mNumMeshes; ++i)
        {

        }
    }
}
#pragma once

#include "model.h"

#include <map>

namespace hrzn::gfx
{
	struct BoneKeyframe
	{
		BoneKeyframe();
		~BoneKeyframe();

		float    m_time;
		XMFLOAT3 m_translation;
		XMFLOAT3 m_scale;
		XMFLOAT4 m_orientationQuaternion;
	};

	struct BoneAnimation
	{
		float getStartTime() const;
		float getEndTime() const;

		void getTransformAtTime(XMFLOAT4X4& M, float time) const;

		std::vector<BoneKeyframe> m_keyframes;
	};

	struct SkeletonAnimation
	{
		float getStartTime() const;
		float getEndTime() const;

		void calcualteStartAndEndTime();

		void getBoneTransformsAtTime(std::vector<XMFLOAT4X4>& boneTransforms, float time) const;

		std::vector<BoneAnimation> m_boneAnimations;

		float                      m_startTime;
		float                      m_endTime;
	};

    class SkinnedModel : public Model
    {
	public:
		SkinnedModel();
		~SkinnedModel();

		bool                     initialise(const std::string& filePath);

		void                     setSkinningData(std::vector<int>& boneHierarchy, std::vector<XMFLOAT4X4>& boneOffsets, std::map<std::string, SkeletonAnimation>& animations);
		
		void                     setCurrentAnimation(const std::string& animationName);
		void                     setCurrentAnimationLooping(bool looping);
		bool                     isCurrentAnimationLooping() const;

		const std::string&       getCurrentAnimationName() const;
		const SkeletonAnimation* getCurrentAnimation() const;
		const SkeletonAnimation* getAnimation(const std::string& animationName) const;

		UINT getNumBones() const;

		void update(float deltaTime);

		void updateBoneTransforms();

	protected:
		virtual void postSceneBasicParse(const aiScene* scene) override;

	private:
		std::vector<XMFLOAT4X4>  m_boneTransforms;
		std::vector<int>         m_boneHierarchy;
		std::vector<XMFLOAT4X4>  m_boneOffsets;

		std::string        m_currentAnimationName;
		SkeletonAnimation* m_currentAnimation;
		float              m_currentAnimationTime;
		bool               m_currentAnimationLooping;

		std::map<std::string, SkeletonAnimation> m_animations;
    };
}

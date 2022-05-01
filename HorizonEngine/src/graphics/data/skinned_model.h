#pragma once

#include "model.h"
#include "vertex_types.h"

#include <map>

/***********************************************

MARKING SCHEME: Human skeletal structure + Walking animation + Loading a humanoid character
COMMENT INDEX: 9
DESCRIPTION: skinned_model.h and skinned_model.cpp is where you can find all of the code
             used to load, process and ultimately render skinned (mostly humanoid) characters

***********************************************/

#define MAX_BONES_PER_VERTEX 4

namespace hrzn::gfx
{
	struct Bone
	{
		static XMFLOAT4X4&& convertAIMatToXMFloat4x4(const aiMatrix4x4& aiMatrix)
		{
			return XMFLOAT4X4(
				aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1,
				aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2,
				aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3,
				aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4);
		}

		Bone(int boneId, const aiBone* bone) :
			m_id(boneId),
			m_name(bone->mName.C_Str()),

			m_parent(nullptr),
			m_children()
		{
			m_offsetMatrix = convertAIMatToXMFloat4x4(bone->mOffsetMatrix);

			m_translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
			m_orientationQuaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		Bone(int boneId, const std::string& name) :
			m_id(boneId),
			m_name(name),

			m_parent(nullptr),
			m_children()
		{
			XMStoreFloat4x4(&m_offsetMatrix, XMMatrixIdentity());

			m_translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
			m_orientationQuaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		int                m_id;
		std::string        m_name;

		Bone*              m_parent;
		std::vector<Bone*> m_children;

		XMFLOAT4X4         m_offsetMatrix;

		XMFLOAT3           m_translation;
		XMFLOAT3           m_scale;
		XMFLOAT4           m_orientationQuaternion;
	};

	struct VertexBoneData
	{
		int   m_boneIds[MAX_BONES_PER_VERTEX] = { 0 };
		float m_weights[MAX_BONES_PER_VERTEX] = { 0.0f };

		void addBoneData(int boneId, float weight)
		{
			for (int i = 0; i < MAX_BONES_PER_VERTEX; ++i)
			{
				if (m_weights[i] == 0.0f)
				{
					m_boneIds[i] = boneId;
					m_weights[i] = weight;
					return;
				}
			}
		}
	};

	struct BoneTranslationKeyframe
	{
		float    m_time;
		XMFLOAT3 m_translation;
	};

	struct BoneScaleKeyframe
	{
		float    m_time;
		XMFLOAT3 m_scale;
	};

	struct BoneRotationKeyframe
	{
		float    m_time;
		XMFLOAT4 m_orientationQuaternion;
	};

	struct BoneAnimation
	{
		float getStartTime() const;
		float getEndTime() const;

		void getTransformAtTime(XMFLOAT4X4& M, float timeTicks) const;

		void getTranslationAtTime(XMFLOAT3& translation, float timeTicks) const;
		void getScaleAtTime(XMFLOAT3& scale, float timeTicks) const;
		void getRotationAtTime(XMFLOAT4& rotation, float timeTicks) const;

		void calculateStartAndEndTimes();

		std::vector<BoneTranslationKeyframe> m_translationKeyframes;
		std::vector<BoneScaleKeyframe>       m_scaleKeyframes;
		std::vector<BoneRotationKeyframe>    m_rotationKeyframes;

	private:
		float                                m_startTime;
		float                                m_endTime;
	};

	struct SkeletonAnimation
	{
	public:
		float getStartTime() const;
		float getEndTime() const;

		void calculateStartAndEndTimes();

		void getBoneTransformsAtTime(std::vector<XMFLOAT4X4>& boneTransforms, float timeTicks) const;

		void getBoneTranslationAtTime(int boneIndex, XMFLOAT3& translation, float timeTicks);
		void getBoneScaleAtTime(int boneIndex, XMFLOAT3& scale, float timeTicks);
		void getBoneOrientationAtTime(int boneIndex, XMFLOAT4& orientation, float timeTicks);

		std::vector<BoneAnimation> m_boneAnimations;
		float                      m_ticksPerSecond;

	private:
		float                      m_startTime;
		float                      m_endTime;
	};

    class SkinnedModel : public Model
    {
	public:
		SkinnedModel();
		~SkinnedModel();

		bool                     initialise(const std::string& filePath);
		virtual void             updateAlternatePerObjectCB() const override;

		// Terrible solution and I hate it but you cant have inheritance and templates
		bool                     loadSkinnedModel(const std::string& filePath);
		void                     processSkinnedNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
		Mesh*                    processSkinnedMesh(aiMesh* mesh, Material* material, const XMMATRIX& transformMatrix);
		void                     fillVertexSkinningData(int vertexIndex, gfx::SkinnedVertex& vertex);

		void                     setCurrentAnimation(const std::string& animationName);
		void                     setCurrentAnimationLooping(bool looping);
		bool                     isCurrentAnimationLooping() const;

		const std::string&       getCurrentAnimationName() const;
		float*                   getCurrentAnimationTimePtr();
		const SkeletonAnimation* getCurrentAnimation() const;
		const SkeletonAnimation* getAnimation(const std::string& animationName) const;

		const Bone*                                     getRootBone() const;
		std::map<std::string, SkeletonAnimation>&       getAnimations();

		UINT getNumBones() const;

		void update(const XMMATRIX& modelMatrix, float deltaTime);
		virtual void debugDraw(const XMMATRIX& transformMatrix, bool bindPSData = true) const override;

		void updateBoneTransforms(const XMMATRIX& modelMatrix);

	protected:
		virtual void preSceneParse(const aiScene* scene) override;
		virtual void preMeshProcessed(int meshIndex, const aiMesh* mesh) override;

		void generateAssimpBoneMap(const aiScene* scene);
		void recursiveGenerateBoneHierarchy(Bone* bone, const aiNode* assimpNode, int& latestId);
		void recursiveApplyBoneTransform(Bone* bone, XMMATRIX parentTransform);
		void loadAnimations(const aiScene* scene);

		void parseMeshBones(int meshIndex, const aiMesh* mesh);
		void parseSingleBone(int meshIndex, const aiBone* bone);
		int  getBoneId(const aiBone* bone);

	private:
		std::vector<XMFLOAT4X4> m_boneTransforms;

		std::string        m_currentAnimationName;
		SkeletonAnimation* m_currentAnimation;
		float              m_currentAnimationTimeTicks;
		bool               m_currentAnimationLooping;

		std::map<std::string, SkeletonAnimation> m_animations;

		std::vector<VertexBoneData>    m_vertexBoneData;
		std::map<std::string, Bone*>   m_boneDataMap;
		Bone*                          m_rootBone;
		const aiNode*                  m_rootAssimpBoneNode;

		std::map<std::string, aiBone*> m_assimpBoneMap;
    };
}

#include "skinned_model.h"

#include "../data/vertex_types.h"
#include "../graphics_handler.h"

namespace hrzn::gfx
{
    // Bone Keyframe

    // Bone Animation

    float BoneAnimation::getStartTime() const
    {
        return m_startTime;
    }

    float BoneAnimation::getEndTime() const
    {
        return m_endTime;
    }

    void BoneAnimation::getTransformAtTime(XMFLOAT4X4& transformationMatrix, float timeTicks) const
    {
        XMFLOAT3 scale;
        getScaleAtTime(scale, timeTicks);

        XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        XMFLOAT4 orientation;
        getRotationAtTime(orientation, timeTicks);

        XMFLOAT3 translation;
        getTranslationAtTime(translation, timeTicks);

        XMStoreFloat4x4(&transformationMatrix, XMMatrixAffineTransformation(XMLoadFloat3(&scale), zero, XMLoadFloat4(&orientation), XMLoadFloat3(&translation)));
    }

    void BoneAnimation::getTranslationAtTime(XMFLOAT3& translation, float timeTicks) const
    {
        if (m_translationKeyframes.empty())
        {
            translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
            return;
        }

        // If before the first frame
        if (timeTicks <= m_translationKeyframes.front().m_time)
        {
            translation = m_translationKeyframes.front().m_translation;
        }
        // Or after the last
        else if (timeTicks >= m_translationKeyframes.back().m_time)
        {
            translation = m_translationKeyframes.back().m_translation;
        }
        else
        {
            for (UINT i = 0; i < m_translationKeyframes.size() - 1; ++i)
            {
                if (timeTicks >= m_translationKeyframes[i].m_time && timeTicks <= m_translationKeyframes[i + 1].m_time)
                {
                    float progress = (timeTicks - m_translationKeyframes[i].m_time) / (m_translationKeyframes[i + 1].m_time - m_translationKeyframes[i].m_time);

                    XMVECTOR translationCurrent = XMLoadFloat3(&(m_translationKeyframes[i].m_translation));
                    XMVECTOR translationNext = XMLoadFloat3(&(m_translationKeyframes[i + 1].m_translation));
                    XMVECTOR translationLerped = XMVectorLerp(translationCurrent, translationNext, progress);

                    XMStoreFloat3(&translation, translationLerped);
                    break;
                }
            }
        }
    }

    void BoneAnimation::getScaleAtTime(XMFLOAT3& scale, float timeTicks) const
    {
        if (m_scaleKeyframes.empty())
        {
            scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
            return;
        }

        // If before the first frame
        if (timeTicks <= m_scaleKeyframes.front().m_time)
        {
            scale = m_scaleKeyframes.front().m_scale;
        }
        // Or after the last
        else if (timeTicks >= m_scaleKeyframes.back().m_time)
        {
            scale = m_scaleKeyframes.back().m_scale;
        }
        else
        {
            for (UINT i = 0; i < m_scaleKeyframes.size() - 1; ++i)
            {
                if (timeTicks >= m_scaleKeyframes[i].m_time && timeTicks <= m_scaleKeyframes[i + 1].m_time)
                {
                    float progress = (timeTicks - m_scaleKeyframes[i].m_time) / (m_scaleKeyframes[i + 1].m_time - m_scaleKeyframes[i].m_time);

                    XMVECTOR scaleCurrent = XMLoadFloat3(&(m_scaleKeyframes[i].m_scale));
                    XMVECTOR scaleScaleNext = XMLoadFloat3(&(m_scaleKeyframes[i + 1].m_scale));
                    XMVECTOR lerpedScale = XMVectorLerp(scaleCurrent, scaleScaleNext, progress);

                    XMStoreFloat3(&scale, lerpedScale);
                    break;
                }
            }
        }
    }

    void BoneAnimation::getRotationAtTime(XMFLOAT4& rotation, float timeTicks) const
    {
        if (m_rotationKeyframes.empty())
        {
            rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            return;
        }

        // If before the first frame
        if (timeTicks <= m_rotationKeyframes.front().m_time)
        {
            rotation = m_rotationKeyframes.front().m_orientationQuaternion;
        }
        // Or after the last
        else if (timeTicks >= m_rotationKeyframes.back().m_time)
        {
            rotation = m_rotationKeyframes.back().m_orientationQuaternion;
        }
        else
        {
            for (UINT i = 0; i < m_rotationKeyframes.size() - 1; ++i)
            {
                if (timeTicks >= m_rotationKeyframes[i].m_time && timeTicks <= m_rotationKeyframes[i + 1].m_time)
                {
                    float progress = (timeTicks - m_rotationKeyframes[i].m_time) / (m_rotationKeyframes[i + 1].m_time - m_rotationKeyframes[i].m_time);

                    XMVECTOR orientationCurrent = XMLoadFloat4(&(m_rotationKeyframes[i].m_orientationQuaternion));
                    XMVECTOR orientationNext = XMLoadFloat4(&(m_rotationKeyframes[i + 1].m_orientationQuaternion));
                    XMVECTOR orientationSlerped = XMQuaternionSlerp(orientationCurrent, orientationNext, progress);

                    XMStoreFloat4(&rotation, orientationSlerped);
                    break;
                }
            }
        }
    }

    void BoneAnimation::calculateStartAndEndTimes()
    {
        // Start time
        float earliestStartTime = FLT_MAX;
        if (!m_translationKeyframes.empty())
        {
            earliestStartTime = fminf(earliestStartTime, m_translationKeyframes.front().m_time);
        }
        if (!m_scaleKeyframes.empty())
        {
            earliestStartTime = fminf(earliestStartTime, m_scaleKeyframes.front().m_time);
        }
        if (!m_rotationKeyframes.empty())
        {
            earliestStartTime = fminf(earliestStartTime, m_rotationKeyframes.front().m_time);
        }
        m_startTime = earliestStartTime;

        // End time
        float latestEndTime = 0.0f;
        if (!m_translationKeyframes.empty())
        {
            latestEndTime = fmaxf(earliestStartTime, m_translationKeyframes.back().m_time);
        }
        if (!m_scaleKeyframes.empty())
        {
            latestEndTime = fmaxf(earliestStartTime, m_scaleKeyframes.back().m_time);
        }
        if (!m_rotationKeyframes.empty())
        {
            latestEndTime = fmaxf(earliestStartTime, m_rotationKeyframes.back().m_time);
        }
        m_endTime = latestEndTime;
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

    void SkeletonAnimation::calculateStartAndEndTimes()
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

    void SkeletonAnimation::getBoneTranslationAtTime(int boneIndex, XMFLOAT3& translation, float timeTicks)
    {
        m_boneAnimations[boneIndex].getTranslationAtTime(translation, timeTicks);
    }

    void SkeletonAnimation::getBoneScaleAtTime(int boneIndex, XMFLOAT3& scale, float timeTicks)
    {
        m_boneAnimations[boneIndex].getScaleAtTime(scale, timeTicks);
    }

    void SkeletonAnimation::getBoneOrientationAtTime(int boneIndex, XMFLOAT4& orientation, float timeTicks)
    {
        m_boneAnimations[boneIndex].getRotationAtTime(orientation, timeTicks);
    }

    // Skinned Model
    SkinnedModel::SkinnedModel() :
        Model(),

        m_currentAnimation(nullptr),
        m_currentAnimationLooping(true),
        m_currentAnimationTimeTicks(0.0f),

        m_rootBone(nullptr),
        m_rootAssimpBoneNode(nullptr)
    {
        m_boneTransforms.resize(MAX_SKINNED_MODEL_BONES);
    }

    SkinnedModel::~SkinnedModel()
    {
        for (auto& bone : m_boneDataMap)
        {
            delete bone.second;
        }
        m_boneDataMap.clear();
    }

    bool SkinnedModel::initialise(const std::string& filePath)
    {
        m_filePath = filePath;

        try
        {
            if (!loadSkinnedModel(filePath))
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

    void SkinnedModel::updateAlternatePerObjectCB() const
    {
        for (auto& bone : m_boneDataMap)
        {
            gfx::GraphicsHandler::it().getPerSkinnedObjectCB().m_data.m_boneTransforms[bone.second->m_id] = XMLoadFloat4x4(&(bone.second->m_offsetMatrix)) * XMLoadFloat4x4(&(m_boneTransforms[bone.second->m_id]));
        }
        gfx::GraphicsHandler::it().getPerSkinnedObjectCB().mapToGPU();
    }

    bool SkinnedModel::loadSkinnedModel(const std::string& filePath)
    {
        m_directory = utils::string_helpers::getDirectoryFromPath(filePath);

        std::string meshMaterialFilePath = utils::string_helpers::changeFileExtension(filePath, "mtl");
        if (!loadMeshMaterials(meshMaterialFilePath))
        {
            m_useEmbeddedMaterials = true;
        }

        Assimp::Importer importer;

        //aiProcess_ConvertToLeftHanded
        // v
        //aiProcess_MakeLeftHanded
        //aiProcess_FlipUVs
        //aiProcess_FlipWindingOrder

        const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);
        if (scene == nullptr)
        {
            return false;
        }

        preSceneParse(scene);

        m_isGBufferCompatible = true;

        processSkinnedNode(scene->mRootNode, scene, XMMatrixIdentity());// * XMMatrixScaling(0.025f, 0.025f, 0.025f)

        if (m_vertices.empty())
        {
            utils::ErrorLogger::log("Model at path '" + filePath + "' failed to load as it had no vertices");
            return false;
        }

        BoundingBox::CreateFromPoints(m_boundingBox, m_vertices.size(), &(m_vertices.at(0)), sizeof(XMFLOAT3));

        loadModelMetaData(filePath.substr(0, filePath.length() - 4) + "_meta.txt"); // Remove model file extension, add '_meta.txt'

        return true;
    }

    void SkinnedModel::processSkinnedNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
    {
        XMMATRIX nodeTransformationMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

        for (UINT i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            Material* meshMaterialPtr = nullptr;

            if (std::string(node->mName.C_Str()) == std::string("head"))
            {
                int cat = 12;
            }

            if (m_useEmbeddedMaterials)
            {
                MeshMaterial meshMaterial;
                meshMaterial.m_material = new Material();
                meshMaterial.m_deleteRequired = true;

                aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<MaterialTexture> finalMaterialTextures;

                std::vector<Texture*> albedoTextures;
                loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_DIFFUSE, scene, albedoTextures);
                if (!albedoTextures.empty())
                {
                    finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eAlbedo, albedoTextures[0]));
                }

                std::vector<Texture*> specularTextures;
                loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_SPECULAR, scene, specularTextures);
                if (!specularTextures.empty())
                {
                    finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eRoughness, specularTextures[0]));
                }

                std::vector<Texture*> normalTextures;
                loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_NORMALS, scene, normalTextures);
                if (!normalTextures.empty())
                {
                    finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eNormal, normalTextures[0]));
                }

                std::vector<Texture*> displacementTextures;
                loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_DISPLACEMENT, scene, displacementTextures);
                if (!displacementTextures.empty())
                {
                    finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eDepth, displacementTextures[0]));
                }

                meshMaterial.m_material->initialiseFromTextures(finalMaterialTextures);

                m_meshMaterials.insert({ std::string(node->mName.C_Str()), meshMaterial });

                meshMaterialPtr = meshMaterial.m_material;
            }
            else
            {
                const auto& meshMaterialsIterator = m_meshMaterials.find(node->mName.C_Str());
                if (meshMaterialsIterator != m_meshMaterials.end())
                {
                    meshMaterialPtr = meshMaterialsIterator->second.m_material;
                }
                else
                {
                    meshMaterialPtr = ResourceManager::it().getDefaultMaterialPtr();
                }
            }

            if (!meshMaterialPtr->isGBufferCompatible())
            {
                m_isGBufferCompatible = false;
            }

            m_meshes.push_back(processSkinnedMesh(mesh, meshMaterialPtr, nodeTransformationMatrix));
        }

        for (UINT i = 0; i < node->mNumChildren; i++)
        {
            processSkinnedNode(node->mChildren[i], scene, nodeTransformationMatrix);
        }
    }

    Mesh* SkinnedModel::processSkinnedMesh(aiMesh* mesh, Material* material, const XMMATRIX& transformMatrix)
    {
        std::vector<gfx::SkinnedVertex> meshVertices;
        std::vector<DWORD> indices;

        m_meshBaseVertices.push_back(m_vertices.size());

        preMeshProcessed((int)m_meshes.size(), mesh);

        for (UINT i = 0; i < mesh->mNumVertices; i++)
        {
            gfx::SkinnedVertex vertex;
            vertex.fillUsingAssimpMesh(mesh, i);
            fillVertexSkinningData(m_currentNumVerts + i, vertex);

            meshVertices.push_back(vertex);

            m_vertices.push_back(vertex.m_pos.getAsXMFLOAT3());
        }

        for (UINT i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (UINT j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
                m_indices.push_back(m_currentNumVerts + face.mIndices[j]);
            }
        }

        m_currentNumVerts = static_cast<int>(m_vertices.size());

        std::string meshName = mesh->mName.C_Str();
        return new Mesh(meshName, meshVertices, indices, material);
    }

    void SkinnedModel::fillVertexSkinningData(int vertexIndex, gfx::SkinnedVertex& vertex)
    {
        vertex.m_boneIndices[0] = m_vertexBoneData[vertexIndex].m_boneIds[0];
        vertex.m_boneIndices[1] = m_vertexBoneData[vertexIndex].m_boneIds[1];
        vertex.m_boneIndices[2] = m_vertexBoneData[vertexIndex].m_boneIds[2];
        vertex.m_boneIndices[3] = m_vertexBoneData[vertexIndex].m_boneIds[3];

        vertex.m_boneWeights[0] = m_vertexBoneData[vertexIndex].m_weights[0];
        vertex.m_boneWeights[1] = m_vertexBoneData[vertexIndex].m_weights[1];
        vertex.m_boneWeights[2] = m_vertexBoneData[vertexIndex].m_weights[2];
        vertex.m_boneWeights[3] = m_vertexBoneData[vertexIndex].m_weights[3];
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

    float* SkinnedModel::getCurrentAnimationTimePtr()
    {
        return &m_currentAnimationTimeTicks;
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

    const Bone* SkinnedModel::getRootBone() const
    {
        return m_rootBone;
    }

    std::map<std::string, SkeletonAnimation>& SkinnedModel::getAnimations()
    {
        return m_animations;
    }

    UINT SkinnedModel::getNumBones() const
    {
        return (UINT)m_boneDataMap.size();
    }

    void SkinnedModel::update(const XMMATRIX& modelMatrix, float deltaTime)
    {
        if (m_currentAnimation != nullptr)
        {
            m_currentAnimationTimeTicks += deltaTime * m_currentAnimation->m_ticksPerSecond;

            if (m_currentAnimationTimeTicks > m_currentAnimation->getEndTime())
            {
                m_currentAnimationTimeTicks -= m_currentAnimation->getEndTime();
            }
        }

        updateBoneTransforms(modelMatrix);
    }

    void SkinnedModel::debugDraw(const XMMATRIX& transformMatrix, bool bindPSData) const
    {
        Model* debugModel = ResourceManager::it().getModelPtr<FancyLitVertex>("res/models/normal_map_test_cube.obj");

        //GraphicsHandler::it().setDepthTestingEnabled(false);

        for (int boneIndex = 0; boneIndex < m_boneDataMap.size(); ++boneIndex)
        {
            debugModel->draw(XMMatrixScaling(2.5f, 7.5f, 2.5f) * XMLoadFloat4x4(&(m_boneTransforms[boneIndex])), bindPSData);// * transformMatrix
        }

        //GraphicsHandler::it().setDepthTestingEnabled(true);
    }

    void SkinnedModel::updateBoneTransforms(const XMMATRIX& modelMatrix)
    {
        UINT numBones = m_boneDataMap.size();

        // If we have an animation, use it
        if (m_currentAnimation != nullptr)
        {
            for (auto& boneIter : m_boneDataMap)
            {
                m_currentAnimation->getBoneTranslationAtTime(boneIter.second->m_id, boneIter.second->m_translation, m_currentAnimationTimeTicks);
                m_currentAnimation->getBoneScaleAtTime(boneIter.second->m_id, boneIter.second->m_scale, m_currentAnimationTimeTicks);
                m_currentAnimation->getBoneOrientationAtTime(boneIter.second->m_id, boneIter.second->m_orientationQuaternion, m_currentAnimationTimeTicks);
            }
        }
        else
        {
            for (auto& boneIter : m_boneDataMap)
            {
                boneIter.second->m_translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
                boneIter.second->m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
                boneIter.second->m_orientationQuaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        recursiveApplyBoneTransform(m_rootBone, modelMatrix);
    }

    void SkinnedModel::preSceneParse(const aiScene* scene)
    {
        m_currentNumVerts = 0;
        generateAssimpBoneMap(scene);
        int latestId = 0;
        m_rootBone = new Bone(0, "hrzn_root");
        m_boneDataMap["hrzn_root"] = m_rootBone;
        recursiveGenerateBoneHierarchy(m_rootBone, scene->mRootNode, latestId);
        loadAnimations(scene);

        m_rootAssimpBoneNode = nullptr;
        m_assimpBoneMap.clear();
    }

    void SkinnedModel::preMeshProcessed(int meshIndex, const aiMesh* mesh)
    {
        m_vertexBoneData.resize(m_currentNumVerts + mesh->mNumVertices);
        if (mesh->HasBones())
        {
            parseMeshBones(meshIndex, mesh);
        }
    }

    void SkinnedModel::generateAssimpBoneMap(const aiScene* scene)
    {
        m_assimpBoneMap.clear();

        for (int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            aiMesh* aiMesh = scene->mMeshes[meshIndex];

            for (int boneIndex = 0; boneIndex < aiMesh->mNumBones; ++boneIndex)
            {
                aiBone* bone = aiMesh->mBones[boneIndex];
                m_assimpBoneMap.insert({ bone->mName.C_Str(), bone });
            }
        }
    }

    void SkinnedModel::recursiveGenerateBoneHierarchy(Bone* bone, const aiNode* assimpNode, int& latestId)
    {
        if (bone == nullptr || assimpNode == nullptr)
        {
            return;
        }

        for (int childIndex = 0; childIndex < assimpNode->mNumChildren; ++childIndex)
        {
            aiNode* childNode = assimpNode->mChildren[childIndex];
            auto assimpBoneMapIter = m_assimpBoneMap.find(childNode->mName.C_Str());
            if (assimpBoneMapIter != m_assimpBoneMap.end())
            {
                Bone* newBone = new Bone(++latestId, assimpBoneMapIter->second);
                m_boneDataMap[childNode->mName.C_Str()] = newBone;

                newBone->m_parent = bone;
                bone->m_children.push_back(newBone);

                recursiveGenerateBoneHierarchy(newBone, childNode, latestId);
            }
            else
            {
                recursiveGenerateBoneHierarchy(bone, childNode, latestId);
            }
        }
    }

    void SkinnedModel::recursiveApplyBoneTransform(Bone* bone, XMMATRIX parentTransform)
    {
        XMMATRIX myTransform = XMMatrixScaling(bone->m_scale.x, bone->m_scale.y, bone->m_scale.z) * XMMatrixRotationQuaternion(XMLoadFloat4(&(bone->m_orientationQuaternion))) * XMMatrixTranslation(bone->m_translation.x, bone->m_translation.y, bone->m_translation.z) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&(bone->m_offsetMatrix))) * parentTransform;
        //XMMATRIX myTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&(bone->m_offsetMatrix))) * parentTransform;

        XMStoreFloat4x4(&(m_boneTransforms[bone->m_id]), myTransform);

        myTransform = XMLoadFloat4x4(&(bone->m_offsetMatrix)) * myTransform;

        //myTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&(bone->m_offsetMatrix))) * myTransform;

        for (auto* child : bone->m_children)
        {
            recursiveApplyBoneTransform(child, myTransform);
        }
    }

    void SkinnedModel::loadAnimations(const aiScene* scene)
    {
        for (int animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex)
        {
            const aiAnimation* assimpAnimation = scene->mAnimations[animationIndex];
            SkeletonAnimation skeletonAnimation;

            skeletonAnimation.m_boneAnimations.resize(m_boneDataMap.size());

            skeletonAnimation.m_ticksPerSecond = (float)assimpAnimation->mTicksPerSecond;

            for (int channelIndex = 0; channelIndex < assimpAnimation->mNumChannels; ++channelIndex)
            {
                assimpAnimation->mMorphMeshChannels;

                const aiNodeAnim* assimpAnimationNode = assimpAnimation->mChannels[channelIndex];
                std::string cleanedName = assimpAnimationNode->mNodeName.C_Str();
                size_t dollarSignPos = cleanedName.find('$');
                if (dollarSignPos != std::string::npos)
                {
                    cleanedName = cleanedName.substr(0, dollarSignPos - 1);
                }

                auto boneIter = m_boneDataMap.find(cleanedName);
                if (boneIter != m_boneDataMap.end())
                {
                    BoneAnimation& boneAnimation = skeletonAnimation.m_boneAnimations[boneIter->second->m_id];

                    for (int translationKeyframeIndex = 0; translationKeyframeIndex < assimpAnimationNode->mNumPositionKeys; ++translationKeyframeIndex)
                    {
                        aiVectorKey& translationKeyframe = assimpAnimationNode->mPositionKeys[translationKeyframeIndex];
                        aiVector3D& translation = translationKeyframe.mValue;
                        boneAnimation.m_translationKeyframes.push_back({ (float)translationKeyframe.mTime, XMFLOAT3(translation.x, translation.y, translation.z) });
                    }

                    for (int rotationKeyframeIndex = 0; rotationKeyframeIndex < assimpAnimationNode->mNumRotationKeys; ++rotationKeyframeIndex)
                    {
                        aiQuatKey& rotationKeyframe = assimpAnimationNode->mRotationKeys[rotationKeyframeIndex];
                        aiQuaternion& rotation = rotationKeyframe.mValue;
                        boneAnimation.m_rotationKeyframes.push_back({ (float)rotationKeyframe.mTime, XMFLOAT4(rotation.x, rotation.y, rotation.z, rotation.w) });
                    }

                    for (int scaleKeyframeIndex = 0; scaleKeyframeIndex < assimpAnimationNode->mNumScalingKeys; ++scaleKeyframeIndex)
                    {
                        aiVectorKey& rotationKeyframe = assimpAnimationNode->mScalingKeys[scaleKeyframeIndex];
                        aiVector3D& scale = rotationKeyframe.mValue;
                        boneAnimation.m_scaleKeyframes.push_back({ (float)rotationKeyframe.mTime, XMFLOAT3(scale.x, scale.y, scale.z) });
                    }

                    boneAnimation.calculateStartAndEndTimes();
                }
            };

            skeletonAnimation.calculateStartAndEndTimes();

            m_animations.insert({ assimpAnimation->mName.C_Str(), skeletonAnimation });
        }

        for (auto& animation : m_animations)
        {
            setCurrentAnimation(animation.first);
            break;
        }
    }

    void SkinnedModel::parseMeshBones(int meshIndex, const aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
            parseSingleBone(meshIndex, mesh->mBones[i]);
        }
    }

    void SkinnedModel::parseSingleBone(int meshIndex, const aiBone* bone)
    {
        int boneId = getBoneId(bone);

        for (unsigned int i = 0; i < bone->mNumWeights; i++)
        {
            const aiVertexWeight& vw = bone->mWeights[i];

            int globalVertexId = m_meshBaseVertices[meshIndex] + vw.mVertexId;
            m_vertexBoneData[globalVertexId].addBoneData(boneId, vw.mWeight);
        }
    }

    int SkinnedModel::getBoneId(const aiBone* bone)
    {
        std::string boneName(bone->mName.C_Str());

        auto boneNameIter = m_boneDataMap.find(boneName);
        if (boneNameIter != m_boneDataMap.end())
        {
            return boneNameIter->second->m_id;
        }

        return -1;
    }
}

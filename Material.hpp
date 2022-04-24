#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include <string>
#include <string.h>

class BaseMaterial
{
public:
    static std::unique_ptr<BaseMaterial> create(uint32_t prototypeId);

    virtual std::string getName() const = 0;
    virtual void setName(std::string name) = 0;

    virtual void setBaseColor(float value[4]);
    virtual void setRoughness(float value);
    virtual void setMetallic(float value);

    virtual const float *getBaseColor() const;
    virtual float getRoughness() const;
    virtual float getMetallic() const;

    virtual void load(const tinygltf::Material &mat) = 0;

    bool operator==(const BaseMaterial *rhs) { return (m_sName == rhs->m_sName); }

protected:
    std::string m_sName;

private:
    enum
    {
        PBR = 0,
    };
};

class PbrMaterial : public BaseMaterial
{
public:
    void setName(std::string name) override { m_sName = name; }
    std::string getName() const override { return m_sName; }

    void setBaseColor(float value[4]) override { memcpy(data.m_4fBaseColor, value, sizeof(data.m_4fBaseColor)); }
    void setRoughness(float value) override { data.m_fRoughness = value; }
    void setMetallic(float value) override { data.m_fMetallic = value; }

    const float *getBaseColor() const override { return data.m_4fBaseColor; }
    float getRoughness() const override { return data.m_fRoughness; }
    float getMetallic() const override { return data.m_fMetallic; }

    void load(const tinygltf::Material &gltfMat);

private:
    struct Data
    {
        float m_4fBaseColor[4] = {0.0, 0.0, 0.0, 0.0};
        float m_fRoughness = 0.0;
        float m_fMetallic = 0.0;
    } data;
};

std::unique_ptr<BaseMaterial> BaseMaterial::create(uint32_t protoypeId)
{
    switch (protoypeId)
    {
    case PBR:
        return std::make_unique<PbrMaterial>();
    default:
        break;
    }
}

void PbrMaterial::load(const tinygltf::Material &gltfMat)
{
    static const std::array<std::string, 3> members{
        "baseColorFactor",
        "metallicFactor",
        "roughnessFactor"};

    if constexpr (LOADER_DEBUG)
    {
        for (const std::string &memberName : members)
            assert(gltfMat.values.contains(memberName));
    }

    m_sName = gltfMat.name;

    auto &colorArray = gltfMat.values.at(members[0]).number_array;
    for (uint32_t i = 0; i < 4; ++i)
        data.m_4fBaseColor[i] = static_cast<float>(colorArray[i]);

    data.m_fMetallic = static_cast<float>(gltfMat.values.at(members[1]).number_value);
    data.m_fRoughness = static_cast<float>(gltfMat.values.at(members[2]).number_value);
}

#endif // MATERIAL_HPP
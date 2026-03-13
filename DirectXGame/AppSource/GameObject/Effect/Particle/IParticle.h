#pragma once
#include <Render/RenderObject.h>
#include <Assets/Texture/TextureManager.h>
#include <Render/DrawDataManager.h>
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Utility/Vector.h>
#include <Utility/Matrix.h>

enum class ParticleType
{
	None,
	Test1,
	Test2,
	Test3,
};

class IParticle
{
public:
	virtual ~IParticle() = default;

	virtual void Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix) = 0;
	virtual void Trigger(const Vector3& pos) = 0;
	virtual void Stop() = 0;
};
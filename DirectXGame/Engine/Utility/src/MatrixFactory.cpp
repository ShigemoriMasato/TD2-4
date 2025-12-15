#include "../MatrixFactory.h"

Matrix3x3 Matrix::MakeTranslationMatrix(const Vector2& translation) {
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		translation.x, translation.y, 1.0f
	};
}

Matrix3x3 Matrix::MakeRotationMatrix(float angle) {
	return {
		std::cosf(angle), std::sinf(angle), 0.0f,
		-sinf(angle), std::cosf(angle), 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix::MakeScaleMatrix(const Vector2& scale) {
	return {
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix::MakeAffineMatrix(const Vector2& scale, const float angle, const Vector2 translation) {
	return MakeScaleMatrix(scale) * MakeRotationMatrix(angle) * MakeTranslationMatrix(translation);
}

Matrix3x3 Matrix::TransMatrix(const Matrix3x3& m) {
	Matrix3x3 ans{};
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			ans.m[j][i] = m.m[i][j];
		}
	}
	return ans;
}

Matrix4x4 Matrix::TransMatrix(const Matrix4x4& m) {
	Matrix4x4 ans{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ans.m[j][i] = m.m[i][j];
		}
	}
	return ans;
}

Matrix4x4 Matrix::MakeUVMatrix(Vector2 scale, float rotate, Vector2 position) {
	return MakeScaleMatrix(Vector3(scale.x, scale.y, 1.0f)) *
		MakeRotationZMatrix(rotate) *
		MakeTranslationMatrix(Vector3(position.x, position.y, 0.0f));
}

Matrix4x4 Matrix::MakeTranslationMatrix(const Vector3& pos) {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, pos.z, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotationMatrix(Vector3 angle) {
	return 
		MakeRotationXMatrix(angle.x) *
		MakeRotationYMatrix(angle.y) *
		MakeRotationZMatrix(angle.z);
}

Matrix4x4 Matrix::MakeRotationXMatrix(float angle) {
	return {
		1.0f ,0.0f, 0.0f, 0.0f,
		0.0f, std::cosf(angle), std::sinf(angle), 0.0f,
		0.0f, -std::sinf(angle), std::cosf(angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotationYMatrix(float angle) {
	return {
		std::cosf(angle), 0.0f, -std::sinf(angle), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sinf(angle), 0.0f, std::cosf(angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotationZMatrix(float angle) {
	return {
		std::cosf(angle), std::sinf(angle), 0.0f, 0.0f,
		-sinf(angle), std::cosf(angle), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeScaleMatrix(const Vector3& scale) {
	return {
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
	return MakeScaleMatrix(scale) *
		MakeRotationMatrix(rotation) *
		MakeTranslationMatrix(translation);
}

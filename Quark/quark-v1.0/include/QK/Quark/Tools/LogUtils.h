#pragma once

#include "../Core/Core.h"

// Logging vectors
inline std::ostream& operator<<(std::ostream& os, const glm::vec2& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec4& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << ", w:" << vec.w << " ]";
}


inline std::ostream& operator<<(std::ostream& os, const glm::dvec2& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::dvec3& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::dvec4& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << ", w:" << vec.w << " ]";
}


inline std::ostream& operator<<(std::ostream& os, const glm::ivec2& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::ivec3& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::ivec4& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << ", w:" << vec.w << " ]";
}


// Logging matrices
inline std::ostream& operator<<(std::ostream& os, const glm::mat4& mat)
{
	os << std::fixed;
	os << "+-------------------------------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " | " << mat[2][0] << " | " << mat[3][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " | " << mat[2][1] << " | " << mat[3][1] << " |\n";
	os << "| " << mat[0][2] << " | " << mat[1][2] << " | " << mat[2][2] << " | " << mat[3][2] << " |\n";
	os << "| " << mat[0][3] << " | " << mat[1][3] << " | " << mat[2][3] << " | " << mat[3][3] << " |\n";
	os << "+-------------------------------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::mat3& mat)
{
	os << std::fixed;
	os << "+--------------------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " | " << mat[2][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " | " << mat[2][1] << " |\n";
	os << "| " << mat[0][2] << " | " << mat[1][2] << " | " << mat[2][2] << " |\n";
	os << "+--------------------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::mat2& mat)
{
	os << std::fixed;
	os << "+---------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " |\n";
	os << "+---------------------+";
	return  os;
}

// Logging quaternions
inline std::ostream& operator<<(std::ostream& os, const glm::quat& quat)
{
	os << std::fixed;
	os << "[ w:" << quat.w << ", <x:" << quat.x << ", y:" << quat.y << ", z:" << quat.z << "> ]";
	return  os;
}

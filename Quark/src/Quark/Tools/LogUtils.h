#pragma once

#include "../Core/Core.h"

// Logging vectors
inline std::ostream& operator<<(std::ostream& os, const Quark::Vector2f& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Vector3f& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Vector4f& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << ", w:" << vec.w << " ]";
}


inline std::ostream& operator<<(std::ostream& os, const Quark::Vector2d& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Vector3d& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Vector4d& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << ", w:" << vec.w << " ]";
}


inline std::ostream& operator<<(std::ostream& os, const Quark::Vector2i& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Vector3i& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << " ]";
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Vector4i& vec)
{
	os << std::fixed;
	return os << "[ x:" << vec.x << ", y:" << vec.y << ", z:" << vec.z << ", w:" << vec.w << " ]";
}


// Logging matrices
inline std::ostream& operator<<(std::ostream& os, const Quark::Mat2f& mat)
{
	os << std::fixed;
	os << "+---------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " |\n";
	os << "+---------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Mat3f& mat)
{
	os << std::fixed;
	os << "+---------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " |\n";
	os << "+---------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Mat4f& mat)
{
	os << std::fixed;
	os << "+---------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " |\n";
	os << "+---------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Mat2d& mat)
{
	os << std::fixed;
	os << "+---------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " |\n";
	os << "+---------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Mat3d& mat)
{
	os << std::fixed;
	os << "+--------------------------------+\n";
	os << "| " << mat[0][0] << " | " << mat[1][0] << " | " << mat[2][0] << " |\n";
	os << "| " << mat[0][1] << " | " << mat[1][1] << " | " << mat[2][1] << " |\n";
	os << "| " << mat[0][2] << " | " << mat[1][2] << " | " << mat[2][2] << " |\n";
	os << "+--------------------------------+";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Mat4d& mat)
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


// Logging quaternions
inline std::ostream& operator<<(std::ostream& os, const Quark::Quatf& quat)
{
	os << std::fixed;
	os << "[ w:" << quat.w << ", <x:" << quat.x << ", y:" << quat.y << ", z:" << quat.z << "> ]";
	return  os;
}

inline std::ostream& operator<<(std::ostream& os, const Quark::Quatd& quat)
{
	os << std::fixed;
	os << "[ w:" << quat.w << ", <x:" << quat.x << ", y:" << quat.y << ", z:" << quat.z << "> ]";
	return  os;
}

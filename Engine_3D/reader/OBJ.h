#pragma once
//OBJ reader

#ifndef _OBJPARSER_H_
#define _OBJPARSER_H_

#include <string>
#include <fstream>
#include <sstream>

class ObjParser
{
private:
	struct VertexType
	{
		float x, y, z;
	};

	struct FaceType
	{
		union {
			struct {
				int vIndex1, vIndex2, vIndex3, vIndex4;
			};
			int vIndex[4];
		};
		union {
			struct {
				int tIndex1, tIndex2, tIndex3, tIndex4;
			};
			int tIndex[4];
		};
		union {
			struct {
				int nIndex1, nIndex2, nIndex3, nIndex4;
			};
			int nIndex[4];
		};

	};

public:
	ObjParser();
	ObjParser(const ObjParser& other);
	~ObjParser();

	void Malloc();
	void Release();
	void AddVertice(float x, float y, float z) {
		vertices[mVertexCount].x = x;
		vertices[mVertexCount].y = y;
		vertices[mVertexCount].z = z;// -1.0f * z;  // RH->LH
		mVertexCount++;
	}
	void AddTexture(float u, float v) {
		texcoords[mTexcoordCount].x = u;
		texcoords[mTexcoordCount].y = v;// 1.0f - v;// RH->LH
		mTexcoordCount++;
	}
	void AddNormal(float x, float y, float z) {
		normals[mNormalCount].x = x;
		normals[mNormalCount].y = y;
		normals[mNormalCount].z = z;// -1.0f * z;  // RH->LH
		mNormalCount++;
	}
	void AddFace(int v, int t, int n, int i, int end) {
		faces[mFaceCount].vIndex[i] = v;
		faces[mFaceCount].tIndex[i] = t;
		faces[mFaceCount].nIndex[i] = n;
		if (end) {
			mFaceCount++;
		}
	}

	bool Parse(char* fileName);

	#define MAX_STR 100
	#define MAX_PAR	10
	typedef int(*parseParameterFun)(char [], char [], char [][MAX_STR], int );
	bool ParseEx(char* fileName, char buffer[], char command[], char parameters[][MAX_STR], char attrs[][MAX_STR], int OBJ_MAX_STR, parseParameterFun parseParameter);
public:
	void SetFileName(char* fileName);
	bool ReadFileCounts();
	bool LoadDataStructures();

	std::ofstream mErr;
	std::string mInFile;
	std::string mOutFile;
	int mVertexCount;
	int mTexcoordCount;
	int mNormalCount;
	int mFaceCount;

	VertexType* vertices;
	VertexType* texcoords;
	VertexType* normals;
	FaceType* faces;
};
#endif
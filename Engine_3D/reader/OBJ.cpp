#pragma once
//OBJ reader

#include "OBJ.h"

#include "../flatform/Drawable.h"

ObjParser::ObjParser() :
	mInFile(""), mOutFile(""),
	mVertexCount(0), mTexcoordCount(0), mNormalCount(0), mFaceCount(0),
	vertices(NULL), texcoords(NULL), normals(NULL), faces(NULL)
{
	mErr.open("parser-error-log.txt");
}

void ObjParser::Malloc() {
	vertices = new VertexType[mVertexCount];
	texcoords = new VertexType[mTexcoordCount];
	normals = new VertexType[mNormalCount];
	faces = new FaceType[mFaceCount];
	memset(vertices, 0, sizeof(VertexType) * mVertexCount);
	memset(texcoords, 0, sizeof(VertexType) * mTexcoordCount);
	memset(normals, 0, sizeof(VertexType) * mNormalCount);
	memset(faces, 0, sizeof(FaceType) * mFaceCount);
}

void ObjParser::Release() {

	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals)
	{
		delete[] normals;
		normals = 0;
	}
	if (faces)
	{
		delete[] faces;
		faces = 0;
	}
}

ObjParser::ObjParser(const ObjParser& other)
{
}

ObjParser::~ObjParser()
{
	mErr.close();
}

bool ObjParser::ParseEx(char* fileName, char buffer[], char command[], char parameters[][MAX_STR], char attrs[][MAX_STR], int OBJ_MAX_STR, parseParameterFun parseParameter) {
	SetFileName(fileName);

	int attrCount = 0;
	FILE * _fp = NULL;
	fopen_s(&_fp, fileName, "r");
	int result = 0;
	if (_fp) {
		while (!feof(_fp)) {
			fgets(buffer, OBJ_MAX_STR, _fp);

			attrCount = parseParameter(buffer, command, attrs, 0);

			if (!strcmp(command, "vt")) {
				mTexcoordCount++;
				result++;
			}
			else if (!strcmp(command, "vn")) {
				mNormalCount++;
				result++;
			}
			else if (!strcmp(command, "v")) {
				mVertexCount++;
				result++;
			}
			else if (!strcmp(command, "f")) {
				mFaceCount += attrCount;
				result++;
			}
		}

		if (result > 0) {
			Malloc();
			mVertexCount = 0;
			mTexcoordCount = 0;
			mNormalCount = 0;
			mFaceCount = 0;

			result = 0;
			rewind(_fp);
			while (!feof(_fp)) {
				buffer[0] = 0;
				fgets(buffer, OBJ_MAX_STR, _fp);

				attrCount = parseParameter(buffer, command, attrs, 0);

				if (!strcmp(command, "vt")) {
					if (attrCount >= 2) {
						AddTexture(atof(attrs[0]), atof(attrs[1]));
						result++;
					}
				}
				else if (!strcmp(command, "vn")) {
					if (attrCount >= 3) {
						AddNormal(atof(attrs[0]), atof(attrs[1]), atof(attrs[2]));
						result++;
					}
				}
				else if (!strcmp(command, "v")) {
					if (attrCount >= 3) {
						AddVertice(atof(attrs[0]), atof(attrs[1]), atof(attrs[2]));
						result++;
					}
				}
				else if (!strcmp(command, "f")) {
					union {
						struct {
							char * v, *t, *n;
						};
						char * _attr[3];
					};
					char * attr;
					if (attrCount > 4) {
						attrCount = 4;
					}
					for (int i = 0; i < attrCount; i++) {
						attr = attrs[i];
						for (int j = 0; j < 3; j++) {
							_attr[j] = NULL;
						}
						int index = 0;
						_attr[index++] = attr;
						for (int j = 0; attr[j]; j++) {
							if (attr[j] == '/') {
								attr[j] = 0;
								_attr[index++] = attr + j + 1;
								if (index >= 3) {
									break;
								}
							}
						}
						AddFace(v ? atoi(v) : 0, t ? atoi(t) : 0, n ? atoi(n) : 0, i, attrCount - i == 1 ? 1 : 0);
						result++;
					}
				}
			}
		}
	}
	return result;
}
bool ObjParser::Parse(char* fileName)
{
	SetFileName(fileName);
	if (!ReadFileCounts())
		return false;
	if (!LoadDataStructures())
		return false;

	return true;
}

void ObjParser::SetFileName(char* fileName)
{
	std::stringstream ss;
	ss << fileName;
	ss >> mInFile;

	mOutFile = mInFile;
	int size = mOutFile.size();
	mOutFile[size - 3] = 't';
	mOutFile[size - 2] = 'x';
	mOutFile[size - 1] = 't';
}

bool ObjParser::ReadFileCounts()
{
	char input;
	std::ifstream fin;
	fin.open(mInFile);
	if (fin.fail())
	{
		mErr << "Open input file : Failed." << std::endl;
		return false;
	}

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			switch (input)
			{
			case ' ':
			{
				mVertexCount++;
				break;
			}
			case 't':
			{
				mTexcoordCount++;
				break;
			}
			case 'n':
			{
				mNormalCount++;
				break;
			}
			}
		}
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
				mFaceCount++;
		}

		// otherwise read in the remainder of the line.
		while (input != '\n')
			fin.get(input);

		// start reading the beginning of the next line.
		fin.get(input);
	}
	fin.close();

	return true;
}

bool ObjParser::LoadDataStructures()
{

	this->Malloc();

	// After get enough memory, clear these counts to 0, use it as index
	mVertexCount = 0;
	mTexcoordCount = 0;
	mNormalCount = 0;
	mFaceCount = 0;

	std::ifstream fin;
	fin.open(mInFile);
	if (fin.fail())
	{
		mErr << "Open input file : Failed." << std::endl;
		return false;
	}
	char input;
	char ignore;
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			switch (input)
			{
			case ' ':
			{
				fin >> vertices[mVertexCount].x >> vertices[mVertexCount].y >> vertices[mVertexCount].z;
				vertices[mVertexCount].z *= -1.0f;  // RH->LH
				mVertexCount++;
				break;
			}
			case 't':
			{
				fin >> texcoords[mTexcoordCount].x >> texcoords[mTexcoordCount].y;
				texcoords[mTexcoordCount].y = 1.0f - texcoords[mTexcoordCount].y;  // RH->LH
				mTexcoordCount++;
				break;
			}
			case 'n':
			{
				fin >> normals[mNormalCount].x >> normals[mNormalCount].y >> normals[mNormalCount].z;
				normals[mNormalCount].z *= -1.0f; // RH->LH
				mNormalCount++;
				break;
			}
			}
		}
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[mFaceCount].vIndex1 >> ignore >> faces[mFaceCount].tIndex1 >> ignore >> faces[mFaceCount].nIndex1;
				fin >> faces[mFaceCount].vIndex2 >> ignore >> faces[mFaceCount].tIndex2 >> ignore >> faces[mFaceCount].nIndex2;
				fin >> faces[mFaceCount].vIndex3 >> ignore >> faces[mFaceCount].tIndex3 >> ignore >> faces[mFaceCount].nIndex3;
				//fin >> faces[mFaceCount].vIndex4 >> ignore >> faces[mFaceCount].tIndex4 >> ignore >> faces[mFaceCount].nIndex4;
				mFaceCount++;
				if (fin.eof()) break;
			}
		}

		while (input != '\n')
			fin.get(input);

		fin.get(input);
	}
	fin.close();
	std::ofstream fout(mOutFile, std::ios::out);
	if (!fout)
	{
		mErr << "Find output file : Failed." << std::endl;
		return false;
	}

	// Write out the file header that our model format uses.
	fout << "Vertex Count: " << (mFaceCount * 3) << std::endl;
	fout << std::endl;
	fout << "Data:" << std::endl;
	fout << std::endl;

	int vIndex, tIndex, nIndex;
	for (int i = 0; i<mFaceCount; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;
	}

	fout.close();
	mErr << "Successed in parsing " << mInFile << std::endl;

	return true;
}


//Texture.h
//

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string> 
using namespace std;
#include "../math3d/Manager3D.h"

class Texture {
public:
	Texture() : texture(NULL) {
		initialize();
	}

	~Texture() {
		if (texture) {
			delete[] texture;
			texture = NULL;
		}
	}

	INT width;
	INT height;
	DWORD *texture;
	EIMAGE image;

	void LoadTexture(char * filename) {
		if (texture) {
			return;
		}
		EFTYPE w = 0, h = 0;
		EP_LoadImage(image, filename, w, h);
		height = EP_GetImageHeight(image);
		width = EP_GetImageWidth(image);

		texture = new DWORD[sizeof(DWORD)* height * width];
		DWORD * buffer = EP_GetImageBuffer(image);
		INT index, index_r, index_t = width * height - 1;
		for (int y = 0; y < height; y++){
			for (int x = 0; x < width; x++)
			{
				index = x + y * width;
				index_r = index_t -  index;
				texture[index] = buffer[index_r];
			}
		}
	}

	void LoadTexture(INT w, INT h, INT n){
		if (texture) {
			return;
		}
		width = w;
		height = h;
		texture = new DWORD[sizeof(DWORD)* width * height];
		if (0 == n) n = 2;
		int d = width / n;
		int s = 1;
		for (int i = 0; i < width; i++) {
			if (i % d == 0) {
				s = -s;
			}
			for (int j = 0; j < height; j++) {
				if (j % d == 0) {
					s = -s;
				}
				texture[j * width + i] = s > 0 ? DARKGRAY : LIGHTGRAY;
			}
		}
	}

	// for multilinklist
#define MAX_TEXTURE_LINK	3
	void initialize() {
		for (INT i = 0; i < MAX_TEXTURE_LINK; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
	}
	INT uniqueID;
	Texture * prev[MAX_TEXTURE_LINK];
	Texture * next[MAX_TEXTURE_LINK];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < MAX_TEXTURE_LINK; i++)
		{
			if (((Texture*)_ptr)->prev[i] != NULL || ((Texture*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
		_ptr = NULL;
	}
};

class TextureManage {
public:
	TextureManage() : textures(0){

	}
	~TextureManage() {
		textures.~MultiLinkList();
	}

	MultiLinkList<Texture> textures;

	INT addTexture(char * filename) {
		Texture * texture = new Texture();
		texture->uniqueID = this->textures.linkcount;
		texture->LoadTexture(filename);

		this->textures.insertLink(texture);

		return texture->uniqueID;
	}

	INT addTexture(INT w, INT h, INT n) {
		Texture * texture = new Texture();
		texture->uniqueID = this->textures.linkcount;
		texture->LoadTexture(w, h, n);

		this->textures.insertLink(texture);

		return texture->uniqueID;
	}
};

#endif//_TEXTURE_H_
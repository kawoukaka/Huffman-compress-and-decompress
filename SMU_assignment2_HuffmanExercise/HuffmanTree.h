#ifndef __HUFFMANTREE_H_
#define __HUFFMANTREE_H_

#include <stdlib.h>
#include <malloc.h>
#include <cstring>
#include <iostream>

bool CompressHuffman(unsigned char *pSrc, int nSrcLen, unsigned char *&pDes, int &nDesLen);
bool DecompressHuffman(unsigned char *pSrc, int nSrcLen, unsigned char *&pDes, int &nDesLen);

#endif __HUFFMANTREE_H_
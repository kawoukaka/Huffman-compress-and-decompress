#include "HuffmanTree.h"

#define LOG(x) {std::cout << x << std::endl;};


class CHuffmanNode
{
public:
	CHuffmanNode() { memset(this, 0, sizeof(CHuffmanNode)); }

	int nFrequency;	// must be first for unsigned char align
	unsigned char byAscii;
	unsigned long dwCode;
	int nCodeLength;
	CHuffmanNode *pParent, *pLeft, *pRight;
};

int __cdecl frequencyCompare(const void *elem1, const void *elem2)
{
	CHuffmanNode *pNodes[2] = { (CHuffmanNode*)elem1, (CHuffmanNode*)elem2 };
	if (pNodes[0]->nFrequency == pNodes[1]->nFrequency)
		return 0;
	return pNodes[0]->nFrequency < pNodes[1]->nFrequency ? 1 : -1;
}

int __cdecl asciiCompare(const void *elem1, const void *elem2)
{
	return ((CHuffmanNode*)elem1)->byAscii >((CHuffmanNode*)elem2)->byAscii ? 1 : -1;
}

CHuffmanNode* PopNode(CHuffmanNode *pNodes[], int nIndex, bool bRight)
{
	CHuffmanNode* pNode = pNodes[nIndex];
	pNode->dwCode = bRight;
	pNode->nCodeLength = 1;
	return pNode;
}

void SetNodeCode(CHuffmanNode* pNode)
{
	CHuffmanNode* pParent = pNode->pParent;
	while (pParent && pParent->nCodeLength)
	{
		pNode->dwCode <<= 1;
		pNode->dwCode |= pParent->dwCode;
		pNode->nCodeLength++;
		pParent = pParent->pParent;
	}
}

int GetHuffmanTree(CHuffmanNode nodes[], bool bSetCodes = true)
{
	int nCount;
	int i;
	CHuffmanNode* pNodes[256], *pNode;
	// add used ascii to Huffman queue
	int nNodeCount = 0;
	for (int nCount = 0; nCount < 256 && nodes[nCount].nFrequency; nCount++)
		pNodes[nNodeCount++] = &nodes[nCount];
	int nParentNode = nNodeCount, nBackNode = nNodeCount - 1;
	while (nBackNode > 0)
	{
		// parent node
		pNode = &nodes[nParentNode++];
		// pop first child
		pNode->pLeft = PopNode(pNodes, nBackNode--, false);
		// pop second child
		pNode->pRight = PopNode(pNodes, nBackNode--, true);
		// adjust parent of the two poped nodes
		pNode->pLeft->pParent = pNode->pRight->pParent = pNode;
		// adjust parent frequency
		pNode->nFrequency = pNode->pLeft->nFrequency + pNode->pRight->nFrequency;
		// insert parent node depending on its frequency
		for (i = nBackNode; i >= 0; i--)
			if (pNodes[i]->nFrequency >= pNode->nFrequency)
				break;
		memmove(pNodes + i + 2, pNodes + i + 1, (nBackNode - i)*sizeof(int));
		pNodes[i + 1] = pNode;
		nBackNode++;
	}
	// set tree leaves nodes code
	if (bSetCodes)
		for (nCount = 0; nCount < nNodeCount; nCount++)
			SetNodeCode(&nodes[nCount]);

	return nNodeCount;
}

bool CompressHuffman(unsigned char *pSrc, int nSrcLen, unsigned char *&pDes, int &nDesLen)
{
	int nCount;
	CHuffmanNode nodes[511];
	
	// initialize nodes ascii
	for (nCount = 0; nCount < 256; nCount++)
		nodes[nCount].byAscii = nCount;
	// get ascii frequencies
	for (int nCount = 0; nCount < nSrcLen; nCount++)
		nodes[pSrc[nCount]].nFrequency++;
	// sort ascii chars depending on frequency
	qsort(nodes, 256, sizeof(CHuffmanNode), frequencyCompare);
	// construct Huffman tree
	int nNodeCount = GetHuffmanTree(nodes);
	// construct compressed buffer
	int nNodeSize = sizeof(unsigned long) + sizeof(unsigned char);
	nDesLen = nSrcLen + nNodeCount*nNodeSize;
	pDes = (unsigned char*)malloc(nDesLen);
	unsigned char *pDesPtr = pDes;
	memset(pDesPtr, 0, nDesLen);
	
	// save source buffer length at the first unsigned long
	*(unsigned long*)pDesPtr = nSrcLen;
	pDesPtr += sizeof(unsigned long);
	// save Huffman tree leaves count-1 (as it may be 256)
	*pDesPtr = nNodeCount - 1;
	pDesPtr += sizeof(unsigned char);
	
	// save Huffman tree used leaves nodes
	for (nCount = 0; nCount < nNodeCount; nCount++)
	{	// the array sorted on frequency so used nodes come first
		memcpy(pDesPtr, &nodes[nCount], nNodeSize);
		pDesPtr += nNodeSize;
	}
	// sort nodes depending on ascii to can index nodes with its ascii value
	qsort(nodes, 256, sizeof(CHuffmanNode), asciiCompare);

	int nDesIndex = 0;
	// loop to write codes
	for (nCount = 0; nCount < nSrcLen; nCount++)
	{
		*(unsigned long*)(pDesPtr + (nDesIndex >> 3)) |= nodes[pSrc[nCount]].dwCode << (nDesIndex & 7);
		nDesIndex += nodes[pSrc[nCount]].nCodeLength;
	}
	// update destination length
	nDesLen = (pDesPtr - pDes) + (nDesIndex + 7) / 8;
	pDes = (unsigned char*)realloc(pDes, nDesLen);

	return true;
}

bool DecompressHuffman(unsigned char *pSrc, int nSrcLen, unsigned char *&pDes, int &nDesLen)
{
	// copy destination final length
	nDesLen = *(unsigned long*)pSrc;
	// allocate buffer for decompressed buffer
	pDes = (unsigned char*)malloc(nDesLen + 1);
	int nNodeCount = *(pSrc + sizeof(unsigned long)) + 1;
	// initialize Huffman nodes with frequency and ascii
	CHuffmanNode nodes[511], *pNode;
	int nNodeSize = sizeof(unsigned long) + sizeof(unsigned char), nSrcIndex = nNodeSize;
	for (int nCount = 0; nCount < nNodeCount; nCount++)
	{
		memcpy(&nodes[nCount], pSrc + nSrcIndex, nNodeSize);
		nSrcIndex += nNodeSize;
	}
	// construct Huffman tree
	GetHuffmanTree(nodes, false);
	// get Huffman tree root
	CHuffmanNode *pRoot = &nodes[0];
	while (pRoot->pParent)
		pRoot = pRoot->pParent;

	int nDesIndex = 0;
	unsigned long nCode;
	nSrcIndex <<= 3;	// convert from bits to unsigned chars
	while (nDesIndex < nDesLen)
	{
		nCode = (*(unsigned long*)(pSrc + (nSrcIndex >> 3))) >> (nSrcIndex & 7);
		pNode = pRoot;
		while (pNode->pLeft)	// if node has pLeft then it must has pRight
		{	// node not leaf
			pNode = (nCode & 1) ? pNode->pRight : pNode->pLeft;
			nCode >>= 1;
			nSrcIndex++;
		}
		pDes[nDesIndex++] = pNode->byAscii;
	}

	return true;
}


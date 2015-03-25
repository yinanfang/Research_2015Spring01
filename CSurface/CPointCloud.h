//===============================================
//			CPointCloud.h
//			Mingsong Dou (doums@cs.unc.edu)
//			August, 2010
//===============================================

#pragma once
#include <stdio.h>
template<class T>
class CPointCloud
{
public:
	CPointCloud()
		: points_num(0),
		  points(NULL),
		  normals(NULL),
		  colors(NULL)
	{}

	CPointCloud( CPointCloud<T> const& others );

	~CPointCloud()
	{
		if(points != NULL)	
			delete [] points;
		if(normals != NULL) 
			delete [] normals;
		if( colors != NULL )
			delete [] colors;
	}

public:
	CPointCloud<T>& operator=( CPointCloud<T> const& rhs );

	bool hasColor() {return this->colors != NULL;}
	bool hasNormal() {return this->normals != NULL;}

	bool writeToFileASCII(const char *filename) const;
	bool readFromFileASCII(const char *filename);
	bool writeToFileBIN(const char *filename) const;
	bool readFromFileBIN(const char *filename);

	bool readFromFile(char const*filename)
	{
		int len = strlen(filename);
		if( len > 3 )
		{
			if( strcmp("bin", &filename[len-3])==0)
			{
				if( ! this->readFromFileBIN(filename))
				{
					printf("Error: file name <%s> is illegal!\n", filename);
					return false;
				}
			}
			else
			{
				if( !this->readFromFileASCII(filename) )
				{
						
					printf("Error: file name <%s> is illegal!\n", filename);
					return false;
				}
			}
			return true;
		}
		else
		{
			printf("Error: file name <%s> is illegal!\n", filename);
			return false;
		}
	}


public:
	int points_num;
	T* points;
	T* normals;
	T *colors;

public:
	void freeMemory()
	{
		if( this->points != NULL )
		{
			delete [] this->points;
			this->points = NULL;
		}
		if(this->normals != NULL )
		{
			delete [] this->normals;
			this->normals = NULL;
		}
		if(this->colors != NULL )
		{
			delete [] this->colors;
			this->colors = NULL;
		}		
	}
};

#include "CPointCloud.hpp"

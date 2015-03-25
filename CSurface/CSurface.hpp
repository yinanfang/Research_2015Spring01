//===============================================
//			CSurface.hpp
//			Mingsong Dou (doums@cs.unc.edu)
//			August, 2010
//===============================================
#pragma once
#ifndef __CSURFACE_HPP__
#define __CSURFACE_HPP__

#include "CSurface.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <map>
#include <math.h>
using namespace std;

template <class T>
CSurface<T>::CSurface(CSurface<T> const& other)
{
	//printf("Error: Copy function to be implemented!\n");
	this->vtDim = other.vtDim;
	this->vtNum = other.vtNum;
	this->triNum = other.triNum;
	this->color = other.color;
	this->normal = other.normal;

	if(other.vtNum != 0 && other.vtData != NULL)
	{
		this->vtData = new T[this->vtNum*this->vtDim];
		memcpy(this->vtData, other.vtData, this->vtNum*this->vtDim*sizeof(T));
	}
	else
		this->vtData = NULL;
	
	if( other.triNum != 0 && other.triangles != NULL)
	{
		this->triangles = new int[triNum*3];
		memcpy(this->triangles, other.triangles, triNum*3*sizeof(int));
	}
	else
		this->triangles = NULL;

	if( other.colors_of_view.size() > 0 )
	{
		for(int i=0; i<other.colors_of_view.size(); i++)
		{
			float* colors = NULL;
			if( other.colors_of_view[i] != NULL )
			{
				colors = new float[3*this->vtNum];
				memcpy(colors, other.colors_of_view[i], sizeof(float)*this->vtNum*3);
			}
			this->colors_of_view.push_back(colors);
		}
	}
}

template <class T>
CSurface<T>& CSurface<T>::operator=( CSurface<T> const& rhs )
{
	this->vtDim = rhs.vtDim;
	this->vtNum = rhs.vtNum;
	this->triNum = rhs.triNum;
	this->color = rhs.color;
	this->normal = rhs.normal;

	if( this->vtData != NULL )
		delete [] this->vtData;
	if(rhs.vtNum != 0 && rhs.vtData != NULL)
	{
		this->vtData = new T[this->vtNum*this->vtDim];
		memcpy(this->vtData, rhs.vtData, this->vtNum*this->vtDim*sizeof(T));
		//for(int i=0; i<this->vtNum*this->vtDim; i++)
		//{
		//	this->vtData[i] = rhs.vtData[i];
		//}
	}
	else
		this->vtData = NULL;

	if(this->triangles != NULL )
		delete [] this->triangles;
	if(rhs.triNum != 0 && rhs.triangles != NULL )
	{
		this->triangles = new int[triNum*3];
		memcpy(this->triangles, rhs.triangles, triNum*3*sizeof(int));
	}
	else
		this->triangles = NULL;

	//relase old data of colors_of_view
	for(int i=0; i<this->colors_of_view.size(); i++)
	{
		if( this->colors_of_view[i] != NULL )
			delete [] this->colors_of_view[i];
	}
	this->colors_of_view.clear();

	//copy data from rhs.colors_of_view
	if( rhs.colors_of_view.size() > 0 )
	{
		for(int i=0; i<rhs.colors_of_view.size(); i++)
		{
			float* colors = NULL;
			if( rhs.colors_of_view[i] != NULL )
			{
				colors = new float[3*this->vtNum];
				memcpy(colors, rhs.colors_of_view[i], sizeof(float)*this->vtNum*3);
			}
			this->colors_of_view.push_back(colors);
		}
	}

	return *this;
}

template<class T>
template<class T2>
void CSurface<T>::copy_from_another(CSurface<T2> const& other)
{
	this->vtDim = other.vtDim;
	this->vtNum = other.vtNum;
	this->triNum = other.triNum;
	this->color = other.color;
	this->normal = other.normal;

	if (this->vtData != NULL)
		delete[] this->vtData;
	if (other.vtNum != 0 && other.vtData != NULL)
	{
		this->vtData = new T[this->vtNum*this->vtDim];
		for(int i=0; i<this->vtNum*this->vtDim; i++)
		{
			this->vtData[i] = other.vtData[i];
		}
	}
	else
		this->vtData = NULL;

	if (this->triangles != NULL)
		delete[] this->triangles;
	if (other.triNum != 0 && other.triangles != NULL)
	{
		this->triangles = new int[triNum * 3];
		memcpy(this->triangles, other.triangles, triNum * 3 * sizeof(int));
	}
	else
		this->triangles = NULL;

	//relase old data of colors_of_view
	for (int i = 0; i<this->colors_of_view.size(); i++)
	{
		if (this->colors_of_view[i] != NULL)
			delete[] this->colors_of_view[i];
	}
	this->colors_of_view.clear();

	//copy data from rhs.colors_of_view
	if (other.colors_of_view.size() > 0)
	{
		for (int i = 0; i<other.colors_of_view.size(); i++)
		{
			float* colors = NULL;
			if (other.colors_of_view[i] != NULL)
			{
				colors = new float[3 * this->vtNum];
				memcpy(colors, other.colors_of_view[i], sizeof(float)*this->vtNum * 3);
			}
			this->colors_of_view.push_back(colors);
		}
	}
}

template <class T>
bool CSurface<T>::readFromFile(const char* filename)
{
	int len = strlen(filename);
	if (len > 3)
	{
		if (strcmp("bin", &filename[len - 3]) == 0)
		{
			if (this->readFromFileBIN(filename)>0)
				return true;
		}
		else if (strcmp("txt", &filename[len - 3]) == 0)
		{
			if (this->readFromFileASCII(filename)>0)
				return true;

		}
		else if (strcmp("mesh", &filename[len - 4]) == 0)
		{
			if (this->readFromMesh(filename))
				return true;
		}	
	}

	printf( "Error<CSurface<T>::readFromFile>: file name <%s> is illegal!\n", filename);
	return false;

}

template <class T>
int CSurface<T>::writeToFileASCII(const char* filename) const
{
	FILE *file = NULL;
	if( (fopen_s(&file, filename, "w")) != 0 )
	{
		printf("Error when Writing CSurface File <%s>\n", filename);
		return -1;
	}

	fprintf(file, "%d\n", this->vtDim);
	fprintf(file, "%d\n", int(this->normal));
	fprintf(file, "%d\n\n", int(this->color));

	fprintf(file, "%d\n", this->vtNum);
	for( int i=0; i<this->vtNum; i++)
	{
		for(int j=0; j<this->vtDim; j++)
			fprintf(file, "%f ", vtData[vtDim*i+j]);
		fprintf(file, "\n");
	}

	fprintf(file, "%d\n", this->triNum);
	for( int i=0; i<this->triNum; i++)
	{
		fprintf(file, "%d %d %d\n", triangles[3*i], triangles[3*i+1], triangles[3*i+2]);
	}

	if( colors_of_view.size() > 0 )
	{
		fprintf(file, "%d Views of color\n", colors_of_view.size());
		for(int viewIdx=0; viewIdx<colors_of_view.size(); viewIdx++)
		{
			fprintf(file, "Color from %d-th Views\n", viewIdx);
			float* colors = this->colors_of_view[viewIdx];
			if( colors != NULL )
			{
				for(int i=0; i<this->vtNum; i++)
				{
					fprintf(file, "%f %f %f\n", colors[3*i], colors[3*i+1], colors[3*i+2]);
				}
			}
		}
	}

	fclose(file);
	return 1;
}

template <class T>
bool CSurface<T>::writeToFilePLY(const char* filename, char* comments)
{
	FILE *file = NULL;
	if( (fopen_s(&file, filename, "w")) != 0 )
	{
		printf("Error when Writing File %s\n", filename);
		return false;
	}

	fprintf(file, "ply\n");
	fprintf(file, "format ascii 1.0\n");
	if( comments != NULL )
		fprintf(file, "comment %s\n", comments);
	
	fprintf(file, "element vertex %d\n", this->vtNum);
	fprintf(file, "property float x\n");
	fprintf(file, "property float y\n");
	fprintf(file, "property float z\n");
	if( this->haveNormalInfo() )
	{
		fprintf(file, "property float nx\n");
		fprintf(file, "property float ny\n");
		fprintf(file, "property float nz\n");
	}
	if( this->haveColorInfo() )
	{
		fprintf(file, "property uchar red\n");
		fprintf(file, "property uchar green\n");
		fprintf(file, "property uchar blue\n");
	}

	fprintf(file, "element face %d\n", this->triNum);
	fprintf(file, "property list uchar int vertex_indices\n");
	fprintf(file, "end_header\n");

	for(int i=0; i<this->vtNum; i++)
	{
		for(int j=0; j<this->vtDim-3; j++)
		{
			if( _isnan(this->vtData[i*vtDim+j]) )
				fprintf(file, "0 ");
			else
				fprintf(file, "%f ", this->vtData[i*vtDim+j]);
		}
		fprintf(file, "%d %d %d ", int(this->vtData[i*vtDim+vtDim-3]*255), int(this->vtData[i*vtDim+vtDim-2]*255),
								   int(this->vtData[i*vtDim+vtDim-1]*255));
		fprintf(file, "\n");
	}
	for(int i=0; i<this->triNum; i++)
	{
		fprintf(file, "3 %d %d %d\n", this->triangles[3*i], this->triangles[3*i+1], this->triangles[3*i+2]);
	}
	fclose(file);
	return true;
}

template <class T>
int CSurface<T>::readFromFileASCII(const char* filename)
{
	FILE *file = NULL;
	if( (fopen_s(&file, filename, "r")) != 0 )
	{
		printf("Error when Reading CSurface File<%s>\n", filename);
		return -1;
	}

	int isColor;
	int isNormal;

	fscanf(file, "%d\n", &vtDim);
	fscanf(file, "%d\n", &isNormal);
	fscanf(file, "%d\n", &isColor);

	if(isColor > 0)
		this->color = true;
	else
		this->color = false;
	if(isNormal > 0)
		this->normal = true;
	else
		this->normal = false;

	fscanf(file, "%d\n", &vtNum);
	if( this->vtData != NULL ) 
		delete [] vtData;
	vtData = new T[vtNum*vtDim];
	memset(vtData, 0, sizeof(T)*vtNum*vtDim);

	double tmp[9];
	for( int i=0; i<this->vtNum; i++)
	{
		char line[1000];
		fgets(line, 1000, file); //handle the situation when there are NANs in the file

		if( vtDim == 3 )
			sscanf(line, "%lf %lf %lf", &tmp[0], &tmp[1], &tmp[2]);
		else if( vtDim == 6)
			sscanf(line, "%lf %lf %lf %lf %lf %lf", &tmp[0], &tmp[1], &tmp[2],
													&tmp[3], &tmp[4], &tmp[5]);
		else if( vtDim == 9)
			sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &tmp[0], &tmp[1], &tmp[2],
																&tmp[3], &tmp[4], &tmp[5],
																&tmp[6], &tmp[7], &tmp[8]);
		else
			throw exception("Dim is not 3, 6, or 9!");

		for(int j=0; j<vtDim; j++)
			vtData[vtDim*i+j] = (T)tmp[j];

		//double val;
		//for(int j=0; j<vtDim; j++)
		//{
		//	fscanf(file, "%lf", &val);
		//	vtData[vtDim*i+j] = (T)val;
		//}
	}
	printf("%d vertices are read, ", this->vtNum);
	
	fscanf(file, "%d\n", &triNum);
	if( this->triangles != NULL) 
		delete [] triangles;
	triangles = new int[triNum*3];

	for( int i=0; i<this->triNum; i++)
	{
		fscanf(file, "%d %d %d\n", &triangles[3*i], &triangles[3*i+1], &triangles[3*i+2]);
	}
	printf("%d triangles are read, ", this->triNum);

	//release old memory
	for(int i=0; i<this->colors_of_view.size(); i++)
	{
		if( this->colors_of_view[i] != NULL )
			delete [] this->colors_of_view[i];
	}
	this->colors_of_view.clear();

	//read new data
	int viewNum = 0;
	if( !feof(file) )
	{
		fscanf(file, "%d Views of color\n", &viewNum);
		for(int viewIdx=0; viewIdx<viewNum; viewIdx++)
		{
			int id = 0;
			fscanf(file, "Color from %d-th Views\n", &id);
			float* colors = new float[3*this->vtNum];
			for(int i=0; i<this->vtNum; i++)
			{
				fscanf(file, "%f %f %f\n", &(colors[3*i]), &(colors[3*i+1]), &(colors[3*i+2]));
			}
			this->colors_of_view.push_back(colors);
		}
	}
	printf("%d views of color are read.\n", viewNum);

	fclose(file);
	return 1;
}

/*
int CSurface::writeToFileBIN(char *filename)
{
	ofstream file(filename, ofstream::binary);
	file.write((char*)(&vtnmNum), sizeof(int));
	file.write((char*)vtnmData, sizeof(float)*vtnmNum*6);
	file.write((char*)(&triNum), sizeof(int));
	file.write((char*)triangles, sizeof(int)*triNum*3);
	file.close();
	return 1;
}*/

template <class T>
int CSurface<T>::writeToFileBIN(const char *filename) const
{
	int numwritten;
	FILE *file = NULL;
	if( (fopen_s(&file, filename, "wb")) != 0 )
	{
		printf("Error<CSurface<T>::writeToFileBIN> when Writing Surface File %s\n", filename);
		return -1;
	}

	char type = '4';
	if( sizeof(T) == 8 )
		type = '8';
	numwritten = fwrite(&type, 1, 1, file);
	if( numwritten != 1 )
	{
		printf("Error when Writing Surface File\n");
		return -1;
	}	

	numwritten = fwrite(&vtDim, sizeof(int), 1, file);
	if( numwritten != 1 )
	{
		printf("Error when Writing Surface File\n");
		return -1;
	}

	int isNormal = int(normal);
	numwritten = fwrite(&isNormal, sizeof(int), 1, file);
	if( numwritten != 1 )
	{
		printf("Error when Writing Surface File\n");
		return -1;
	}

	int isColor = int(color);
	numwritten = fwrite(&isColor, sizeof(int), 1, file);
	if( numwritten != 1 )
	{
		printf("Error when Writing Surface File\n");
		return -1;
	}

	numwritten = fwrite(&vtNum, sizeof(int), 1, file);
	if( numwritten != 1 )
	{
		printf("Error when Writing Surface File\n");
		return -1;
	}
	
	numwritten = fwrite(vtData, sizeof(T), vtNum*vtDim, file);
	if( numwritten != vtNum*vtDim )
	{
		printf("Error when writing vertex data\n");
		return -1;
	}

	numwritten = fwrite(&triNum, sizeof(int), 1, file);
	if( numwritten != 1 )
	{
		printf("Error when Writing File\n");
		return -1;
	}

	numwritten = fwrite(triangles, sizeof(int), triNum*3, file);
	if( numwritten != triNum*3 )
	{
		printf("Error when Writing triangles\n");
		return -1;
	}

	if( this->colors_of_view.size() > 0)
	{
		int view_num = this->colors_of_view.size();
		fwrite(&view_num, sizeof(int), 1, file);
	}
	for(int i=0; i<this->colors_of_view.size(); i++)
	{
		float* colors = this->colors_of_view[i];
		if(colors != NULL )
		{
			numwritten = fwrite(colors, sizeof(float), vtNum*3, file);
			if( numwritten != vtNum*3 )
			{
				printf("Error when Writing colors of view %d\n", i);
				return -1;
			}
		}
	}

	fclose(file);
	printf("%d vertices, %d triangles, %d colors_of_view have been written into file<%s>\n", this->vtNum, this->triNum, this->colors_of_view.size(), filename);
	return 1;
}

/*
int CSurface::readFromFileBIN(char *filename)
{
	ifstream file(filename, ifstream::binary);
	
	file.read((char*)(&vtnmNum), sizeof(int));
	
	if( this->vtnmData != NULL ) delete [] vtnmData;
	vtnmData = new float[vtnmNum*6];
	file.read((char*)vtnmData, sizeof(float)*vtnmNum*6);

	file.read((char*)(&triNum), sizeof(int));
	if( this->triangles != NULL) delete [] triangles;
	triangles = new int[triNum*3];
	file.read((char*)triangles, sizeof(int)*triNum*3);

	file.close();	
	return 1;
}*/

template <class T>
int CSurface<T>::readFromFileBIN(const char *filename)
{
	char type_cur = '4';
	if( sizeof(T) == 8 )
		type_cur = '8';

	FILE *file = NULL;
	long numread=0;
	if( (fopen_s(&file, filename, "rb")) != 0 )
	{
		printf("Error<CSurface<T>::readFromFileBIN>: when Reading File1<%s>\n", filename);
		return -1;
	}
	char type;
	numread = fread(&type, 1, 1, file);
	if( numread != 1 )
	{
		printf("Error when Reading File2<%s>\n", filename);
		return -1;
	}

	printf("Reading Surface<%s>: ", filename);

	numread = fread(&vtDim, sizeof(int), 1, file);
	if( numread != 1 )
	{
		printf("Error when Reading File2<%s>\n", filename);
		return -1;
	}

	int isNormal;
	numread = fread(&isNormal, sizeof(int), 1, file);
	if( numread != 1 )
	{
		printf("Error when Reading File3\n");
		return -1;
	}
	if(isNormal > 0)
		normal = true;
	else
		normal = false;

	int isColor;
	numread = fread(&isColor, sizeof(int), 1, file);
	if( numread != 1 )
	{
		printf("Error when Reading File4\n");
		return -1;
	}
	if(isColor>0)
		color = true;
	else
		color = false;


	numread = fread(&vtNum, sizeof(int), 1, file);
	if( numread != 1 )
	{
		printf("Error when Reading File5\n");
		return -1;
	}

	long data_len_in_byte = 0;
	if( type == '4' )
		data_len_in_byte = vtNum*vtDim*4;
	else
		data_len_in_byte = vtNum*vtDim*8;
	char* tmp_buf = new char[data_len_in_byte];
	numread = fread(tmp_buf, 1, data_len_in_byte, file);
	if( numread != data_len_in_byte )
	{
		if(feof(file)!=0)
			printf("End of File\n");
		else if(ferror(file) != 0)
			printf("Error\n");
		else
			printf("Error when Reading vertex data\n");
		return -1;
	}
	printf("%d vertices,", vtNum);

	if( this->vtData != NULL ) delete [] vtData;
	if(type_cur == type)
	{
		this->vtData = (T*)tmp_buf;
	}
	else
	{
		vtData = new T[vtNum*vtDim];
		if( type == '4' )
		{
			float *buf = (float*) tmp_buf;
			for(int i=0; i<vtNum*vtDim; i++)
				vtData[i] = buf[i];
		}
		else
		{
			double *buf = (double*) tmp_buf;
			for(int i=0; i<vtNum*vtDim; i++)
				vtData[i] = buf[i];
		}
		delete [] tmp_buf;
	}

	numread = fread(&triNum, sizeof(int), 1, file);
	if( numread != 1 )
	{
		printf("Error when Reading File\n");
		return -1;
	}

	if( this->triangles != NULL) delete [] triangles;
	triangles = new int[triNum*3];
	numread = fread(triangles, sizeof(int), triNum*3, file);
	if( numread != triNum*3 )
	{
		printf("Error when Reading triangles\n");
		return -1;
	}
	printf("%d triangles,", triNum);


	for(int i=0; i<this->colors_of_view.size(); i++)
	{
		if( this->colors_of_view[i] != NULL )
			delete [] this->colors_of_view[i];
	}
	this->colors_of_view.clear();

	//read colors_of_view if necessary
	int view_num = 0;
	fread(&view_num, sizeof(int), 1, file);
	if( !feof(file) )
	{
		for(int i=0; i<view_num; i++)
		{
			float *colors = new float[3*this->vtNum];
			numread = fread(colors, sizeof(float), this->vtNum*3, file);
			if( numread != this->vtNum*3 )
			{
				printf("Error when Reading colors of view %d\n", i);
				return -1;
			}
			this->colors_of_view.push_back(colors);
		}
		printf("%d views of color\n", view_num);
	}
	else
	{
		printf(" are read!\n");
	}

	fclose(file);
	return 1;
}

template <class T>
bool CSurface<T>::readFromMesh(const char* filename)
{
	FILE *file = NULL;
	if ((fopen_s(&file, filename, "r")) != 0)
	{
		printf("Error<CSurface<T>::readFromMesh>: when Reading File<%s>\n", filename);
		return false;
	}

	double version = 0;
	fscanf(file, "MeshVersionFormatted %lf\n\n", &version);
	int dim = 3;
	fscanf(file, "Dimension\n%d\n\n", &dim);

	int vtNum = 0;
	fscanf(file, "Vertices\n%d\n", &vtNum);

	if (vtNum > 0)
		this->freeMemory();

	this->vtNum = vtNum;
	this->vtDim = 3;
	this->vtData = new T[vtNum*3];
	for (int i = 0; i < vtNum; i++)
	{
		double zero;
		double v1, v2, v3;
		fscanf(file, "%lf %lf %lf %lf\n", &v1, &v2, &v3, &zero);
		this->vtData[3 * i] = v1;
		this->vtData[3 * i+1] = v2;
		this->vtData[3 * i+2] = v3;
	}

	fscanf(file, "\nTriangles\n%d\n", &this->triNum);
	this->triangles = new int[3 * this->triNum];
	for (int i = 0; i < triNum; i++)
	{
		int zero;
		int idx1, idx2, idx3;
		fscanf(file, "%d %d %d %d\n", &idx1, &idx2, &idx3, &zero);
		this->triangles[3 * i] = idx1 - 1;
		this->triangles[3 * i+1] = idx2 - 1;
		this->triangles[3 * i+2] = idx3 - 1;
	}

	fclose(file);
	return true;
}


template <class T>
void CSurface<T>::generateNormals()
{
	if(haveNormalInfo())
	{
		printf("Error: normals alread exist!\n");
		return;
	}

	if( this->triangles == 0 || this->vtNum == 0)
	{
		printf("Error: No triangle data or no vertex data!\n");
		return;
	}

	T *normalData = new T[this->vtNum*3];
	memset(normalData, 0, sizeof(T)*vtNum*3);
	int *adjacentFacetNum = new int[this->vtNum];
	memset(adjacentFacetNum, 0, sizeof(int)*vtNum);

	//update the normal of each vertex
	for(int i=0; i<this->triNum; i++)
	{
		int idx1 = this->triangles[i*3];
		int idx2 = this->triangles[i*3+1];
		int idx3 = this->triangles[i*3+2];

		T* p1 = &vtData[idx1*vtDim];
		T* p2 = &vtData[idx2*vtDim];
		T* p3 = &vtData[idx3*vtDim];
		
		T e1[3];
		T e2[3];
		T normal[3];

		VecOperation<T>::VecSub(p3, p1, e1, 3);
		VecOperation<T>::VecSub(p2, p1, e2, 3);
		VecOperation<T>::CrossProdVec3(e1, e2, normal);
		VecOperation<T>::Unitalization(normal, 3);

		if( adjacentFacetNum[idx1] > 0 )
		{
			T *prev_normal = &normalData[idx1*3];
			//if(VecOperation<T>::DotProd(normal, prev_normal, 3) > 0)
				VecOperation<T>::VecAdd(normal, prev_normal, prev_normal, 3);
			//else
			//	VecOperation<T>::VecSub(prev_normal, normal, prev_normal, 3 );
			adjacentFacetNum[idx1] ++;
		}
		else
		{
			T *prev_normal = &normalData[idx1*3];
			VecOperation<T>::VecCopy(normal, prev_normal, 3);
			adjacentFacetNum[idx1] = 1;
		}
		
		if( adjacentFacetNum[idx2] > 0 )
		{
			T *prev_normal = &normalData[idx2*3];
			//if(VecOperation<T>::DotProd(normal, prev_normal, 3) > 0)
				VecOperation<T>::VecAdd(normal, prev_normal, prev_normal, 3);
			//else
			//	VecOperation<T>::VecSub(prev_normal, normal, prev_normal, 3);
			adjacentFacetNum[idx2] ++;
		}
		else
		{
			T *prev_normal = &normalData[idx2*3];
			VecOperation<T>::VecCopy(normal, prev_normal, 3);
			adjacentFacetNum[idx2] = 1;
		}

		if( adjacentFacetNum[idx3] > 0 )
		{
			T *prev_normal = &normalData[idx3*3];
			//if(VecOperation<T>::DotProd(normal, prev_normal, 3) > 0)
				VecOperation<T>::VecAdd(normal, prev_normal, prev_normal, 3);
			//else
			//	VecOperation<T>::VecSub(prev_normal, normal, prev_normal, 3 );
			adjacentFacetNum[idx3] ++;
		}
		else
		{
			T *prev_normal = &normalData[idx3*3];
			VecOperation<T>::VecCopy(normal, prev_normal, 3);
			adjacentFacetNum[idx3] = 1;
		}
	}

	//unitilize the normal vector
	for(int i=0; i<this->vtNum; i++)
	{
		T* cur_normal = &normalData[i*3];
		VecOperation<T>::Unitalization(cur_normal, 3);
	}

	//combine vertex data with normal data
	int new_vtDim = 3 + this->vtDim;
	T *new_vtData = new T[new_vtDim*vtNum];
	for(int i=0; i<vtNum; i++)
	{
		new_vtData[i*new_vtDim] = this->vtData[i*vtDim];
		new_vtData[i*new_vtDim+1] = this->vtData[i*vtDim+1];
		new_vtData[i*new_vtDim+2] = this->vtData[i*vtDim+2];
		new_vtData[i*new_vtDim+3] = normalData[3*i];
		new_vtData[i*new_vtDim+4] = normalData[3*i+1];
		new_vtData[i*new_vtDim+5] = normalData[3*i+2];
	}

	if(this->haveColorInfo())
	{
		//copy color info
		for(int i=0; i<this->vtNum; i++)
		{
			new_vtData[i*new_vtDim+6] = this->vtData[i*vtDim+3];
			new_vtData[i*new_vtDim+7] = this->vtData[i*vtDim+4];
			new_vtData[i*new_vtDim+8] = this->vtData[i*vtDim+5];
		}
	}

	delete [] this->vtData;
	this->vtData = new_vtData;
	this->vtDim = new_vtDim;
	this->normal = true;

	delete [] normalData;
	delete [] adjacentFacetNum;
}

template <class T>
void CSurface<T>::expand_data(bool bExpandColor, bool bExpandNormal)
{
	int vtDim_new = this->vtDim;
	if( bExpandColor && !this->haveColorInfo() )
		vtDim_new += 3;
	if( bExpandNormal && !this->haveNormalInfo() )
		vtDim_new += 3;

	if( vtDim_new == this->vtDim )
		return;

	T *vtData_new = new T[this->vtNum*vtDim_new];
	memset(vtData_new, 0, sizeof(T)*this->vtNum*vtDim_new);
	for(int i=0; i<this->vtNum; i++)
	{
		vtData_new[i*vtDim_new] = vtData[i*vtDim];
		vtData_new[i*vtDim_new+1] = vtData[i*vtDim+1];
		vtData_new[i*vtDim_new+2] = vtData[i*vtDim+2];

		if(this->haveNormalInfo())
		{
			vtData_new[i*vtDim_new+3] = vtData[i*vtDim+3];
			vtData_new[i*vtDim_new+4] = vtData[i*vtDim+4];
			vtData_new[i*vtDim_new+5] = vtData[i*vtDim+5];
		}
		if(this->haveNormalInfo())
		{
			vtData_new[(i+1)*vtDim_new-3] = vtData[(i+1)*vtDim-3];
			vtData_new[(i+1)*vtDim_new-2] = vtData[(i+1)*vtDim-2];
			vtData_new[(i+1)*vtDim_new-1] = vtData[(i+1)*vtDim-1];
		}
	}

	delete [] this->vtData;
	this->vtData = vtData_new;
	this->vtDim = vtDim_new;
	this->color = (bExpandColor || this->haveColorInfo());
	this->normal = (bExpandNormal || this->haveNormalInfo());
}

template <class T>
bool CSurface<T>::updateNormals()
{
	if(!haveNormalInfo())
	{
		printf("Error: normals do not exist!\n");
		return false;
	}

	if( this->triangles == 0 || this->vtNum == 0)
	{
		printf("Error: No triangle data or no vertex data!\n");
		return false;
	}

	int *adjacentFacetNum = new int[this->vtNum];
	memset(adjacentFacetNum, 0, sizeof(int)*vtNum);

	//update the normal of each vertex
	for(int i=0; i<this->triNum; i++)
	{
		int idx1 = this->triangles[i*3];
		int idx2 = this->triangles[i*3+1];
		int idx3 = this->triangles[i*3+2];

		T* p1 = &vtData[idx1*vtDim];
		T* p2 = &vtData[idx2*vtDim];
		T* p3 = &vtData[idx3*vtDim];
		
		T e1[3];
		T e2[3];
		T normal[3];

		VecOperation<T>::VecSub(p3, p1, e1, 3);
		VecOperation<T>::VecSub(p2, p1, e2, 3);
		VecOperation<T>::CrossProdVec3(e1, e2, normal);
		//VecOperation<T>::Unitalization(normal, 3);

		if( adjacentFacetNum[idx1] > 0 )
		{
			T *prev_normal = this->vt_normal(idx1);
			VecOperation<T>::VecAdd(normal, prev_normal, prev_normal, 3);
			adjacentFacetNum[idx1] ++;
		}
		else
		{
			T *prev_normal = this->vt_normal(idx1);
			VecOperation<T>::VecCopy(normal, prev_normal, 3);
			adjacentFacetNum[idx1] = 1;
		}
		
		if( adjacentFacetNum[idx2] > 0 )
		{
			T *prev_normal = this->vt_normal(idx2);
			VecOperation<T>::VecAdd(normal, prev_normal, prev_normal, 3);
			adjacentFacetNum[idx2] ++;
		}
		else
		{
			T *prev_normal = this->vt_normal(idx2);
			VecOperation<T>::VecCopy(normal, prev_normal, 3);
			adjacentFacetNum[idx2] = 1;
		}

		if( adjacentFacetNum[idx3] > 0 )
		{
			T *prev_normal = this->vt_normal(idx3);
			VecOperation<T>::VecAdd(normal, prev_normal, prev_normal, 3);
			adjacentFacetNum[idx3] ++;
		}
		else
		{
			T *prev_normal = this->vt_normal(idx3);
			VecOperation<T>::VecCopy(normal, prev_normal, 3);
			adjacentFacetNum[idx3] = 1;
		}
	}

	//unitilize the normal vector
	for(int i=0; i<this->vtNum; i++)
	{
		T* cur_normal = this->vt_normal(i);
		VecOperation<T>::Unitalization(cur_normal, 3);
	}

	delete [] adjacentFacetNum;
	return true;
}

template <class T>
void CSurface<T>::centralize()
{
	double x_cen = 0.0;
	double y_cen = 0.0;
	double z_cen = 0.0;
	for(int i=0; i<this->vtNum; i++)
	{
		x_cen += this->vtData[i*this->vtDim];
		y_cen += this->vtData[i*this->vtDim+1];
		z_cen += this->vtData[i*this->vtDim+2];
	}
	x_cen /= this->vtNum;
	y_cen /= this->vtNum;
	z_cen /= this->vtNum;

	for(int i=0; i<this->vtNum; i++)
	{
		this->vtData[i*this->vtDim  ] -= x_cen;
		this->vtData[i*this->vtDim+1] -= y_cen;
		this->vtData[i*this->vtDim+2] -= z_cen;
	}
}

template <class T>
double* CSurface<T>::get_center() const
{
	double x_cen = 0.0;
	double y_cen = 0.0;
	double z_cen = 0.0;
	for(int i=0; i<this->vtNum; i++)
	{
		x_cen += this->vtData[i*this->vtDim];
		y_cen += this->vtData[i*this->vtDim+1];
		z_cen += this->vtData[i*this->vtDim+2];
	}
	x_cen /= this->vtNum;
	y_cen /= this->vtNum;
	z_cen /= this->vtNum;

	double *ret = new double[3];
	ret[0] = x_cen;
	ret[1] = y_cen;
	ret[2] = z_cen;

	return ret;
}

template <class T>
BoundingBox3D CSurface<T>::get_bbox() const
{
	if( this->vtNum == 0)
		return BoundingBox3D(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	double x_min = this->vtData[0];
	double x_max = this->vtData[0];
	double y_min = this->vtData[1];
	double y_max = this->vtData[1];
	double z_min = this->vtData[2];
	double z_max = this->vtData[2];

	for(int i=1; i<this->vtNum; i++)
	{
		T const* vt = this->vt_data_block(i);
		if( x_min > vt[0] )
			x_min = vt[0];
		if( x_max < vt[0] )
			x_max = vt[0];
		if( y_min > vt[1] )
			y_min = vt[1];
		if( y_max < vt[1] )
			y_max = vt[1];
		if( z_min > vt[2] )
			z_min = vt[2];
		if( z_max < vt[2] )
			z_max = vt[2];
	}
	return BoundingBox3D(x_min, x_max, y_min, y_max, z_min, z_max);
}

template <class T>
bool CSurface<T>::copy_texture_from_other( CSurface<T> const& other)
{
	if( !other.haveColorInfo() )
	{
		printf("Warning: Other surface has no color info!\n");
		return false;
	}
	if( !this->haveColorInfo() )
	{
		printf("Warning: current surface has no color infor!\n");
		return false;
	}

	if( this->vtNum != other.vtNum )
	{
		printf("Warning: surface vertex numbers are not equal<%d, %d>!\n", this->vtNum, other.vtNum);
		return false;
	}

	for(int vtIdx=0; vtIdx<this->vtNum; vtIdx++)
	{
		T* clr_dst = this->vt_color(vtIdx);
		T const* clr_src = other.vt_color(vtIdx);
		clr_dst[0] = clr_src[0];
		clr_dst[1] = clr_src[1];
		clr_dst[2] = clr_src[2];
	}
	
	//copy colors_of_view
	if( other.colors_of_view.size() != 0 )
	{
		int view_num = other.colors_of_view.size();
		
		//allocate memory if necessary
		if( this->colors_of_view.size() == 0 )
		{
			for(int i=0; i<view_num; i++)
			{
				float* view_color = new float[this->vtNum*3];
				this->colors_of_view.push_back(view_color);
			}
		}

		//copy colors
		for(int i=0; i<view_num; i++)
			memcpy(this->colors_of_view[i], other.colors_of_view[i], sizeof(float)*this->vtNum*3);
	}
}

template <class T>
bool CSurface<T>::create_and_init_colors_of_view(int viewNum, bool bInitAsVertexColor)
{
	if( this->colors_of_view.size() != 0)
	{
		printf("Warning<CSurface<T>::create_and_init_colors_of_view>: colors_of_view already exist!");
		return false;
	}

	for(int i=0; i<viewNum; i++)
	{
		float* view_colors = new float[this->vtNum*3];
		memset(view_colors, 0, sizeof(float)*this->vtNum*3);
		if( bInitAsVertexColor && this->haveColorInfo() )
		{
			for(int vtIdx=0; vtIdx<this->vtNum; vtIdx++)
			{
				T const*clr = this->vt_color(vtIdx);
				view_colors[vtIdx*3] = clr[0];
				view_colors[vtIdx*3+1] = clr[1];
				view_colors[vtIdx*3+2] = clr[2];
			}
		}
		this->colors_of_view.push_back(view_colors);
	}
}

template <class T>
double* CSurface<T>::get_avg_normal()
{
	if( !this->haveNormalInfo() )
		this->generateNormals();

	double dx_avg = 0.0;
	double dy_avg = 0.0;
	double dz_avg = 0.0;
	for(int i=0; i<this->vtNum; i++)
	{
		dx_avg += this->vtData[i*this->vtDim+3];
		dy_avg += this->vtData[i*this->vtDim+4];
		dz_avg += this->vtData[i*this->vtDim+5];
	}
	dx_avg /= this->vtNum;
	dy_avg /= this->vtNum;
	dz_avg /= this->vtNum;

	double *ret = new double[3];
	ret[0] = dx_avg;
	ret[1] = dy_avg;
	ret[2] = dz_avg;
	return ret;
}

template <class T>
void CSurface<T>::delete_duplicate_vertices()
{
// check vertex duplication for each triangle and remove the triangle with vertex duplication
// change the vtIdx for the duplicated vertex 
	map<int, int> duplications; // first-->second
	vector<int> triangles_new;
	for (int i = 0; i < triNum; i++)
	{
		int vtIdx0 = triangles[3 * i];
		int vtIdx1 = triangles[3 * i+1];
		int vtIdx2 = triangles[3 * i+2];

		const T* vt0 = this->vt_data_block(vtIdx0);
		const T* vt1 = this->vt_data_block(vtIdx1);
		const T* vt2 = this->vt_data_block(vtIdx2);

		int duplication_num = 0;
		if (vt0[0] == vt1[0] && vt0[1] == vt1[1] && vt0[2] == vt1[2])
		{
			int vtIdx_i = min(vtIdx0, vtIdx1);
			int vtIdx_j = max(vtIdx0, vtIdx1);

			map<int, int>::iterator iter = duplications.find(vtIdx_i);
			if (iter == duplications.end())
				duplications[vtIdx_i] = vtIdx_j;
			duplication_num++;
		}

		if (vt0[0] == vt2[0] && vt0[1] == vt2[1] && vt0[2] == vt2[2])
		{
			int vtIdx_i = min(vtIdx0, vtIdx2);
			int vtIdx_j = max(vtIdx0, vtIdx2);
			map<int, int>::iterator iter = duplications.find(vtIdx_i);
			if (iter == duplications.end())
				duplications[vtIdx_i] = vtIdx_j;

			duplication_num++;
		}

		if (vt1[0] == vt2[0] && vt1[1] == vt2[1] && vt1[2] == vt2[2])
		{
			int vtIdx_i = min(vtIdx1, vtIdx2);
			int vtIdx_j = max(vtIdx1, vtIdx2);
			map<int, int>::iterator iter = duplications.find(vtIdx_i);
			if (iter == duplications.end())
				duplications[vtIdx_i] = vtIdx_j;

			duplication_num++;
		}

		if (duplication_num == 0)
		{
			triangles_new.push_back(vtIdx0);
			triangles_new.push_back(vtIdx1);
			triangles_new.push_back(vtIdx2);
		}
	}

	delete[] this->triangles;
	this->triangles = new int[triangles_new.size()];
	this->triNum = triangles_new.size() / 3;
	for (int i = 0; i<triangles_new.size(); i++)
		this->triangles[i] = triangles_new[i];

	for (int i = 0; i < triNum; i++)
	{
		int vtIdx0 = triangles[3 * i];
		int vtIdx1 = triangles[3 * i + 1];
		int vtIdx2 = triangles[3 * i + 2];

		map<int, int>::iterator iter = duplications.find(vtIdx0);
		if (iter != duplications.end())
			triangles[3 * i] = iter->second;

		iter = duplications.find(vtIdx1);
		if (iter != duplications.end())
			triangles[3 * i + 1] = iter->second;

		iter = duplications.find(vtIdx2);
		if (iter != duplications.end())
			triangles[3 * i + 2] = iter->second;
	}

	this->delete_extra_vertice();
}

	//delete vertex
template <class T>
void CSurface<T>::delete_vertices( vector<int> const& vt_indices)
{
	vector<bool> bFlag(this->vtNum, true); //true--kept; false--delete
	for(int i=0; i<vt_indices.size(); i++)
	{
		int vtIdx = vt_indices[i];
		if( vtIdx < this->vtNum )
			bFlag[vtIdx] = false;
	}

	//delete triangles
	vector<int> triangles_new;
	for(int i=0; i<this->triNum; i++)
	{
		int idx1 = triangles[3*i];
		int idx2 = triangles[3*i+1];
		int idx3 = triangles[3*i+2];
		if( !bFlag[idx1] || !bFlag[idx2] || !bFlag[idx3] )
			continue;
		triangles_new.push_back(idx1);
		triangles_new.push_back(idx2);
		triangles_new.push_back(idx3);
	}
	delete [] this->triangles;
	this->triangles = new int[triangles_new.size()];
	this->triNum = triangles_new.size()/3;
	for(int i=0; i<triangles_new.size(); i++)
		this->triangles[i] = triangles_new[i];

	this->delete_extra_vertice();
}

template <class T>
CSurface<T>* CSurface<T>::extract_surface( vector<int> const& vt_indices) const
{
	CSurface<T> *ret = new CSurface<T>();
	vector<bool> bFlag(this->vtNum, false); //true--kept; false--delete
	int vtNum_ret = 0;
	for(int i=0; i<vt_indices.size(); i++)
	{
		int vtIdx = vt_indices[i];
		if( vtIdx < this->vtNum )
		{
			bFlag[vtIdx] = true;
			vtNum_ret++;
		}
	}

	if( vtNum_ret==0 )
	{
		printf("Warning<extract_surface>: empty vertex!\n");
		return ret;
	}

	//copy vtData
	T* vtData_ret = new T[this->vtDim * vtNum_ret];
	int count = 0;
	for(int i=0; i<bFlag.size(); i++)
	{
		if( bFlag[i] )
		{
			memcpy(&(vtData_ret[count*vtDim]), &(vtData[i*vtDim]), sizeof(T)*vtDim);
			count++;
		}
	}

	//build the lookup table: old Idx-->new idx
	vector<int> idx_LUT(this->vtNum, -1);
	count = 0;
	for(int i=0; i<this->vtNum; i++)
	{
		if( bFlag[i] )
		{
			idx_LUT[i] = count;
			count++;
		}
	}

	//get the triangles for ret
	vector<int> triangles_vec_ret;
	int triNum_ret = 0;
	for(int i=0; i<this->triNum; i++)
	{
		int idx1 = this->triangles[3*i];
		int idx2 = this->triangles[3*i+1];
		int idx3 = this->triangles[3*i+2];

		if( bFlag[idx1] && bFlag[idx2] && bFlag[idx3] )
		{
			triangles_vec_ret.push_back(idx_LUT[idx1]);
			triangles_vec_ret.push_back(idx_LUT[idx2]);
			triangles_vec_ret.push_back(idx_LUT[idx3]);
			triNum_ret++;
		}
	}
	int *triangles_ret = new int[triNum_ret*3];
	for(int i=0; i<triNum_ret*3; i++)
		triangles_ret[i] = triangles_vec_ret[i];

	ret->normal = this->normal;
	ret->color = this->color;
	ret->vtDim = this->vtDim;
	ret->vtNum = vtNum_ret;
	ret->vtData = vtData_ret;
	ret->triNum = triNum_ret;
	ret->triangles = triangles_ret;
	return ret;	
}

template <class T>	
void CSurface<T>::delete_vertices_inside_box( BoundingBox3D bbox )
{
	vector<int> vertices_to_delete;
	delete_vertices_inside_box(bbox, vertices_to_delete);
}

template <class T>	
void CSurface<T>::delete_vertices_inside_box( BoundingBox3D bbox, vector<int> &vertices_to_delete )
{
	vertices_to_delete.clear();
	for(int i=0; i<this->vtNum; i++)
	{
		T* vt = this->vt_data_block(i);
		if( vt[0] > bbox.x_s && vt[0] < bbox.x_e &&
			vt[1] > bbox.y_s && vt[1] < bbox.y_e &&
			vt[2] > bbox.z_s && vt[2] < bbox.z_e )
		{
			vertices_to_delete.push_back(i);
		}
	}

	this->delete_vertices(vertices_to_delete);
}

template <class T>
void CSurface<T>::delete_vertices_outside_box( BoundingBox3D bbox )
{
	vector<int> vertices_to_delete;
	delete_vertices_outside_box(bbox, vertices_to_delete);
}

template <class T>
void CSurface<T>::delete_vertices_outside_box( BoundingBox3D bbox, vector<int> &vertices_to_delete )
{
	vertices_to_delete.clear();
	for(int i=0; i<this->vtNum; i++)
	{
		T* vt = this->vt_data_block(i);
		if( vt[0] < bbox.x_s || vt[0] > bbox.x_e ||
			vt[1] < bbox.y_s || vt[1] > bbox.y_e ||
			vt[2] < bbox.z_s || vt[2] > bbox.z_e )
		{
			vertices_to_delete.push_back(i);
		}
	}

	this->delete_vertices(vertices_to_delete);
}

template <class T>
void CSurface<T>::delete_extra_vertice()
{
	vector<int> bFlag(this->vtNum);
	for(int i=0; i<this->vtNum; i++)
		bFlag[i] = 0;

	for(int i=0; i<this->triNum*3; i++)
	{
		int vtIdx = this->triangles[i];
		bFlag[vtIdx] = 1;
	}
	
	//build the lookup table: old Idx-->new idx
	vector<int> idx_LUT(this->vtNum);
	for(int i=0; i<this->vtNum; i++)
		idx_LUT[i] = -1;

	int count = 0;
	for(int i=0; i<this->vtNum; i++)
	{
		if( bFlag[i] )
		{
			idx_LUT[i] = count;
			count++;
		}
	}

	if( count==0 )
	{
		printf("Error<delete_extra_vertice>: empty vertex!\n");
		return;
	}

	T* vtData_new = new T[count*this->vtDim];
	for(int i=0; i<this->vtNum; i++)
	{
		if( !bFlag[i] )
			continue;
		int vtIdx_new = idx_LUT[i];
		assert(vtIdx_new>=0);
		memcpy(&(vtData_new[vtIdx_new*this->vtDim]), &(this->vtData[i*this->vtDim]), sizeof(T)*this->vtDim);
	}
	if( this->vtData != NULL)
		delete [] this->vtData;

	//copy the data
	vector<float*> colors_of_view_new;
	for(int viewIdx=0; viewIdx<this->colors_of_view.size(); viewIdx++)
	{
		float *colors_old = this->colors_of_view[viewIdx];
		float *colors_new = NULL;
		if( colors_old != NULL )
		{
			colors_new = new float[count*3];
			for(int i=0; i<this->vtNum; i++)
			{
				if( !bFlag[i] )
					continue;
				int vtIdx_new = idx_LUT[i];
				assert(vtIdx_new>=0);
				colors_new[3*vtIdx_new]   = colors_old[3*i];
				colors_new[3*vtIdx_new+1] = colors_old[3*i+1];
				colors_new[3*vtIdx_new+2] = colors_old[3*i+2];
			}
		}
		colors_of_view_new.push_back(colors_new);
	}
	//delete the old colors_of_view
	for(int viewIdx=0; viewIdx<this->colors_of_view.size(); viewIdx++)
	{
		if( this->colors_of_view[viewIdx] != NULL )
			delete [] this->colors_of_view[viewIdx];
	}
	printf2(FGI_, "Info<delete_extra_vertice>: %d vertice deleted!\n", this->vtNum-count);

	this->vtData = vtData_new;
	this->vtNum = count;
	this->colors_of_view = colors_of_view_new;
	for(int i=0; i<this->triNum*3; i++)
	{
		int vtIdx_old = this->triangles[i];
		int vtIdx_new = idx_LUT[vtIdx_old];
		assert(vtIdx_new != -1);
		this->triangles[i] = vtIdx_new;
	}
}

template <class T>
void CSurface<T>::extract_boundary()
{
	typedef map<int, bool> VtHashTable; //int--the index of vt that is connected to the vertex currently being examined, bool-indicate whether the edge show up twice
	vector< VtHashTable > vt_lists(this->vtNum);
	for(int i=0; i<this->triNum; i++)
	{
		int vtIdx1 = this->triangles[3*i];
		int vtIdx2 = this->triangles[3*i+1];
		int vtIdx3 = this->triangles[3*i+2];

		if( vtIdx1 < vtIdx2 )
		{
			VtHashTable::iterator iter = vt_lists[vtIdx1].find(vtIdx2);
			if( iter != vt_lists[vtIdx1].end() )
				iter->second = true;
			else
				vt_lists[vtIdx1].insert( make_pair(vtIdx2, false) );
		}
		else
		{
			VtHashTable::iterator iter = vt_lists[vtIdx2].find(vtIdx1);
			if( iter != vt_lists[vtIdx2].end() )
				iter->second = true;
			else
				vt_lists[vtIdx2].insert( make_pair(vtIdx1, false) );
		}

		if( vtIdx1 < vtIdx3 )
		{
			VtHashTable::iterator iter = vt_lists[vtIdx1].find(vtIdx3);
			if( iter != vt_lists[vtIdx1].end() )
				iter->second = true;
			else
				vt_lists[vtIdx1].insert( make_pair(vtIdx3, false) );
		}
		else
		{
			VtHashTable::iterator iter = vt_lists[vtIdx3].find(vtIdx1);
			if( iter != vt_lists[vtIdx3].end() )
				iter->second = true;
			else
				vt_lists[vtIdx3].insert( make_pair(vtIdx1, false) );
		}

		if( vtIdx2 < vtIdx3 )
		{
			VtHashTable::iterator iter = vt_lists[vtIdx2].find(vtIdx3);
			if( iter != vt_lists[vtIdx2].end() )
				iter->second = true;
			else
				vt_lists[vtIdx2].insert( make_pair(vtIdx3, false) );
		}
		else
		{
			VtHashTable::iterator iter = vt_lists[vtIdx3].find(vtIdx2);
			if( iter != vt_lists[vtIdx3].end() )
				iter->second = true;
			else
				vt_lists[vtIdx3].insert( make_pair(vtIdx2, false) );
		}
	}

	//get the boundary edges
	this->boundary_edges.clear();
	for(int vtIdx=0; vtIdx<this->vtNum; vtIdx++)
	{
		VtHashTable &vt_list = vt_lists[vtIdx];
		for(VtHashTable::iterator iter = vt_list.begin(); iter != vt_list.end(); iter++)
		{
			if( iter->second == false )
			{
				this->boundary_edges.push_back(make_pair(vtIdx, iter->first));
			}
		}
	}

	//get the boundary vts
	vector<bool> bFlags(this->vtNum);
	for(int i=0; i<this->vtNum; i++)
		bFlags[i] = false;
	for(int i=0; i<this->boundary_edges.size(); i++)
	{
		int vtIdx1 = this->boundary_edges[i].first;
		int vtIdx2 = this->boundary_edges[i].second;
		bFlags[vtIdx1] = true;
		bFlags[vtIdx2] = true;
	}
	this->bBoundary = bFlags;
	this->boundary_vts.clear();
	for(int i=0; i<this->vtNum; i++)
	{
		if( bFlags[i] )
			this->boundary_vts.push_back(i);
	}
}

#ifdef USING_GLMOBJ
template <class T>
int CSurface<T>::readFromObj(const char *filename)
{
	GLMmodel *obj_model = glmReadOBJ(filename);
	if(obj_model == NULL)
		return -1;
	
	int ret = readFromObj(obj_model);
	glmDelete(obj_model);
	return ret;
}

template <class T>
int CSurface<T>::readFromObj(GLMmodel *obj_model)
{
	if( obj_model->numvertices != obj_model->numnormals )
	{
		printf("Warning: The vertex num does not equal to normal num! Only the vertex is read!\n");
		this->freeMemory();
		
		this->vtDim = 3;
		this->color = false;
		this->normal = false;
		this->vtNum = obj_model->numvertices;
		this->vtData = new T[vtNum*3];
		for( int i=0; i<vtNum; i++ )
		{
			/* fetch from index 1 */
			vtData[3*i] = (T)obj_model->vertices[3*(i+1)];
			vtData[3*i+1] = (T)obj_model->vertices[3*(i+1)+1];
			vtData[3*i+2] = (T)obj_model->vertices[3*(i+1)+2];
		}

		this->triNum = obj_model->numtriangles;
		this->triangles = new int[triNum*3];
		for( int i=0; i<triNum; i++)
		{
			//note that OBJ file counts from 1, instead of 0
			this->triangles[3*i] = obj_model->triangles[i].vindices[0] - 1;
			this->triangles[3*i+1] = obj_model->triangles[i].vindices[1] - 1;
			this->triangles[3*i+2] = obj_model->triangles[i].vindices[2] - 1;
		}

		return 2;
	}

	this->freeMemory();
	//============ the following code is based on the assumption that ============
	//============ the i-th vertex corresponds to i-th normal	=================
	this->vtDim = 6;
	this->color = false;
	this->normal = true;
	this->vtNum = obj_model->numvertices;
	this->vtData = new T[vtNum*6];
	for( int i=0; i<vtNum; i++ )
	{
		/* fetch from index 1 */
		vtData[6*i] = (T)obj_model->vertices[3*(i+1)];
		vtData[6*i+1] = (T)obj_model->vertices[3*(i+1)+1];
		vtData[6*i+2] = (T)obj_model->vertices[3*(i+1)+2];

		vtData[6*i+3] = (T)obj_model->normals[3*(i+1)];
		vtData[6*i+4] = (T)obj_model->normals[3*(i+1)+1];
		vtData[6*i+5] = (T)obj_model->normals[3*(i+1)+2];
	}

	this->triNum = obj_model->numtriangles;
	this->triangles = new int[triNum*3];
	for( int i=0; i<triNum; i++)
	{
		//note that OBJ file counts from 1, instead of 0
		this->triangles[3*i] = obj_model->triangles[i].vindices[0] - 1;
		this->triangles[3*i+1] = obj_model->triangles[i].vindices[1] - 1;
		this->triangles[3*i+2] = obj_model->triangles[i].vindices[2] - 1;
	}

	return 1;
}

template <class T>
void CSurface<T>::writeTOFileOBJ(const char* filename)
{
	GLMmodel *obj_model = convertToObj();
	glmWriteOBJ(obj_model, (char*)filename, GLM_SMOOTH);

	glmDelete(obj_model);
}

template <class T>
GLMmodel* CSurface<T>::convertToObj()
{
	GLMmodel* obj_model = new GLMmodel();
	obj_model->pathname = NULL;
	obj_model->mtllibname = NULL;
	
	obj_model->numvertices = this->vtNum;
	float* vert = new float[this->vtNum*3+3];
	for(int i=1; i<=this->vtNum; i++)
	{
		vert[3*i] = this->vtData[(i-1)*vtDim];//waterSurface->m_ppt3dVertices[i-1][0];
		vert[3*i+1] = this->vtData[(i-1)*vtDim+1];//GRIDHEIGHT-1-waterSurface->m_ppt3dVertices[i-1][1];
		vert[3*i+2] = this->vtData[(i-1)*vtDim+2];//-waterSurface->m_ppt3dVertices[i-1][2];
	}
	obj_model->vertices = vert;

	if(haveNormalInfo())
	{
		obj_model->numnormals = this->vtNum;
		float* norm = new float[this->vtNum*3+3];
		for(int i=1; i<=this->vtNum; i++)
		{
			norm[3*i] = this->vtData[(i-1)*vtDim+3];//waterSurface->m_pvec3dNormals[i-1][0];
			norm[3*i+1] = this->vtData[(i-1)*vtDim+4];//waterSurface->m_pvec3dNormals[i-1][1];
			norm[3*i+2] = this->vtData[(i-1)*vtDim+5];//-waterSurface->m_pvec3dNormals[i-1][2];
		}
		obj_model->normals = norm;
	}
	else
	{
		obj_model->numnormals = 0;
		obj_model->normals = NULL;
	}

	obj_model->numtexcoords = 0;
	obj_model->texcoords = NULL;

	obj_model->numfacetnorms = 0;
	obj_model->facetnorms = NULL;

	obj_model->numtriangles = this->triNum;
	GLMtriangle* tri = new GLMtriangle[this->triNum];
	for( int i=0; i<this->triNum; i++)
	{
		tri[i].vindices[0] = this->triangles[3*i]+1;
		tri[i].vindices[1] = this->triangles[3*i+1]+1;
		tri[i].vindices[2] = this->triangles[3*i+2]+1;

		if(haveNormalInfo())
		{
			tri[i].nindices[0] = this->triangles[3*i]+1;
			tri[i].nindices[1] = this->triangles[3*i+1]+1;
			tri[i].nindices[2] = this->triangles[3*i+2]+1;
		}
	}
	obj_model->triangles = tri;

	obj_model->nummaterials = 0;
	obj_model->materials = NULL;

	obj_model->numgroups = 1;
	GLMgroup* m_group = new GLMgroup();
	m_group->name = __glmStrdup("surface");
	m_group->numtriangles = this->triNum;
	GLuint* triIdx = new GLuint[this->triNum];
	for( int i=0; i<this->triNum; i++)
	{
		triIdx[i] = i;
	}
	m_group->triangles = triIdx;
	m_group->next = NULL;
	m_group->material = 0;
	obj_model->groups = m_group;

	obj_model->position[0] = 0;
	obj_model->position[1] = 0;
	obj_model->position[2] = 0;

	return obj_model;
}
#endif

template <class T>
int CSurface<T>::readFromIsoSurface(CIsoSurface<T> *surface_model)
{
	if( surface_model->m_nVertices != surface_model->m_nNormals )
	{
		printf("The vertex num does not equal to normal num! The program needs to be changed a little bit\n");
		return -1;
	}
	this->freeMemory();

	this->color = (surface_model->m_ppt3dColors != NULL);
	this->normal = true;
	this->vtDim = (surface_model->m_ppt3dColors == NULL)?6:9;
	this->vtNum = surface_model->m_nVertices;
	this->vtData = new T[vtNum*vtDim];
	for( int i=0; i<vtNum; i++ )
	{
		vtData[vtDim*i] = surface_model->m_ppt3dVertices[i][0];
		vtData[vtDim*i+1] = surface_model->m_ppt3dVertices[i][1];
		vtData[vtDim*i+2] = surface_model->m_ppt3dVertices[i][2];

		vtData[vtDim*i+3] = surface_model->m_pvec3dNormals[i][0];
		vtData[vtDim*i+4] = surface_model->m_pvec3dNormals[i][1];
		vtData[vtDim*i+5] = surface_model->m_pvec3dNormals[i][2];

		if( surface_model->m_ppt3dColors != NULL )
		{
			vtData[vtDim*i+6] = surface_model->m_ppt3dColors[i][0];
			vtData[vtDim*i+7] = surface_model->m_ppt3dColors[i][1];
			vtData[vtDim*i+8] = surface_model->m_ppt3dColors[i][2];
		}
	}

	this->triNum = surface_model->m_nTriangles;
	this->triangles = new int[triNum*3];
	for( int i=0; i<triNum; i++ )
	{
		this->triangles[3*i] = surface_model->m_piTriangleIndices[3*i];
		this->triangles[3*i+1] = surface_model->m_piTriangleIndices[3*i+1];
		this->triangles[3*i+2] = surface_model->m_piTriangleIndices[3*i+2];
	}
	return 1;
}

template <class T>
CSurface<T>* cloneCSurface(CSurface<T> const* surf)
{
	if(surf == NULL)
		return NULL;

	CSurface *ret = new CSurface();
	*ret = *surf;
	//int *triangles = new int[surf->triNum*3];
	//memcpy(triangles, surf->triangles, surf->triNum*3*sizeof(int));
	//T *vtData = new T[surf->vtNum*surf->vtDim];
	//memcpy(vtData, surf->vtData, surf->vtNum*surf->vtDim*sizeof(T));
	//ret->triNum = surf->triNum;
	//ret->vtNum = surf->vtNum;
	//ret->vtDim = surf->vtDim;
	//ret->color = surf->color;
	//ret->normal = surf->normal;
	//ret->triangles = triangles;
	//ret->vtData = vtData;
	return ret;
}


#endif
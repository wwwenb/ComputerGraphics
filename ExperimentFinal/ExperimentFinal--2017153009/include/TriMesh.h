#ifndef _TRI_MESH_H_
#define _TRI_MESH_H_

#include "Angel.h"

#include <vector>
#include <string>
#include <fstream>
#include<assert.h>

typedef struct Index {
    unsigned int a,b,c;

    Index(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

typedef struct vec3 vec3f;

class MatrixStack {
	int    _index;
	int    _size;
	mat4* _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;

	}

	mat4& pop(void) {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};

class TriMesh
{
public:

    TriMesh() {}
    ~TriMesh() {}

    std::vector<vec3f> v() const { return vertices; }
    std::vector<vec3i> f() const { return faces; }

    //////////////////////////////////////////////////////////////////////////

    void read_off(const std::string filename) {
	if (filename.empty()) return;

	std::ifstream in;
	in.open(filename.c_str(), std::ios::in);

	// read first line
	std::string off;
	int nE;	
	in >> off >> nV >> nF >> nE;

	// read vertices
	for (int i = 0; i < nV; ++i) {
	    float x, y, z;
	    in >> x >> y >> z;
	    vertices.push_back(vec3f(x, y, z));
	}

	// read indices
	for (int i = 0; i < nF; ++i) {
	    int n, a, b, c;
	    in >> n >> a >> b >> c;
	    faces.push_back(vec3i(a, b, c));
	}

	in.close();
    }

    void write_off(const std::string filename)  const {
	if (filename.empty()) return;
	if (nV == 0 || nF == 0) return;

	std::ofstream out;
	out.open(filename.c_str(), std::ios::out);

	// write first line
	out << "OFF" << "\n" << nV << " " << nF << " " << 0 << "\n";

	// write vertices
	for (int i = 0; i < nV; ++i) {
	    vec3f v = vertices[i];
	    out << v[0] << " " << v[1] << " " << v[2] << "\n";
	}

	// write indices
	for (int i = 0; i < nF; ++i) {
	    vec3i idx = faces[i];
	    out << 3 << " " << idx.a << " " << idx.b << " " << idx.c << "\n";
	}

	out.close();	
    }

private:

    std::string filename;
    std::vector<vec3f> vertices;
    std::vector<vec3i> faces;

    int nV;
    int nF;
};

#endif